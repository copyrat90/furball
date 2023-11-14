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

// This source is a modified version of https://github.com/mgba-emu/mgba/blob/master/opt/libgba/mgba.h
// modified by copyrat90

#ifndef FB_MGBA_LOG_H
#define FB_MGBA_LOG_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    FB_MGBA_LOG_LEVEL_FATAL = 0,
    FB_MGBA_LOG_LEVEL_ERROR = 1,
    FB_MGBA_LOG_LEVEL_WARN = 2,
    FB_MGBA_LOG_LEVEL_INFO = 3,
    FB_MGBA_LOG_LEVEL_DEBUG = 4,
} fb_mgba_log_level;

void fb_mgba_printf(fb_mgba_log_level, const char *str, ...);

#ifdef FB_MGBA_LOG_ENABLED
#define FB_LOG(log_level, ...) fb_mgba_printf(log_level, __VA_ARGS__)
#else
#define FB_LOG(log_level, ...) ((void)0)
#endif // FB_MGBA_LOG_ENABLED

#define FB_LOG_FATAL(...) FB_LOG(FB_MGBA_LOG_LEVEL_FATAL, __VA_ARGS__)
#define FB_LOG_ERROR(...) FB_LOG(FB_MGBA_LOG_LEVEL_ERROR, __VA_ARGS__)
#define FB_LOG_WARN(...) FB_LOG(FB_MGBA_LOG_LEVEL_WARN, __VA_ARGS__)
#define FB_LOG_INFO(...) FB_LOG(FB_MGBA_LOG_LEVEL_INFO, __VA_ARGS__)
#define FB_LOG_DEBUG(...) FB_LOG(FB_MGBA_LOG_LEVEL_DEBUG, __VA_ARGS__)

#ifdef FB_MGBA_LOG_MAY_FATAL_CRASH
#define FB_LOG_MAY_FATAL(...) FB_LOG_FATAL(__VA_ARGS__)
#else
#define FB_LOG_MAY_FATAL(...) FB_LOG_ERROR(__VA_ARGS__)
#endif // FB_MGBA_LOG_MAY_FATAL_CRASH

#ifdef FB_MGBA_LOG_ENABLED
#define FB_ASSERT(condition, ...) \
    if (!(condition)) \
    FB_LOG_FATAL(__VA_ARGS__)
#else
#define FB_ASSERT(condition, ...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif // FB_MGBA_LOG_H
