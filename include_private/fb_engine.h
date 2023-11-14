#ifndef FB_ENGINE_H
#define FB_ENGINE_H

#include "furball.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct fb_instrument_ fb_instrument;

typedef struct fb_master_snd_reg_vals_
{
    uint16_t sndstat;
    uint16_t snddmgcnt;
    uint16_t snddscnt;
    uint16_t sndbias;
} fb_master_snd_reg_vals;

typedef struct fb_engine_
{
    bool initialized;
    fb_init_settings settings;
    fb_master_snd_reg_vals prev_regs;
} fb_engine;

typedef struct fb_dmg_channel_
{
    const fb_instrument *inst;
    bool retrigger;

    // store current indexes of `inst->macros`
    struct
    {
        uint8_t vol;
        uint8_t arp;
        uint8_t duty;
        uint8_t wave;
        uint8_t pan;
        uint8_t pitch;
        uint8_t phase_reset;
    } macro_idxes;

    // Currently applied values
    uint16_t freq_base;
    uint16_t freq_diff; // for effects (`04xy`, `E5xx`, ...)

    uint8_t vol;     // [0..15]
    uint8_t env_len; // [0..7]
    uint8_t snd_len; // [0..64], infinity=64 (FB_GB_SOUND_LENGTH_INFINITY)
    bool dir_up;

    uint8_t duty; // [0..3] 12.5% / 25% / 50% / 75%
    uint8_t pan;  // [0..3] bit 0=right, bit 1=left
} fb_dmg_channel;

typedef struct fb_player_
{
    const fb_music *music;
    fb_play_status play_status;
    fb_loop_setting loop_setting;

    fb_dmg_channel dmg_channels[4];

    // speed can be changed mid-song, so we need these.
    uint8_t speeds[16];
    uint8_t speeds_length;

    uint8_t speed_idx;
    fb_positions pos;
    fb_positions jump_pos;

    int16_t speed_counter;
    int16_t vtempo_counter;
} fb_player;

#ifdef __cplusplus
}
#endif

#endif // FB_ENGINE_H
