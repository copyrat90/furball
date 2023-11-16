#include "furball.h"

#include <stddef.h>

#include "fb_effect.h"
#include "fb_engine.h"
#include "fb_gba_hardware.h"
#include "fb_math.h"
#include "fb_music.h"
#include "fb_note.h"

#define FB_JUMP_POS_STOP_SONG 0x7FF0
#define FB_JUMP_POS_EMPTY 0x7FFF
#define FB_CH3_SND_LEN 0xFE // Furnace limitation, `snd_len` of Ch3 is fixed

static void fb_process_row(void);

static const fb_init_settings default_init_settings = {
    .channels = FB_INIT_CHANNELS_ALL,
    .save_master_sound_regs = true,
};

static const fb_shutdown_settings default_shutdown_settings = {
    .restore_master_sound_regs = true,
};

static const fb_inst_gb default_inst_gb = {
    .initial_volume = 15,
    .envelop_length = 2,
    .sound_length = FB_GB_SOUND_LENGTH_INFINITY,
    .envelop_direction_up = false,
    .always_init_envelop = false,
    .software_envelop = false,
    .hardware_sequence_length = 0,
    .hardware_sequence = NULL,
};

static const fb_instrument default_gb_instrument = {
    .kind = FB_INST_KIND_GB,
    .macros_count = 0,
    .gb = &default_inst_gb,
    .macros = NULL,
    .wave_synth = NULL,
    .sample = NULL,
};

static const fb_dmg_channel init_dmg_channel = {
    .inst = &default_gb_instrument,
    .note_on = false,
    .retrigger = false,
    .envelop_initialized = false,
    .snd_len_enabled = false,
    .macro_idxes = {0},
    .freq_base = 1,
    .freq_diff = 0,
    .vol = 15,
    .env_len = 2,
    .snd_len = FB_GB_SOUND_LENGTH_INFINITY,
    .dir_up = false,
    .duty = 0,
    .pan = 2,
};

static const uint16_t dmg_period_table[1 + 12 * 8] = {
    // C  C#    D     D#    E     F     F#    G     G#    A     A#    B     // Furnace Octaves
    1,                                                                      // lower (B_1)
    44,   157,  263,  363,  457,  547,  631,  711,  786,  856,  923,  986,  // Oct 2
    1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517, // Oct 3
    1547, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783, // Oct 4
    1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915, // Oct 5
    1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982, // Oct 6
    1985, 1989, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015, // Oct 7
    2017, 2018, 2020, 2022, 2023, 2025, 2026, 2027, 2028, 2029, 2030, 2031, // Oct 8
    2032, 2033, 2034, 2035, 2036, 2036, 2037, 2038, 2038, 2039, 2039, 2040, // Oct 9
};

static const uint8_t dmg_noise_table[12 * 6] = {
    // C  C#    D     D#    E     F     F#    G     G#    A     A#    B     // Furnace Octaves
    0x00, 0xF7, 0xF6, 0xF5, 0xF4, 0xE7, 0xE6, 0xE5, 0xE4, 0xD7, 0xD6, 0xD5, // Oct 0
    0xD4, 0xC7, 0xC6, 0xC5, 0xC4, 0xB7, 0xB6, 0xB5, 0xB4, 0xA7, 0xA6, 0xA5, // Oct 1
    0xA4, 0x97, 0x96, 0x95, 0x94, 0x87, 0x86, 0x85, 0x84, 0x77, 0x76, 0x75, // Oct 2
    0x74, 0x67, 0x66, 0x65, 0x64, 0x57, 0x56, 0x55, 0x54, 0x47, 0x46, 0x45, // Oct 3
    0x44, 0x37, 0x36, 0x35, 0x34, 0x27, 0x26, 0x25, 0x24, 0x17, 0x16, 0x15, // Oct 4
    0x14, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,                   // Oct 5
};

static FB_EWRAM_BSS fb_engine engine;
static FB_EWRAM_BSS fb_player player;

