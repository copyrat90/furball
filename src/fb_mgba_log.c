/*
 mgba.h
 Copyright (c) 2016 Jeffrey Pfau

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// This source is a modified version of https://github.com/mgba-emu/mgba/blob/master/opt/libgba/mgba.c
// modified by copyrat90

#include "fb_mgba_log.h"

#ifdef FB_MGBA_LOG_ENABLED
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define REG_MGBA_DEBUG_ENABLE *((volatile uint16_t *)0x4FFF780)
#define REG_MGBA_DEBUG_FLAGS *((volatile uint16_t *)0x4FFF700)
#define REG_MGBA_DEBUG_STRING ((char *)0x4FFF600)
#endif // FB_MGBA_LOG_ENABLED

void fb_mgba_printf(fb_mgba_log_level log_level, const char *str, ...)
{
#ifdef FB_MGBA_LOG_ENABLED
    REG_MGBA_DEBUG_ENABLE = 0xC0DE;
    va_list arg_ptr;
    va_start(arg_ptr, str);
    vsnprintf(REG_MGBA_DEBUG_STRING, 256, str, arg_ptr);
    va_end(arg_ptr);
    REG_MGBA_DEBUG_FLAGS = log_level | 256;
#else
    ((void)log_level);
    ((void)str);
#endif // FB_MGBA_LOG_ENABLED
}
