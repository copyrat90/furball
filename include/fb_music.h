#ifndef FB_MUSIC_H
#define FB_MUSIC_H

#include "fb_instrument.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    const uint16_t width;  // 32 or 64
    const uint16_t height; // 16 (always)

    const uint32_t *const data; // len = width / 8
} fb_wavetable;

typedef struct
{
    // if `has_*` is false, it's not in `data`.
    const bool has_volume;
    const bool has_note;
    const bool has_instrument;
    const uint8_t max_effects_count; // [0..8]

    // volume = 2 bytes [0..256] (empty: `0xFFFF`)
    // note = 1 byte (empty: `0xFF`)
    // instrument = 1 byte [0..254] (empty: `0xFF`)
    // effect, val = 1 byte each (empty: `0xAA, 0xAA`)
    //
    // note value: https://github.com/tildearrow/furnace/blob/master/papers/format.md#pattern-157
    const uint8_t *const data;
} fb_pattern;

typedef struct
{
    const uint16_t instruments_count; // [0..254]
    const uint16_t wavetables_count;  // [0..255]

    const fb_instrument *const instruments; // shared across DMG & PCM
    const fb_wavetable *const wavetables; // only for ch3 (DMG WAV)

    const uint16_t order_length;   // [0..256]
    const uint16_t pattern_length; // [0..256]

    const fb_pattern *const *const ch1_order; // DMG PU1
    const fb_pattern *const *const ch2_order; // DMG PU2
    const fb_pattern *const *const ch3_order; // DMG WAV
    const fb_pattern *const *const ch4_order; // DMG NOI
} fb_music;

#ifdef __cplusplus
}
#endif

#endif // FB_MUSIC_H