bool fb_init(const fb_init_settings *const init_settings)
{
    // if already initialized, stop music before changing `engine.settings`
    if (engine.initialized)
        fb_stop();

    const fb_init_settings *const settings = (init_settings) ? init_settings : &default_init_settings;

    // don't allow no channel init
    if (settings->channels == FB_INIT_CHANNELS_NONE)
        return false;

    // save settings
    engine.settings = *settings;

    // save master sound register values (only on first init)
    if (!engine.initialized && engine.settings.save_master_sound_regs)
    {
        // simply save everything, we can selectively restore some flags anyways.
        engine.prev_regs.sndstat = FB_REG_SNDSTAT;
        engine.prev_regs.snddscnt = FB_REG_SNDDSCNT;
        engine.prev_regs.sndbias = FB_REG_SNDBIAS;

        // `SNDDMGCNT` can't be read when master sound is not enabled, so check that first.
        if (FB_REG_SNDSTAT & FB_SNDSTAT_MASTER_ENABLE)
            engine.prev_regs.snddmgcnt = FB_REG_SNDDMGCNT;
        else
            engine.prev_regs.snddmgcnt = 0;
    }

    // enable master sound first
    FB_REG_SNDSTAT = FB_SNDSTAT_MASTER_ENABLE;

    // set DMG master volume
    if (engine.settings.channels & FB_INIT_CHANNELS_DMG)
    {
        FB_REG_SNDDSCNT = (FB_SNDDSCNT_PSG_VOLUME_100 | (FB_REG_SNDDSCNT & ~FB_SNDDSCNT_PSG_VOLUME_MASK));
    }

    // Instead of here, DMG pannings are set on `fb_play()`,
    // since we need to reset the pannings for new music.

    // TODO: manipulate DirectSound sound register

    engine.initialized = true;
    return true;
}

void fb_shutdown(const fb_shutdown_settings *shutdown_settings)
{
    if (!engine.initialized)
        return;

    fb_stop();
    engine.initialized = false;

    if (shutdown_settings == NULL)
        shutdown_settings = &default_shutdown_settings;

    // restore master sound regs if requested
    if (shutdown_settings->restore_master_sound_regs && engine.settings.save_master_sound_regs)
    {
        FB_REG_SNDBIAS = engine.prev_regs.sndbias;
        FB_REG_SNDDSCNT = engine.prev_regs.snddscnt;
        FB_REG_SNDDMGCNT = engine.prev_regs.snddmgcnt;
        FB_REG_SNDSTAT = engine.prev_regs.sndstat & FB_SNDSTAT_MASTER_ENABLE;
    }
}

bool fb_initialized(void)
{
    return engine.initialized;
}

void fb_play(const fb_music *const music, const fb_loop_setting loop_setting)
{
    if (!engine.initialized)
        return;

    fb_stop();

    if (music == NULL)
        return;

    if (engine.settings.channels & FB_INIT_CHANNELS_DMG)
    {
        // Set default Ch3 waveform
        if (engine.settings.channels & FB_INIT_CHANNELS_DMG_CH3)
        {
            // avoid loud spike in Ch3 DAC
            // https://gbdev.io/pandocs/Audio_details.html#game-boy-advance-audio
            FB_REG_SNDDMGCNT &= ~(FB_SNDDMGCNT_PSG_3_ENABLE_LEFT | FB_SNDDMGCNT_PSG_3_ENABLE_RIGHT);

            FB_REG_SND3SEL = FB_SND3SEL_BANK_SET(1) | FB_SND3SEL_SIZE_32 | FB_SND3SEL_ENABLE;
            for (int i = 0; i < 4; ++i)
                FB_REG_WAVE_RAM[i] = music->wavetables[0].data[i];
            FB_REG_SND3SEL ^= FB_SND3SEL_BANK_SET(1);
        }

        // Reset DMG pannings
        const uint16_t mask = (engine.settings.channels & FB_INIT_CHANNELS_DMG);
        FB_REG_SNDDMGCNT |=
            ((mask << 12) | (mask << 8) | FB_SNDDMGCNT_PSG_VOL_LEFT_SET(7) | FB_SNDDMGCNT_PSG_VOL_RIGHT_SET(7));
    }

    player.music = music;
    player.loop_setting = loop_setting;

    for (int i = 0; i < 4; ++i)
        player.dmg_channels[i] = init_dmg_channel;

    // speed can be changed mid-song, so copy the speed values
    player.speeds_length = music->speeds_length;
    for (int i = 0; i < player.speeds_length; ++i)
        player.speeds[i] = music->speeds[i];

    player.speed_idx = 0;
    player.pos.order = 0;
    player.pos.row = -1; // before first row

    player.jump_pos.order = FB_JUMP_POS_EMPTY; // no jump
    player.jump_pos.row = FB_JUMP_POS_EMPTY;   // no jump

    player.speed_counter = player.speeds[0] - 1;
    player.vtempo_counter = music->virtual_tempo_denominator - music->virtual_tempo_numerator;

    player.play_status = FB_PLAY_STATUS_PLAY;
}

