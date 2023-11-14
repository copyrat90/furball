#ifndef FB_NOTE_H
#define FB_NOTE_H

#ifdef __cplusplus
extern "C"
{
#endif

// https://github.com/tildearrow/furnace/blob/master/papers/format.md#pattern-157
typedef enum fb_note_
{
    FB_NOTE_c_5 = 0,
    FB_NOTE_C_0 = 60, // lowest in noise table
    FB_NOTE_B_1 = 83, // lowest in period table
    FB_NOTE_C_2 = 84,
    FB_NOTE_Gs5 = 120, // highest in noise table
    FB_NOTE_B_9 = 179, // highest in period table

    FB_NOTE_OFF = 180,
    FB_NOTE_NOTE_REL = 181,
    FB_NOTE_MACRO_REL = 182,

    FB_NOTE_EMPTY = 0xFF
} fb_note;

#ifdef __cplusplus
}
#endif

#endif // FB_NOTE_H
