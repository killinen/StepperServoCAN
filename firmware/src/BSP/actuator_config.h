/**
 * StepperServoCAN
 *
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


/**
 * @ Description:
 * Quick calculation of actuator coefficients.
 */

#ifndef ACTUATOR_CONFIG_H
#define ACTUATOR_CONFIG_H

#include <stdint.h>

extern const uint16_t rated_current; //mA
extern const uint16_t rated_torque; //cNm

//calculate actuator parameters to be used by control_api 
extern volatile float gearing_ratio;
extern volatile float actuatorTq_to_current;
extern volatile float current_to_actuatorTq;

extern const int8_t anticogging_factor;

void update_actuator_parameters(void);


#endif // ACTUATOR_CONFIG_H