// This should only silence Furball managed channels
static void silence_channels(const fb_init_channels channels)
{
    // Silence active DMG channels without audio pops (keep DACs on)
    // https://gbdev.io/pandocs/Audio_details.html#mixer
    if ((channels & FB_INIT_CHANNELS_DMG_CH1) && (engine.settings.channels & FB_INIT_CHANNELS_DMG_CH1))
    {
        FB_REG_SND1CNT = (FB_REG_SND1CNT & 0x00FF) |
                         (FB_SND1CNT_ENV_VOLUME_SET(0) | FB_SND1CNT_ENV_DIR_INC | FB_SND1CNT_ENV_STEP_TIME_SET(0));
        FB_REG_SND1FREQ = FB_SND1FREQ_RETRIG;
    }
    if ((channels & FB_INIT_CHANNELS_DMG_CH2) && (engine.settings.channels & FB_INIT_CHANNELS_DMG_CH2))
    {
        FB_REG_SND2CNT = (FB_REG_SND2CNT & 0x00FF) |
                         (FB_SND2CNT_ENV_VOLUME_SET(0) | FB_SND2CNT_ENV_DIR_INC | FB_SND2CNT_ENV_STEP_TIME_SET(0));
        FB_REG_SND2FREQ = FB_SND2FREQ_RETRIG;
    }
    if ((channels & FB_INIT_CHANNELS_DMG_CH3) && (engine.settings.channels & FB_INIT_CHANNELS_DMG_CH3))
    {
        FB_REG_SND3CNT = (FB_REG_SND3CNT & 0x00FF) | (FB_SND3CNT_VOLUME_0);
        FB_REG_SND3FREQ = FB_SND3FREQ_RETRIG;
    }
    if ((channels & FB_INIT_CHANNELS_DMG_CH4) && (engine.settings.channels & FB_INIT_CHANNELS_DMG_CH4))
    {
        FB_REG_SND4CNT = (FB_REG_SND4CNT & 0x00FF) |
                         (FB_SND4CNT_ENV_VOLUME_SET(0) | FB_SND4CNT_ENV_DIR_INC | FB_SND4CNT_ENV_STEP_TIME_SET(0));
        FB_REG_SND4FREQ = FB_SND4FREQ_RETRIG;
    }

    // TODO: Silence Furball managed DirectSound channels
}

void fb_stop(void)
{
    if (!engine.initialized || player.play_status == FB_PLAY_STATUS_STOP)
        return;

    player.play_status = FB_PLAY_STATUS_STOP;

    silence_channels(FB_INIT_CHANNELS_DMG);
}

