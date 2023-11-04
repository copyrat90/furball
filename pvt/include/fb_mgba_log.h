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

void fb_mgba_printf(fb_mgba_log_level, const char* str, ...);

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

#ifdef __cplusplus
}
#endif

#endif // FB_MGBA_LOG_H
