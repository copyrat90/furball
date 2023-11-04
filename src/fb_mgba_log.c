#include "fb_mgba_log.h"

#ifdef FB_MGBA_LOG_ENABLED
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define REG_MGBA_DEBUG_ENABLE *((volatile uint16_t*)0x4FFF780)
#define REG_MGBA_DEBUG_FLAGS *((volatile uint16_t*)0x4FFF700)
#define REG_MGBA_DEBUG_STRING ((char*)0x4FFF600)
#endif // FB_MGBA_LOG_ENABLED

void fb_mgba_printf(fb_mgba_log_level log_level, const char* str, ...)
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
