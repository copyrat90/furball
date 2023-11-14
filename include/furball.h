#ifndef FURBALL_H
#define FURBALL_H

#include <stdint.h>

#include "fb_settings.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct fb_music_ fb_music;

typedef enum fb_play_status_
{
    FB_PLAY_STATUS_STOP,
    FB_PLAY_STATUS_PLAY,
    FB_PLAY_STATUS_PAUSE,
} fb_play_status;

typedef struct fb_positions_
{
    int16_t order; // 0x7FFF=empty, 0x7FF0=stop song
    int16_t row;   // 0x7FFF=empty
} fb_positions;

/// @brief Initialize Furball.
/// This should be called before using any other Furball functions.
/// After calling this, the sound registers Furball use are managed, so DO NOT touch them.
/// @param init_settings Furball initialize settings. if `NULL`, default settings are used.
/// @return whether the Furball is successfully initialized or not.
bool fb_init(const fb_init_settings *init_settings);

/// @brief Shutdown Furball.
/// This releases the sound registers from the managed state.
/// @param shutdown_settings Furball shutdown settings. if `NULL`, default settings are used.
void fb_shutdown(const fb_shutdown_settings *shutdown_settings);

/// @return whether Furball is initialized or not.
bool fb_initialized(void);

/// @brief Starts to play a music.
/// @param music pointer to a music stored in ROM.
/// @param loop_setting loop setting, see `fb_loop_setting`
void fb_play(const fb_music *music, fb_loop_setting loop_setting);

/// @brief Pauses the music.
void fb_pause(void);

/// @brief Resumes the music.
void fb_resume(void);

/// @brief Stops the music.
void fb_stop(void);

/// @brief Updates the furball playback.
/// This should be called once in each VBlank Interrupt.
void fb_update_vblank(void);

/// @return Get the current play status.
fb_play_status fb_get_play_status(void);

/// @brief Get the currently playing music.
/// @return pointer to the currently playing music, or `NULL` if not playing.
const fb_music *fb_get_playing_music(void);

/// @return Get the current order, or `-1` if not playing.
int fb_get_order(void);

/// @return Get the current row in pattern, or `-1` if not playing.
int fb_get_row(void);

#ifdef __cplusplus
}
#endif

#endif // FURBALL_H
