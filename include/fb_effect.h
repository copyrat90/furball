#ifndef FB_EFFECT_H
#define FB_EFFECT_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum fb_effect_
{
    FB_EFFECT_JUMP_TO_PATTERN = 0x0B, // value: row
    FB_EFFECT_JUMP_TO_NEXT_PATTERN = 0x0D,

    FB_EFFECT_SET_DUTY_CYCLE = 0x12,

    FB_EFFECT_SEND_EXTERNAL_COMMAND = 0xEE,
    FB_EFFECT_STOP_SONG = 0xFF,
} fb_effect;

#ifdef __cplusplus
}
#endif

#endif // FB_EFFECT_H
