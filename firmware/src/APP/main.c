 /**
 * StepperServoCAN
 *
 * Copyright (c) 2020 Makerbase.
 * Copyright (C) 2018 MisfitTech LLC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <www.gnu.org/licenses/>.
 *
 */

#include "main.h"
#include "MKS.h"
#include "stepper_controller.h"
#include "encoder.h"
#include "board.h"
#include "can.h"
#include "control_api.h"
#include "calibration.h"
#include "nonvolatile.h"
#include "actuator_config.h"
#include "display.h"
#include "delay.h"
#include <stdio.h>

extern void initialise_monitor_handles(void); //semihosting


volatile bool runCalibration = false;
static void RunCalibration(void){
	StepperCtrl_enable(false);
	runCalibration = true; //set again depending who calls the function

	Set_Error_LED(true);

	bool err1 = !CalibrationTable_calValid();
	bool err2 = false;

	do{//assert errors
		err1 = !CalibrationTable_calValid();
		
		//print errors on after failed calibration
		if(err1){
			(void) printf("Calibration not set\n");
		}
		if (err2){
			(void) printf("Large deviation. Reposition the magnet\n");
			delay_ms(1000);
		}
		(void) printf("\n[Enter] to confirm start of the calibration..\n");
		// delay_ms(2000); //make sure the printf above reaches the host
		do{	//wait for the user
			Set_Func_LED(true);
			int c;
			c = getchar();
			// (void) printf("%c", c);
			if(c == '@')
				break;
		}while(!F1_button_state());
		Set_Func_LED(false);

		//print angle using fixed point
		uint16_t max_error = StepperCtrl_calibrateEncoder(false);
		float max_error_deg = ANGLERAW_T0_DEGREES(max_error);
		(void) printf("Max deviation was %01u.%02u deg\n", (uint16_t)max_error_deg, (uint16_t)((uint32_t)(max_error_deg*100.0)%100U));

		//assert errors
		err1 = !CalibrationTable_calValid();
		err2 = max_error > CALIBRATION_MAX_ERROR;

	}while(err1 || err2);
	Set_Error_LED(false);
	(void) printf("Calibration ok\n");

	runCalibration = false;
	StepperCtrl_enable(true);
}


volatile stepCtrlError_t stepCtrlError = STEPCTRL_NO_POWER;
static void Begin_process(void)
{
	
	update_actuator_parameters();

	board_init();	//set up the pins correctly on the board.

	nonvolatile_begin();

	validateAndInitNVMParams(); //systemParams init

	display_begin(); //display init
	Serivice_task_init(); //task init

	display_show("StepperServoCAN", "initialization..", "", "");
	delay_ms(10);
	stepCtrlError = STEPCTRL_NO_CAL;
	while(STEPCTRL_NO_ERROR != stepCtrlError)
	{
		Set_Error_LED(true);
		//start controller before accepting step inputs
		stepCtrlError = StepperCtrl_begin();

		//start up encoder
		if (STEPCTRL_NO_ENCODER == stepCtrlError)
		{
			display_show("Encoder", " Error!", "REBOOT", "");
			//slow red blink - encoder initialization fail - power down the board
			Set_Error_LED(false);
			delay_ms(1000);
			Set_Error_LED(true);
			delay_ms(1000);
		}else if(STEPCTRL_NO_POWER == stepCtrlError)
		{
			display_show("Waiting", "MOTOR", "POWER", "");
			//interrupted red led (as if it is retrying) - waiting for power
			delay_ms(1000);
			Set_Error_LED(false);
		}else if(STEPCTRL_NO_CAL == stepCtrlError)
		{
			display_show("   NOT ", "Calibrated", " ", "");
			delay_ms(2200);
			display_process();
			RunCalibration();
		}else{
			Set_Error_LED(false);
		}
	}
	Set_Error_LED(false);

	display_setMenu(MenuMain);
	printf("Initialization successful\n");
	

	printf("Starting motion task\n");
	StepperCtrl_enable(true);
	
}


static void Background_process(void){
	if(runCalibration){
		RunCalibration();
	}
}

//fast motor control task
//rolling counters for debugging
volatile uint32_t motion_task_counter=0;	// cppcheck-suppress  misra-c2012-8.4
volatile uint32_t service_task_counter=0;	// cppcheck-suppress  misra-c2012-8.4
void Motion_task(void){
	motion_task_counter++;

	(void) StepperCtrl_processMotion(); //handle the control loop
}

//10ms task for communication and diagnostic
void Service_task(void){
	service_task_counter++;

	ChipTemp_adc_update();
	Vmot_adc_update();
	Vbat_adc_update();
	LSS_adc_update();

	//transmit CAN every 10ms
	CAN_TransmitMotorStatus(service_task_counter);

	//go to Soft Off if motor is actively controlled but control signal is not received
	bool comm_error = false;
	if(enableFeedback){
		comm_error = (Check_Control_CAN_rx_validate_tick() == false);
	}
	if(comm_error)
	{	//once SOFT_TORQUE_OFF is set, the motor will not be controlled until STEPCTRL_OFF is requested
		StepperCtrl_setControlMode(STEPCTRL_FEEDBACK_SOFT_TORQUE_OFF);
	}

	//Function button and LED processing
	static uint16_t f1_button_count = 0; //centiseconds
	const uint16_t button_delay_calib = 200U;//hold 2s to trigger re-calibration
	if(F1_button_state() && (stepCtrlError == STEPCTRL_NO_ERROR)){//look for button long press
		f1_button_count++;
		StepperCtrl_setControlMode(STEPCTRL_FEEDBACK_SOFT_TORQUE_OFF);
	}
	if(f1_button_count == (button_delay_calib-10U))	{Set_Func_LED(true);} 	//short LED blink
	if(	f1_button_count == button_delay_calib)		{Set_Func_LED(false);}
	if((f1_button_count >= button_delay_calib)  && (!F1_button_state())){ 	//wait for button release
		runCalibration = true; 		//request calibration - it will run in Background_process()
	}
	if(!F1_button_state()){
		f1_button_count=0;
	}
}

int main (void)
{	
	#ifdef DEBUG
	initialise_monitor_handles();
	#endif
	
	Begin_process();
	while(1)
	{
		Background_process();
	}
}