/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This file was generated by cantools version 36.0.0 Fri Apr 16 01:29:27 2021.
 */

#include <string.h>

#include "Msg.h"

static inline uint8_t pack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_right_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t pack_right_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint16_t unpack_left_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) << shift);
}

static inline uint32_t unpack_left_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) << shift);
}

static inline uint8_t unpack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) >> shift);
}

static inline uint16_t unpack_right_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) >> shift);
}

static inline uint32_t unpack_right_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) >> shift);
}

int Msg_control_cmd1_unpack(
    struct Msg_control_cmd1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t position_change;
    uint16_t torque_feedforward;

    if (size < 7u) {
        return (-EINVAL);
    }

    dst_p->crc8_cmd1 = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->target_motor_id_cmd1 = unpack_right_shift_u8(src_p[1], 0u, 0x03u);
    dst_p->target_mode = unpack_right_shift_u8(src_p[1], 2u, 0x0cu);
    dst_p->counter_cmd1 = unpack_right_shift_u8(src_p[1], 4u, 0xf0u);
    position_change = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    position_change |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->position_change = (int16_t)position_change;
    torque_feedforward = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    torque_feedforward |= unpack_left_shift_u16(src_p[5], 8u, 0x07u);

    if ((torque_feedforward & (1u << 10)) != 0u) {
        torque_feedforward |= 0xf800u;
    }

    dst_p->torque_feedforward = (int16_t)torque_feedforward;
    dst_p->torque_closeloop_max = unpack_right_shift_u16(src_p[5], 3u, 0xf8u);
    dst_p->torque_closeloop_max |= unpack_left_shift_u16(src_p[6], 5u, 0x1fu);

    return (0);
}

bool Msg_control_cmd1_crc8_cmd1_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

bool Msg_control_cmd1_target_motor_id_cmd1_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

bool Msg_control_cmd1_target_mode_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

bool Msg_control_cmd1_counter_cmd1_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

bool Msg_control_cmd1_position_change_is_in_range(int16_t value)
{
    return ((value >= -28672) && (value <= 28672));
}

bool Msg_control_cmd1_torque_feedforward_is_in_range(int16_t value)
{
    return ((value >= -974) && (value <= 974));
}

bool Msg_control_cmd1_torque_closeloop_max_is_in_range(uint16_t value)
{
    return (value <= 974u);
}

int Msg_limits_cmd2_unpack(
    struct Msg_limits_cmd2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 6u) {
        return (-EINVAL);
    }

    dst_p->crc8_cmd2 = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->counter_cmd2 = unpack_right_shift_u8(src_p[1], 4u, 0xf0u);
    dst_p->velocity_limit = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->velocity_limit |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->accel_limit = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    dst_p->accel_limit |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);

    return (0);
}

bool Msg_limits_cmd2_crc8_cmd2_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

bool Msg_limits_cmd2_counter_cmd2_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

bool Msg_limits_cmd2_velocity_limit_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

bool Msg_limits_cmd2_accel_limit_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int Msg_control_status1_pack(
    uint8_t *dst_p,
    const struct Msg_control_status1_t *src_p,
    size_t size)
{
    uint16_t position_error;
    uint16_t torque_actual;
    uint16_t torque_close_loop_actual;
    uint8_t speed_actual;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->crc8_stat1, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->counter_stat1, 4u, 0xf0u);
    position_error = (uint16_t)src_p->position_error;
    dst_p[2] |= pack_left_shift_u16(position_error, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(position_error, 8u, 0xffu);
    torque_actual = (uint16_t)src_p->torque_actual;
    dst_p[4] |= pack_left_shift_u16(torque_actual, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(torque_actual, 8u, 0x07u);
    torque_close_loop_actual = (uint16_t)src_p->torque_close_loop_actual;
    dst_p[5] |= pack_left_shift_u16(torque_close_loop_actual, 3u, 0xf8u);
    dst_p[6] |= pack_right_shift_u16(torque_close_loop_actual, 5u, 0x3fu);
    speed_actual = (uint8_t)src_p->speed_actual;
    dst_p[7] |= pack_left_shift_u8(speed_actual, 0u, 0xffu);

    return (8);
}


bool Msg_control_status1_crc8_stat1_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

bool Msg_control_status1_counter_stat1_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

bool Msg_control_status1_position_error_is_in_range(int16_t value)
{
    return ((value >= -28672) && (value <= 28672));
}

bool Msg_control_status1_torque_actual_is_in_range(int16_t value)
{
    return ((value >= -779) && (value <= 779));
}

bool Msg_control_status1_torque_close_loop_actual_is_in_range(int16_t value)
{
    return ((value >= 0) && (value <= 779));
}

bool Msg_control_status1_speed_actual_is_in_range(int8_t value)
{
    return ((value >= -125) && (value <= 125));
}

int Msg_system_status2_pack(
    uint8_t *dst_p,
    const struct Msg_system_status2_t *src_p,
    size_t size)
{
    uint32_t position_raw;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->crc8_stat2, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->counter_stat2, 4u, 0xf0u);
    dst_p[2] |= pack_left_shift_u8(src_p->chip_temp, 0u, 0xffu);
    position_raw = (uint32_t)src_p->position_raw;
    dst_p[3] |= pack_left_shift_u32(position_raw, 0u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(position_raw, 8u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(position_raw, 16u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(position_raw, 24u, 0xffu);

    return (8);
}


bool Msg_system_status2_crc8_stat2_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

bool Msg_system_status2_counter_stat2_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

bool Msg_system_status2_chip_temp_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

bool Msg_system_status2_position_raw_is_in_range(int32_t value)
{
    (void)value;

    return (true);
}
