#ifndef FB_INSTRUMENT_H
#define FB_INSTRUMENT_H

#include <stdbool.h>
#include <stdint.h>

#define FB_GB_SOUND_LENGTH_INFINITY 64

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum fb_gb_hw_cmd_kind_
{
    FB_GB_HW_CMD_KIND_ENVELOP,
    FB_GB_HW_CMD_KIND_SWEEP,
    FB_GB_HW_CMD_KIND_WAIT,
    FB_GB_HW_CMD_KIND_WAIT_FOR_RELEASE,
    FB_GB_HW_CMD_KIND_LOOP,
    FB_GB_HW_CMD_KIND_LOOP_UNTIL_RELEASE,
} fb_gb_hw_cmd_kind;

typedef struct fb_gb_hw_cmd_envelop_
{
    const uint8_t volume;         // [0..15]
    const uint8_t envelop_length; // [0..7]
    const uint8_t sound_length;   // [0..63], infinity=64 (FB_GB_SOUND_LENGTH_INFINITY)
    const bool direction_up;
} fb_gb_hw_cmd_envelop;

typedef struct fb_gb_hw_cmd_sweep_
{
    const uint8_t shift; // [0..7]
    const uint8_t speed; // [0..7]
    const bool direction_down;
} fb_gb_hw_cmd_sweep;

typedef struct fb_gb_hw_cmd_wait_
{
    const uint16_t length; // [1..256]
} fb_gb_hw_cmd_wait;

typedef struct fb_gb_hw_cmd_loop_
{
    const uint16_t position;
} fb_gb_hw_cmd_loop;

typedef struct fb_gb_hw_cmd_loop_until_release_
{
    const uint16_t position;
} fb_gb_hw_cmd_loop_until_release;

typedef struct fb_gb_hw_cmd_
{
    const fb_gb_hw_cmd_kind kind;

    const union {
        const fb_gb_hw_cmd_envelop envelop;
        const fb_gb_hw_cmd_sweep sweep;
        const fb_gb_hw_cmd_wait wait;
        const fb_gb_hw_cmd_loop loop;
        const fb_gb_hw_cmd_loop_until_release loop_until_release;
    };
} fb_gb_hw_cmd;

typedef struct fb_inst_gb_
{
    const uint8_t initial_volume; // [0..15]
    const uint8_t envelop_length; // [0..7]
    const uint8_t sound_length;   // [0..63], infinity=64 (FB_GB_SOUND_LENGTH_INFINITY)
    const bool envelop_direction_up;
    const bool always_init_envelop;
    const bool software_envelop;

    const uint8_t hardware_sequence_length; // [0..255]
    const fb_gb_hw_cmd *const hardware_sequence;
} fb_inst_gb;

typedef enum fb_macro_kind_
{
    FB_MACRO_KIND_VOL,
    FB_MACRO_KIND_ARP,
    FB_MACRO_KIND_DUTY,
    FB_MACRO_KIND_WAVE,
    // if DMG, hard-pan for both L/R [0..3]
    // if PCM, soft-pan
    FB_MACRO_KIND_PAN_L,
    // only PCM, soft-pan
    FB_MACRO_KIND_PAN_R,
    FB_MACRO_KIND_PITCH,
    FB_MACRO_KIND_PHASE_RESET,
} fb_macro_kind;

typedef struct fb_inst_macro_
{
    const fb_macro_kind kind;
    // checkbox
    // PITCH: Relative
    // SOFT_PAN: Qsound
    const uint8_t mode;

    const uint8_t length;      // [0..255]
    const uint8_t loop_pos;    // 0xFF: no loop
    const uint8_t release_pos; // 0xFF: no release
    const uint8_t delay;       // [0..255]
    const uint8_t speed;       // [1..255]

    // VOL: uint8_t[]      (DMG soft_env: [0..15])
    //                     (PCM: [0..64])
    // ARP: {i8, bool}[]   ([-120..120], fixed arp?)
    // DUTY: uint8_t[]     ([0..3])
    // WAVE: uint8_t[]     ([0..255])
    // PAN_*: int8_t[]     (DMG [0..3])
    //                     (PCM [0..127])
    //                     (PCM Qsound [-16..16])
    // PITCH: int16_t[]    ([-2048..2047])
    // PHASE_RESET: bool[]
    const void *const data;
} fb_inst_macro;

typedef enum fb_wave_synth_kind_
{
    // Single-waveform
    FB_WAVE_SYNTH_KIND_NONE = 0,
    FB_WAVE_SYNTH_KIND_INVERT,
    FB_WAVE_SYNTH_KIND_ADD,
    FB_WAVE_SYNTH_KIND_SUBTRACT,
    FB_WAVE_SYNTH_KIND_AVERAGE,
    FB_WAVE_SYNTH_KIND_PHASE,
    FB_WAVE_SYNTH_KIND_CHORUS,

    // Dual-waveform
    FB_WAVE_SYNTH_KIND_NONE_DUAL = 128,
    FB_WAVE_SYNTH_KIND_WIPE,
    FB_WAVE_SYNTH_KIND_FADE,
    FB_WAVE_SYNTH_KIND_PING_PONG,
    FB_WAVE_SYNTH_KIND_OVERLAY,
    FB_WAVE_SYNTH_KIND_NEGATIVE_OVERLAY,
    FB_WAVE_SYNTH_KIND_SLIDE,
    FB_WAVE_SYNTH_KIND_MIX,
    FB_WAVE_SYNTH_KIND_PHASE_MOD,
} fb_wave_synth_kind;

typedef struct fb_inst_wave_synth_
{
    const fb_wave_synth_kind kind;
    const bool global;

    const uint8_t wave_1;
    const uint8_t wave_2;

    const uint8_t rate_divider;
    const uint8_t speed; // actual speed = speed - 1

    const uint8_t amount;
    const uint8_t power; // only used for Phase Modulation
} fb_inst_wave_synth;

typedef struct fb_inst_sample_
{
    const char todo; // TODO
} fb_inst_sample;

typedef enum fb_inst_kind_
{
    FB_INST_KIND_GB,
    FB_INST_KIND_SAMPLE,
} fb_inst_kind;

typedef struct fb_instrument_
{
    const fb_inst_kind kind;
    const uint8_t macros_count;

    const fb_inst_gb *const gb;
    const fb_inst_macro *const macros; // array
    const fb_inst_wave_synth *const wave_synth;
    const fb_inst_sample *const sample;
} fb_instrument;

#ifdef __cplusplus
}
#endif

#endif // FB_INSTRUMENT_H