void fb_update_vblank(void)
{
    if (!engine.initialized || player.play_status != FB_PLAY_STATUS_PLAY)
        return;

    // process a row if enough ticks have been passed
    player.vtempo_counter += player.music->virtual_tempo_numerator;
    while (player.vtempo_counter >= player.music->virtual_tempo_denominator)
    {
        player.vtempo_counter -= player.music->virtual_tempo_denominator;

        // next row in pattern
        if (++player.speed_counter >= player.speeds[player.speed_idx])
        {
            player.speed_idx = (player.speed_idx + 1) % player.speeds_length;
            player.speed_counter = 0;

            // reserved stop
            if (player.jump_pos.order == FB_JUMP_POS_STOP_SONG)
            {
                fb_stop();
                return;
            }
            // reserved jump
            if (player.jump_pos.order != FB_JUMP_POS_EMPTY)
            {
                player.pos.order = player.jump_pos.order;
                player.pos.row = player.jump_pos.row - 1; // before

                // reset jump pos
                player.jump_pos.order = FB_JUMP_POS_EMPTY;
                player.jump_pos.row = FB_JUMP_POS_EMPTY;
            }

            // next pattern in order
            if (++player.pos.row >= player.music->pattern_length)
            {
                player.pos.row = 0;

                if (++player.pos.order >= player.music->order_length)
                {
                    if (player.loop_setting == FB_LOOP_SETTING_LOOP ||
                        player.loop_setting == FB_LOOP_SETTING_FORCE_LOOP)
                    {
                        player.pos.order = 0;
                    }
                    else
                    {
                        fb_stop();
                        return;
                    }
                }
            }

            fb_process_row();
        }
    }

    // TODO: Update non-tick effects
}

fb_play_status fb_get_play_status(void)
{
    return player.play_status;
}

const fb_music *fb_get_playing_music(void)
{
    if (!engine.initialized || player.play_status == FB_PLAY_STATUS_STOP)
        return NULL;

    return player.music;
}

int fb_get_order(void)
{
    if (!engine.initialized || player.play_status == FB_PLAY_STATUS_STOP)
        return -1;

    return player.pos.order;
}

int fb_get_row(void)
{
    if (!engine.initialized || player.play_status == FB_PLAY_STATUS_STOP)
        return -1;

    return player.pos.row;
}

/**
 * @brief Fetch, decode and execute a row in a DMG channel
 *
 * @param ch channel number [1..4]
 * @param pattern current pattern for that channel
 */
