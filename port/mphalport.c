/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Armink (armink.ztl@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include <rtthread.h>
#include <py/mpconfig.h>
#include <py/runtime.h>
#include "mphalport.h"
#include "rtt_getchar.h"

int mp_hal_stdin_rx_chr(void) {
    char ch;
    while (1) {
        ch = rtt_getchar();
        if (ch != 0xFF) {
            break;
        }
        MICROPY_EVENT_POLL_HOOK;
        rt_thread_delay(1);
    }
    return ch;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    rt_device_t console;

    console = rt_console_get_device();
    if (console) {
        rt_device_write(console, 0, str, len);
    }
}

mp_uint_t mp_hal_ticks_us(void) {
    return rt_tick_get() * 1000000UL / RT_TICK_PER_SECOND;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return rt_tick_get() * 1000 / RT_TICK_PER_SECOND;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return rt_tick_get();
}

void mp_hal_delay_us(mp_uint_t us) {
    rt_tick_t t0 = rt_tick_get(), t1, dt;
    uint64_t dtick = us * RT_TICK_PER_SECOND / 1000000L;
    while (1) {
        t1 = rt_tick_get();
        dt = t1 - t0;
        if (dt >= dtick) {
            break;
        }
        mp_handle_pending();
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    rt_tick_t t0 = rt_tick_get(), t1, dt;
    uint64_t dtick = ms * RT_TICK_PER_SECOND / 1000L;
    while (1) {
        t1 = rt_tick_get();
        dt = t1 - t0;
        if (dt >= dtick) {
            break;
        }
        MICROPY_EVENT_POLL_HOOK;
        rt_thread_delay(1);
    }
}
