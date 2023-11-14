#ifndef FB_SETTINGS_H
#define FB_SETTINGS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// @brief DMG and/or DirectSound usage
typedef enum fb_init_channels_
{
    FB_INIT_CHANNELS_NONE = 0,

    FB_INIT_CHANNELS_DMG_CH1 = (1 << 0),
    FB_INIT_CHANNELS_DMG_CH2 = (1 << 1),
    FB_INIT_CHANNELS_DMG_CH3 = (1 << 2),
    FB_INIT_CHANNELS_DMG_CH4 = (1 << 3),
    // FB_INIT_CHANNELS_DIRECTSOUND_A = (1 << 4),
    // FB_INIT_CHANNELS_DIRECTSOUND_B = (1 << 5),

    FB_INIT_CHANNELS_DMG =
        (FB_INIT_CHANNELS_DMG_CH1 | FB_INIT_CHANNELS_DMG_CH2 | FB_INIT_CHANNELS_DMG_CH3 | FB_INIT_CHANNELS_DMG_CH4),
    // FB_INIT_CHANNELS_DIRECTSOUND =
    //    (FB_INIT_CHANNELS_DIRECTSOUND_A | FB_INIT_CHANNELS_DIRECTSOUND_B),

    FB_INIT_CHANNELS_ALL = (FB_INIT_CHANNELS_DMG
                            // | FB_INIT_CHANNELS_DIRECTSOUND
    )
} fb_init_channels;

typedef struct fb_init_settings_
{
    // DMG and/or DirectSound usage.
    // (default is `FB_INIT_CHANNELS_ALL`)
    fb_init_channels channels;

    // TODO: Add DirectSound mixing rate settings

    // If `true`, save the master sound register values, so that they can be restored on `fb_shutdown()`.
    // Affected regs are `SNDSTAT`, `SNDDMGCNT`, `SNDDSCNT` and `SNDBIAS`.
    // Disable this if you're mixing Furball with other sound libraries at the same moment.
    // (default is `true`)
    bool save_master_sound_regs;
} fb_init_settings;

typedef struct fb_shutdown_settings_
{
    // If `true`, restore the master sound register values, if they were saved.
    // Affected regs are `SNDSTAT`, `SNDDMGCNT`, `SNDDSCNT` and `SNDBIAS`.
    // Disable this if you're mixing Furball with other sound libraries at the same moment.
    // (default is `true`)
    bool restore_master_sound_regs;
} fb_shutdown_settings;

typedef enum fb_loop_setting_
{
    // These settings respect the effects to loop or stop the music.
    FB_LOOP_SETTING_LOOP, // loops the music from the start at the end of the order.
    FB_LOOP_SETTING_STOP, // stops the music at the end of the order.

    // These settings ignore the effects to loop or stop the music.
    FB_LOOP_SETTING_FORCE_LOOP, // loops the music from the start at the end of the order or stop effect `FFxx`.
    FB_LOOP_SETTING_FORCE_STOP, // stops the music at the end of the order or effect to jump to previous pattern.
} fb_loop_setting;

#ifdef __cplusplus
}
#endif

#endif // FB_SETTINGS_H
