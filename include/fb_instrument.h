#ifndef FB_INSTRUMENT_H
#define FB_INSTRUMENT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    FB_GB_HW_CMD_KIND_ENVELOP,
    FB_GB_HW_CMD_KIND_SWEEP,
    FB_GB_HW_CMD_KIND_WAIT,
    FB_GB_HW_CMD_KIND_WAIT_FOR_RELEASE,
    FB_GB_HW_CMD_KIND_LOOP,
    FB_GB_HW_CMD_KIND_LOOP_UNTIL_RELEASE,
} fb_gb_hw_cmd_kind;

typedef struct
{
    const uint8_t volume;         // [0..15]
    const uint8_t envelop_length; // [0..7]
    const uint8_t sound_length;   // [0..64], infinity=64
    const bool direction_up;
} fb_gb_hw_cmd_envelop;

typedef struct
{
    const uint8_t shift; // [0..7]
    const uint8_t speed; // [0..7]
    const bool direction_down;
} fb_gb_hw_cmd_sweep;

typedef struct
{
    const uint16_t length; // [1..256]
} fb_gb_hw_cmd_wait;

typedef struct
{
    const uint16_t position;
} fb_gb_hw_cmd_loop;

typedef struct
{
    const uint16_t position;
} fb_gb_hw_cmd_loop_until_release;

typedef struct
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

typedef struct
{
    const uint8_t initial_volume; // [0..15]
    const uint8_t envelop_length; // [0..7]
    const uint8_t sound_length;   // [0..64], infinity=64
    const bool envelop_direction_up;
    const bool always_init_envelop;
    const bool software_envelop;

    const uint8_t hardware_sequence_length; // [0..255]
    const fb_gb_hw_cmd *const hardware_sequence;
} fb_inst_gb;

typedef enum
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

typedef struct
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
    // ARP: int8_t[]       ([-120..120])
    // DUTY: uint8_t[]     ([0..3])
    // WAVE: uint8_t[]     ([0..255])
    // PAN_*: int8_t[]     (DMG [0..3])
    //                     (PCM [0..127])
    //                     (PCM Qsound [-16..16])
    // PITCH: int16_t[]    ([-2048..2047])
    // PHASE_RESET: bool[]
    const void *const data;
} fb_inst_macro;

typedef struct
{
    // TODO
} fb_inst_wave_synth;

typedef struct
{
    // TODO
} fb_inst_sample;

typedef enum
{
    FB_INST_KIND_GB,
    FB_INST_KIND_SAMPLE,
} fb_inst_kind;

typedef struct
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