static void fb_process_dmg_row(const int ch, const fb_pattern *const pattern)
{
    fb_dmg_channel *const channel = &player.dmg_channels[ch - 1];

    const int row_skip = player.pos.row * ((pattern->has_volume ? 2 : 0) + (pattern->has_note ? 1 : 0) +
                                           (pattern->has_instrument ? 1 : 0) +
                                           (pattern->max_effects_count ? 2 * pattern->max_effects_count : 0));
    const uint8_t *data = pattern->data + row_skip;

    uint16_t vol = 0xFFFF;
    uint8_t note = 0xFF, inst = 0xFF;
    struct
    {
        uint8_t kind, val;
    } effects[8] = {{0xAA, 0xAA}, {0xAA, 0xAA}, {0xAA, 0xAA}, {0xAA, 0xAA},
                    {0xAA, 0xAA}, {0xAA, 0xAA}, {0xAA, 0xAA}, {0xAA, 0xAA}};

    // fetch row
    if (pattern->has_volume)
    {
        vol = *((const uint16_t *)data);
        data += 2;
    }
    if (pattern->has_note)
    {
        note = *data++;
    }
    if (pattern->has_instrument)
    {
        inst = *data++;
    }
    for (int i = 0; i < pattern->max_effects_count; ++i)
    {
        effects[i].kind = *data++;
        effects[i].val = *data++;
    }

    // decode row
    if (inst != 0xFF)
    {
        const fb_instrument *prev_inst = channel->inst;

        channel->inst = &player.music->instruments[inst];
        const fb_inst_gb *const gb = ((channel->inst->gb != NULL) ? channel->inst->gb : &default_inst_gb);

        if (prev_inst != channel->inst)
        {
            if (ch != 3)
                channel->vol = gb->initial_volume;
            channel->env_len = gb->envelop_length;
            channel->snd_len = gb->sound_length;
            channel->dir_up = gb->envelop_direction_up;

            channel->envelop_initialized = true;
        }
    }

    if (vol != 0xFFFF)
    {
        channel->vol = vol;
        channel->envelop_initialized = true;
    }

    if (note != FB_NOTE_EMPTY)
    {
        if (note == FB_NOTE_OFF)
        {
            if (channel->note_on)
            {
                channel->note_on = false;
                channel->envelop_initialized = true;
                channel->retrigger = true;
            }
        }
        else if (note == FB_NOTE_NOTE_REL)
        {
            // TODO
        }
        else if (note == FB_NOTE_MACRO_REL)
        {
            // TODO
        }
        else if (note <= FB_NOTE_B_9)
        {
            if (ch == 4)
                channel->freq_base = dmg_noise_table[fb_clamp_s32(note, FB_NOTE_C_0, FB_NOTE_Gs5) - FB_NOTE_C_0];
            else
                channel->freq_base = dmg_period_table[fb_clamp_s32(note, FB_NOTE_B_1, FB_NOTE_B_9) - FB_NOTE_B_1];

            if (!channel->note_on)
            {
                channel->note_on = true;
                channel->envelop_initialized = true;
            }
            channel->retrigger = true;
        }
    }

    for (int i = 0; i < pattern->max_effects_count; ++i)
    {
        const uint8_t fx = effects[i].kind;
        const uint8_t val = effects[i].val;

        // TODO: Add more effects
        switch (fx)
        {
        case FB_EFFECT_JUMP_TO_PATTERN:
            if (player.jump_pos.order != FB_JUMP_POS_STOP_SONG)
            {
                player.jump_pos.order = val;

                if (player.jump_pos.row == FB_JUMP_POS_EMPTY)
                    player.jump_pos.row = 0;
            }
            break;
        case FB_EFFECT_JUMP_TO_NEXT_PATTERN:
            if (player.jump_pos.order != FB_JUMP_POS_STOP_SONG)
            {
                if (player.jump_pos.order == FB_JUMP_POS_EMPTY)
                    player.jump_pos.order = (player.pos.order + 1) % player.music->order_length;

                player.jump_pos.row = val;
            }
            break;
        case FB_EFFECT_SET_WAVEFORM: {
            const int wt_idx = (val < player.music->wavetables_count) ? val : 0;
            for (int i = 0; i < 4; ++i)
                FB_REG_WAVE_RAM[i] = player.music->wavetables[wt_idx].data[i];
            FB_REG_SND3SEL ^= FB_SND3SEL_BANK_SET(1);
            break;
        }
        case FB_EFFECT_SET_NOISE_LENGTH:
            const bool short_noise = (val != 0);
            FB_REG_SND4FREQ = (FB_REG_SND4FREQ & ~FB_SND4FREQ_WIDTH_7_BITS) |
                              (short_noise ? FB_SND4FREQ_WIDTH_7_BITS : FB_SND4FREQ_WIDTH_15_BITS);
            break;
        case FB_EFFECT_SET_DUTY_CYCLE:
            if (ch == 1 || ch == 2)
            {
                channel->duty = val % 4;
                channel->envelop_initialized = true;
            }
            break;
        case FB_EFFECT_STOP_SONG:
            player.jump_pos.order = FB_JUMP_POS_STOP_SONG;
            break;
        default:
            // ignore unknown effect
            break;
        }
    }

    // execute row
    const bool snd_len_enabled = (channel->snd_len != FB_GB_SOUND_LENGTH_INFINITY);

    // avoid audio pop (keep DAC on)
    const uint16_t sndcnt_env_dir =
        ((channel->dir_up || channel->vol == 0) ? FB_SND1CNT_ENV_DIR_INC : FB_SND1CNT_ENV_DIR_DEC);

    if (ch == 1 || ch == 2)
    {
        volatile uint16_t *const reg_sndfreq = ((ch == 1) ? &FB_REG_SND1FREQ : &FB_REG_SND2FREQ);
        volatile uint16_t *const reg_sndcnt = ((ch == 1) ? &FB_REG_SND1CNT : &FB_REG_SND2CNT);

        if (channel->envelop_initialized)
            *reg_sndcnt =
                (snd_len_enabled ? FB_SND1CNT_LENGTH_SET(FB_GB_SOUND_LENGTH_INFINITY - 1 - channel->snd_len) : 0) |
                ((channel->duty & 3) << 6) | FB_SND1CNT_ENV_STEP_TIME_SET(channel->env_len) | sndcnt_env_dir |
                FB_SND1CNT_ENV_VOLUME_SET((channel->note_on) ? channel->vol : 0);
        if (channel->retrigger || channel->snd_len_enabled != snd_len_enabled)
            *reg_sndfreq = ((channel->retrigger) ? FB_SND1FREQ_RETRIG : 0) |
                           ((snd_len_enabled) ? FB_SND1FREQ_LENGTH_ENABLE : 0) |
                           FB_SND1FREQ_PERIOD_SET(channel->freq_base + channel->freq_diff);
    }
    else if (ch == 3)
    {
        const int vol = ((!channel->note_on)    ? FB_SND3CNT_VOLUME_0
                         : (channel->vol >= 12) ? FB_SND3CNT_VOLUME_100
                         : (channel->vol >= 8)  ? FB_SND3CNT_VOLUME_50
                         : (channel->vol >= 4)  ? FB_SND3CNT_VOLUME_25
                                                : FB_SND3CNT_VOLUME_0);

        if (channel->envelop_initialized)
            FB_REG_SND3CNT = vol | ((snd_len_enabled) ? FB_SND3CNT_LENGTH_SET(FB_CH3_SND_LEN) : 0);
        if (channel->retrigger || channel->snd_len_enabled != snd_len_enabled)
            FB_REG_SND3FREQ = ((channel->retrigger) ? FB_SND3FREQ_RETRIG : 0) |
                              ((snd_len_enabled) ? FB_SND3FREQ_LENGTH_ENABLE : 0) |
                              FB_SND3FREQ_PERIOD_SET(channel->freq_base + channel->freq_diff);
    }
    else if (ch == 4)
    {
        if (channel->envelop_initialized)
            FB_REG_SND4CNT =
                (snd_len_enabled ? FB_SND4CNT_LENGTH_SET(FB_GB_SOUND_LENGTH_INFINITY - 1 - channel->snd_len) : 0) |
                ((channel->duty & 3) << 6) | FB_SND4CNT_ENV_STEP_TIME_SET(channel->env_len) | sndcnt_env_dir |
                FB_SND4CNT_ENV_VOLUME_SET((channel->note_on) ? channel->vol : 0);
        if (channel->retrigger || channel->snd_len_enabled != snd_len_enabled)
            FB_REG_SND4FREQ =
                ((channel->retrigger) ? FB_SND4FREQ_RETRIG : 0) | ((snd_len_enabled) ? FB_SND4FREQ_LENGTH_ENABLE : 0) |
                FB_SND4FREQ_PRE_STEP_RATIO_SET(channel->freq_base >> 4) | FB_SND4FREQ_DIV_RATIO_SET(channel->freq_base);
    }

    channel->snd_len_enabled = snd_len_enabled;
    channel->retrigger = false;
    channel->envelop_initialized = false;
}

static void fb_process_row(void)
{
    // DMG channels
    if (engine.settings.channels & FB_INIT_CHANNELS_DMG_CH1)
    {
        const fb_pattern *pattern = player.music->ch1_order[player.pos.order];
        if (pattern != NULL)
            fb_process_dmg_row(1, pattern);
    }
    if (engine.settings.channels & FB_INIT_CHANNELS_DMG_CH2)
    {
        const fb_pattern *pattern = player.music->ch2_order[player.pos.order];
        if (pattern != NULL)
            fb_process_dmg_row(2, pattern);
    }
    if (engine.settings.channels & FB_INIT_CHANNELS_DMG_CH3)
    {
        const fb_pattern *pattern = player.music->ch3_order[player.pos.order];
        if (pattern != NULL)
            fb_process_dmg_row(3, pattern);
    }
    if (engine.settings.channels & FB_INIT_CHANNELS_DMG_CH4)
    {
        const fb_pattern *pattern = player.music->ch4_order[player.pos.order];
        if (pattern != NULL)
            fb_process_dmg_row(4, pattern);
    }

    // TODO: DirectSound channels
}
