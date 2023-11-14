#ifndef FB_MATH_H
#define FB_MATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

static inline int32_t fb_clamp_s32(int32_t val, int32_t min_limit, int32_t max_limit)
{
    return val < min_limit ? min_limit : val > max_limit ? max_limit : val;
}

#ifdef __cplusplus
}
#endif

#endif
