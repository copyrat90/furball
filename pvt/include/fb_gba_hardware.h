// I/O registers naming scheme : Tonc's
// their values  naming scheme : UGBA's

#ifndef FB_GBA_HARDWARE_H
#define FB_GBA_HARDWARE_H

#include <stdint.h>

#define FB_ARM_CODE   __attribute__((target("arm")))
#define FB_THUMB_CODE __attribute__((target("thumb")))
#define FB_IWRAM_BSS  // IWRAM is the default location for .bss symbols
#define FB_IWRAM_DATA // IWRAM is the default location for .data symbols
#define FB_IWRAM_CODE __attribute__((section(".iwram_code"), long_call))
#define FB_EWRAM_BSS  __attribute__((section(".sbss")))
#define FB_EWRAM_DATA __attribute__((section(".ewram_data")))
#define FB_EWRAM_CODE __attribute__((section(".ewram_code"), long_call))


#define FB_MEM_IO  (0x04000000) // I/O register address

// Registers
// ---------------

// Sound registers
#define FB_REG_SND1SWEEP   *((volatile uint16_t*)(FB_MEM_IO + 0x60)) // Sound 1 Sweep control
#define FB_REG_SND1CNT     *((volatile uint16_t*)(FB_MEM_IO + 0x62)) // Sound 1 Length, wave duty and envelope control
#define FB_REG_SND1FREQ    *((volatile uint16_t*)(FB_MEM_IO + 0x64)) // Sound 1 Frequency, reset and loop control
#define FB_REG_SND2CNT     *((volatile uint16_t*)(FB_MEM_IO + 0x68)) // Sound 2 Lenght, wave duty and envelope control
#define FB_REG_SND2FREQ    *((volatile uint16_t*)(FB_MEM_IO + 0x6C)) // Sound 2 Frequency, reset and loop control
#define FB_REG_SND3SEL     *((volatile uint16_t*)(FB_MEM_IO + 0x70)) // Sound 3 Enable and wave ram bank control
#define FB_REG_SND3CNT     *((volatile uint16_t*)(FB_MEM_IO + 0x72)) // Sound 3 Sound lenght and output level control
#define FB_REG_SND3FREQ    *((volatile uint16_t*)(FB_MEM_IO + 0x74)) // Sound 3 Frequency, reset and loop control
#define FB_REG_SND4CNT     *((volatile uint16_t*)(FB_MEM_IO + 0x78)) // Sound 4 Length, output level and envelope control
#define FB_REG_SND4FREQ    *((volatile uint16_t*)(FB_MEM_IO + 0x7C)) // Sound 4 Noise parameters, reset and loop control
#define FB_REG_SNDDMGCNT    *((volatile uint16_t*)(FB_MEM_IO + 0x80)) // Sound 1-4 Output level and Stereo control
#define FB_REG_SNDDSCNT    *((volatile uint16_t*)(FB_MEM_IO + 0x82)) // Direct Sound control and Sound 1-4 output ratio
#define FB_REG_SNDSTAT     *((volatile uint16_t*)(FB_MEM_IO + 0x84)) // Master sound enable and Sound 1-4 play status
#define FB_REG_SNDBIAS     *((volatile uint16_t*)(FB_MEM_IO + 0x88)) // Sound bias and Amplitude resolution control

// Sound buffers
#define FB_REG_WAVE_RAM       ((volatile uint32_t*)(FB_MEM_IO + 0x90)) // Sound 3 samples pointer
#define FB_REG_WAVE_RAM_0    *((volatile uint32_t*)(FB_MEM_IO + 0x90)) // Sound 3 samples 0-7
#define FB_REG_WAVE_RAM_0_L  *((volatile uint16_t*)(FB_MEM_IO + 0x90)) // Sound 3 samples 0-3
#define FB_REG_WAVE_RAM_0_H  *((volatile uint16_t*)(FB_MEM_IO + 0x90)) // Sound 3 samples 4-7
#define FB_REG_WAVE_RAM_1    *((volatile uint32_t*)(FB_MEM_IO + 0x94)) // Sound 3 samples 8-15
#define FB_REG_WAVE_RAM_1_L  *((volatile uint16_t*)(FB_MEM_IO + 0x94)) // Sound 3 samples 8-11
#define FB_REG_WAVE_RAM_1_H  *((volatile uint16_t*)(FB_MEM_IO + 0x94)) // Sound 3 samples 12-15
#define FB_REG_WAVE_RAM_2    *((volatile uint32_t*)(FB_MEM_IO + 0x98)) // Sound 3 samples 16-23
#define FB_REG_WAVE_RAM_2_L  *((volatile uint16_t*)(FB_MEM_IO + 0x98)) // Sound 3 samples 16-19
#define FB_REG_WAVE_RAM_2_H  *((volatile uint16_t*)(FB_MEM_IO + 0x98)) // Sound 3 samples 20-23
#define FB_REG_WAVE_RAM_3    *((volatile uint32_t*)(FB_MEM_IO + 0x9C)) // Sound 3 samples 24-31
#define FB_REG_WAVE_RAM_3_L  *((volatile uint16_t*)(FB_MEM_IO + 0x9C)) // Sound 3 samples 24-27
#define FB_REG_WAVE_RAM_3_H  *((volatile uint16_t*)(FB_MEM_IO + 0x9C)) // Sound 3 samples 28-31
#define FB_REG_FIFO_A        *((volatile uint32_t*)(FB_MEM_IO + 0xA0)) // Direct Sound channel A samples
#define FB_REG_FIFO_B        *((volatile uint32_t*)(FB_MEM_IO + 0xA4)) // Direct Sound channel B samples

// DMA registers
#define FB_REG_DMA0SAD     *((volatile uint32_t*)(FB_MEM_IO + 0xB0)) // DMA 0 Source address
#define FB_REG_DMA0DAD     *((volatile uint32_t*)(FB_MEM_IO + 0xB4)) // DMA 0 Destination address
#define FB_REG_DMA0CNT     *((volatile uint32_t*)(FB_MEM_IO + 0xB8)) // DMA 0 Control
#define FB_REG_DMA0CNT_L   *((volatile uint16_t*)(FB_MEM_IO + 0xB8)) // DMA 0 Control (Low)
#define FB_REG_DMA0CNT_H   *((volatile uint16_t*)(FB_MEM_IO + 0xBA)) // DMA 0 Control (High)
#define FB_REG_DMA1SAD     *((volatile uint32_t*)(FB_MEM_IO + 0xBC)) // DMA 1 Source address
#define FB_REG_DMA1DAD     *((volatile uint32_t*)(FB_MEM_IO + 0xC0)) // DMA 1 Destination address
#define FB_REG_DMA1CNT     *((volatile uint32_t*)(FB_MEM_IO + 0xC4)) // DMA 1 Control
#define FB_REG_DMA1CNT_L   *((volatile uint16_t*)(FB_MEM_IO + 0xC4)) // DMA 1 Control (Low)
#define FB_REG_DMA1CNT_H   *((volatile uint16_t*)(FB_MEM_IO + 0xC6)) // DMA 1 Control (High)
#define FB_REG_DMA2SAD     *((volatile uint32_t*)(FB_MEM_IO + 0xC8)) // DMA 2 Source address
#define FB_REG_DMA2DAD     *((volatile uint32_t*)(FB_MEM_IO + 0xCC)) // DMA 2 Destination address
#define FB_REG_DMA2CNT     *((volatile uint32_t*)(FB_MEM_IO + 0xD0)) // DMA 2 Control
#define FB_REG_DMA2CNT_L   *((volatile uint16_t*)(FB_MEM_IO + 0xD0)) // DMA 2 Control (Low)
#define FB_REG_DMA2CNT_H   *((volatile uint16_t*)(FB_MEM_IO + 0xD2)) // DMA 2 Control (High)
#define FB_REG_DMA3SAD     *((volatile uint32_t*)(FB_MEM_IO + 0xD4)) // DMA 3 Source address
#define FB_REG_DMA3DAD     *((volatile uint32_t*)(FB_MEM_IO + 0xD8)) // DMA 3 Destination address
#define FB_REG_DMA3CNT     *((volatile uint32_t*)(FB_MEM_IO + 0xDC)) // DMA 3 Control
#define FB_REG_DMA3CNT_L   *((volatile uint16_t*)(FB_MEM_IO + 0xDC)) // DMA 3 Control (Low)
#define FB_REG_DMA3CNT_H   *((volatile uint16_t*)(FB_MEM_IO + 0xDE)) // DMA 3 Control (High)

// Timer registers
#define FB_REG_TM0D        *((volatile uint16_t*)(FB_MEM_IO + 0x100)) // Timer 0 data
#define FB_REG_TM0CNT      *((volatile uint16_t*)(FB_MEM_IO + 0x102)) // Timer 0 control
#define FB_REG_TM1D        *((volatile uint16_t*)(FB_MEM_IO + 0x104)) // Timer 1 data
#define FB_REG_TM1CNT      *((volatile uint16_t*)(FB_MEM_IO + 0x106)) // Timer 1 control
#define FB_REG_TM2D        *((volatile uint16_t*)(FB_MEM_IO + 0x108)) // Timer 2 data
#define FB_REG_TM2CNT      *((volatile uint16_t*)(FB_MEM_IO + 0x10A)) // Timer 2 control
#define FB_REG_TM3D        *((volatile uint16_t*)(FB_MEM_IO + 0x10C)) // Timer 3 data
#define FB_REG_TM3CNT      *((volatile uint16_t*)(FB_MEM_IO + 0x10E)) // Timer 3 control


// Register values
// ---------------

// SND1SWEEP (NR10) (SOUND1CNT_L)

#define FB_SND1SWEEP_SWEEP_SHIFT_SET(n)  ((n) & 0x7) // R/W
#define FB_SND1SWEEP_SWEEP_SHIFT_GET(n)  ((n) & 0x7)

#define FB_SND1SWEEP_SWEEP_DIR_INC       (0 << 3) // R/W
#define FB_SND1SWEEP_SWEEP_DIR_DEC       (1 << 3)

#define FB_SND1SWEEP_SWEEP_TIME_SET(n)   (((n) & 0x7) << 4) // R/W
#define FB_SND1SWEEP_SWEEP_TIME_GET(n)   (((n) >> 4) & 0x7)

// SND1CNT (NR11, NR12) (SOUND1CNT_H)

#define FB_SND1CNT_LENGTH_SET(n)         ((n) & 0x3F) // WO
#define FB_SND1CNT_LENGTH_GET(n)         ((n) & 0x3F) // Internal use

#define FB_SND1CNT_WAVE_DUTY_GET(n)      (((n) >> 6) & 0x3) // R/W
#define FB_SND1CNT_WAVE_DUTY_12          (0 << 6)
#define FB_SND1CNT_WAVE_DUTY_25          (1 << 6)
#define FB_SND1CNT_WAVE_DUTY_50          (2 << 6)
#define FB_SND1CNT_WAVE_DUTY_75          (3 << 6)

#define FB_SND1CNT_ENV_STEP_TIME_SET(n)  (((n) & 0x7) << 8) // R/W
#define FB_SND1CNT_ENV_STEP_TIME_GET(n)  (((n) >> 8) & 0x7)

#define FB_SND1CNT_ENV_DIR_DEC           (0 << 11) // R/W
#define FB_SND1CNT_ENV_DIR_INC           (1 << 11)

#define FB_SND1CNT_ENV_VOLUME_SET(n)     (((n) & 0xF) << 12) // R/W
#define FB_SND1CNT_ENV_VOLUME_GET(n)     (((n) >> 12) & 0xF)

// SND1FREQ (NR13, NR14) (SOUND1CNT_X)

#define FB_SND1FREQ_FREQUENCY_SET(n)    ((n) & 0x7FF) // WO
#define FB_SND1FREQ_FREQUENCY_GET(n)    ((n) & 0x7FF) // Internal use

#define FB_SND1FREQ_LENGTH_ENABLE       (1 << 14) // R/W

#define FB_SND1FREQ_RESTART             (1 << 15) // WO

// SND2CNT (NR21, NR22) (SOUND2CNT_L)

#define FB_SND2CNT_LENGTH_SET(n)        ((n) & 0x3F) // WO
#define FB_SND2CNT_LENGTH_GET(n)        ((n) & 0x3F) // Internal use

#define FB_SND2CNT_WAVE_DUTY_GET(n)     (((n) >> 6) & 0x3) // R/W
#define FB_SND2CNT_WAVE_DUTY_12         (0 << 6)
#define FB_SND2CNT_WAVE_DUTY_25         (1 << 6)
#define FB_SND2CNT_WAVE_DUTY_50         (2 << 6)
#define FB_SND2CNT_WAVE_DUTY_75         (3 << 6)

#define FB_SND2CNT_ENV_STEP_TIME_SET(n) (((n) & 0x7) << 8) // R/W
#define FB_SND2CNT_ENV_STEP_TIME_GET(n) (((n) >> 8) & 0x7)

#define FB_SND2CNT_ENV_DIR_DEC          (0 << 11) // R/W
#define FB_SND2CNT_ENV_DIR_INC          (1 << 11)

#define FB_SND2CNT_ENV_VOLUME_SET(n)    (((n) & 0xF) << 12) // R/W
#define FB_SND2CNT_ENV_VOLUME_GET(n)    (((n) >> 12) & 0xF)

// SND2FREQ (NR23, NR24) (SOUND2CNT_H)

#define FB_SND2FREQ_FREQUENCY_SET(n)    ((n) & 0x7FF) // WO
#define FB_SND2FREQ_FREQUENCY_GET(n)    ((n) & 0x7FF) // Internal use

#define FB_SND2FREQ_LENGTH_ENABLE       (1 << 14) // R/W

#define FB_SND2FREQ_RESTART             (1 << 15) // WO

// SND3SEL (NR30) (SOUND3CNT_L)

#define FB_SND3SEL_SIZE_32             (0 << 5)
#define FB_SND3SEL_SIZE_64             (1 << 5)

#define FB_SND3SEL_BANK_SET(n)         (((n) & 1) << 6)
#define FB_SND3SEL_BANK_GET(n)         (((n) >> 6) & 1)

#define FB_SND3SEL_ENABLE              (1 << 7)
#define FB_SND3SEL_DISABLE             (0 << 7)

// SND3CNT (NR31, NR32) (SOUND3CNT_H)

#define FB_SND3CNT_LENGTH_SET(n)       ((n) & 0xFF) // R/W
#define FB_SND3CNT_LENGTH_GET(n)       ((n) & 0xFF)

#define FB_SND3CNT_VOLUME_0            (0x0 << 13) // R/W
#define FB_SND3CNT_VOLUME_25           (0x3 << 13)
#define FB_SND3CNT_VOLUME_50           (0x2 << 13)
#define FB_SND3CNT_VOLUME_75           (0x4 << 13)
#define FB_SND3CNT_VOLUME_100          (0x1 << 13)
#define FB_SND3CNT_VOLUME_MASK         (0x7 << 13)

// SND3FREQ (NR33, NR34) (SOUND3CNT_X)

#define FB_SND3FREQ_SAMPLE_RATE_SET(n)  ((n) & 0x7FF) // WO
#define FB_SND3FREQ_SAMPLE_RATE_GET(n)  ((n) & 0x7FF) // Internal use

#define FB_SND3FREQ_LENGTH_ENABLE       (1 << 14) // R/W

#define FB_SND3FREQ_RESTART             (1 << 15) // WO

// SND4CNT (NR41, NR42) (SOUND4CNT_L)

#define FB_SND4CNT_LENGTH_SET(n)       ((n) & 0x3F) // WO
#define FB_SND4CNT_LENGTH_GET(n)       ((n) & 0x3F) // Internal use

#define FB_SND4CNT_ENV_STEP_TIME_SET(n) (((n) & 0x7) << 8) // R/W
#define FB_SND4CNT_ENV_STEP_TIME_GET(n) (((n) >> 8) & 0x7)

#define FB_SND4CNT_ENV_DIR_DEC         (0 << 11) // R/W
#define FB_SND4CNT_ENV_DIR_INC         (1 << 11)

#define FB_SND4CNT_ENV_VOLUME_SET(n)   (((n) & 0xF) << 12) // R/W
#define FB_SND4CNT_ENV_VOLUME_GET(n)   (((n) >> 12) & 0xF)

// SND4FREQ (NR43, NR44) (SOUND4CNT_H)

#define FB_SND4FREQ_DIV_RATIO_SET(n)    ((n) & 0x7) // R/W
#define FB_SND4FREQ_DIV_RATIO_GET(n)    ((n) & 0x7)

#define FB_SND4FREQ_WIDTH_15_BITS       (0 << 3) // R/W
#define FB_SND4FREQ_WIDTH_7_BITS        (1 << 3)

#define FB_SND4FREQ_FREQUENCY_SET(n)    (((n) & 0xF) << 4) // R/W
#define FB_SND4FREQ_FREQUENCY_GET(n)    (((n) >> 4) & 0xF)

#define FB_SND4FREQ_LENGTH_ENABLE       (1 << 14) // R/W

#define FB_SND4FREQ_RESTART             (1 << 15) // WO

// SNDDMGCNT (SOUNDCNT_L)

#define FB_SNDDMGCNT_PSG_VOL_RIGHT_SET(v) (((v) & 0x7) << 0)
#define FB_SNDDMGCNT_PSG_VOL_RIGHT_GET(v) (((v) >> 0) & 0x7)
#define FB_SNDDMGCNT_PSG_VOL_LEFT_SET(v)  (((v) & 0x7) << 4)
#define FB_SNDDMGCNT_PSG_VOL_LEFT_GET(v)  (((v) >> 4) & 0x7)

#define FB_SNDDMGCNT_PSG_1_ENABLE_RIGHT   (1 << 8)
#define FB_SNDDMGCNT_PSG_2_ENABLE_RIGHT   (1 << 9)
#define FB_SNDDMGCNT_PSG_3_ENABLE_RIGHT   (1 << 10)
#define FB_SNDDMGCNT_PSG_4_ENABLE_RIGHT   (1 << 11)

#define FB_SNDDMGCNT_PSG_1_ENABLE_LEFT    (1 << 12)
#define FB_SNDDMGCNT_PSG_2_ENABLE_LEFT    (1 << 13)
#define FB_SNDDMGCNT_PSG_3_ENABLE_LEFT    (1 << 14)
#define FB_SNDDMGCNT_PSG_4_ENABLE_LEFT    (1 << 15)

// SNDDSCNT (SOUNDCNT_H)

#define FB_SNDDSCNT_PSG_VOLUME_25        (0 << 0)
#define FB_SNDDSCNT_PSG_VOLUME_50        (1 << 0)
#define FB_SNDDSCNT_PSG_VOLUME_100       (2 << 0)
#define FB_SNDDSCNT_PSG_VOLUME_MASK      (3 << 0)

#define FB_SNDDSCNT_DMA_A_VOLUME_50      (0 << 2)
#define FB_SNDDSCNT_DMA_A_VOLUME_100     (1 << 2)

#define FB_SNDDSCNT_DMA_B_VOLUME_50      (0 << 3)
#define FB_SNDDSCNT_DMA_B_VOLUME_100     (1 << 3)

#define FB_SNDDSCNT_DMA_A_ENABLE_RIGHT   (1 << 8)
#define FB_SNDDSCNT_DMA_A_ENABLE_LEFT    (1 << 9)

#define FB_SNDDSCNT_DMA_A_TIMER0         (0 << 10)
#define FB_SNDDSCNT_DMA_A_TIMER1         (1 << 10)

#define FB_SNDDSCNT_DMA_A_RESET          (1 << 11)

#define FB_SNDDSCNT_DMA_B_ENABLE_RIGHT   (1 << 12)
#define FB_SNDDSCNT_DMA_B_ENABLE_LEFT    (1 << 13)

#define FB_SNDDSCNT_DMA_B_TIMER0         (0 << 14)
#define FB_SNDDSCNT_DMA_B_TIMER1         (1 << 14)

#define FB_SNDDSCNT_DMA_B_RESET          (1 << 15)

// SNDSTAT (SOUNDCNT_X)

#define FB_SNDSTAT_PSG_1_IS_ON           (1 << 0) // Read-only flags
#define FB_SNDSTAT_PSG_2_IS_ON           (1 << 1)
#define FB_SNDSTAT_PSG_3_IS_ON           (1 << 2)
#define FB_SNDSTAT_PSG_4_IS_ON           (1 << 3)

#define FB_SNDSTAT_MASTER_ENABLE         (1 << 7)
#define FB_SNDSTAT_MASTER_DISABLE        (0 << 7)

// SNDBIAS (SOUNDBIAS)

#define FB_SNDBIAS_BIAS_LEVEL_SET(v)     (((v) & 0x1FF) << 1)
#define FB_SNDBIAS_BIAS_LEVEL_GET(v)     (((v) >> 1) & 0x1FF)
#define FB_SNDBIAS_BIAS_MASK             (0x1FF << 1)

#define FB_SNDBIAS_SAMPLE_RATE_32KHZ     (0 << 14) // Best for DMA channels A,B
#define FB_SNDBIAS_SAMPLE_RATE_65KHZ     (1 << 14)
#define FB_SNDBIAS_SAMPLE_RATE_131KHZ    (2 << 14)
#define FB_SNDBIAS_SAMPLE_RATE_262KHZ    (3 << 14) // Best for PSG channels 1-4

// DMA0CNT_H, DMA1CNT_H, DMA2CNT_H, DMA3CNT_H

#define FB_DMAxCNT_H_DST_INCREMENT       (0 << 5)
#define FB_DMAxCNT_H_DST_DECREMENT       (1 << 5)
#define FB_DMAxCNT_H_DST_FIXED           (2 << 5)
#define FB_DMAxCNT_H_DST_RELOAD          (3 << 5)

#define FB_DMAxCNT_H_SRC_INCREMENT       (0 << 7)
#define FB_DMAxCNT_H_SRC_DECREMENT       (1 << 7)
#define FB_DMAxCNT_H_SRC_FIXED           (2 << 7)

#define FB_DMAxCNT_H_REPEAT_ON           (1 << 9)

#define FB_DMAxCNT_H_TRANSFER_16_BITS    (0 << 10)
#define FB_DMAxCNT_H_TRANSFER_32_BITS    (1 << 10)

#define FB_DMAxCNT_H_GAME_PAK_NORMAL     (0 << 11) // DMA3 only
#define FB_DMAxCNT_H_GAME_PAK_DRQ        (1 << 11) // DMA3 only

#define FB_DMAxCNT_H_START_NOW           (0 << 12)
#define FB_DMAxCNT_H_START_VBLANK        (1 << 12)
#define FB_DMAxCNT_H_START_HBLANK        (2 << 12)
#define FB_DMAxCNT_H_START_SPECIAL       (3 << 12) // DMA1, DMA2, DMA3

#define FB_DMAxCNT_H_IRQ_ENABLE          (1 << 14)

#define FB_DMAxCNT_H_DMA_ENABLE          (1 << 15)

// TM0CNT, TM1CNT, TM2CNT, TM3CNT

#define FB_TMxCNT_PRESCALER_F_DIV_1      (0 << 0)
#define FB_TMxCNT_PRESCALER_F_DIV_64     (1 << 0)
#define FB_TMxCNT_PRESCALER_F_DIV_256    (2 << 0)
#define FB_TMxCNT_PRESCALER_F_DIV_1024   (3 << 0)

#define FB_TMxCNT_STANDALONE             (0 << 2)
#define FB_TMxCNT_CASCADE                (1 << 2) // Not used in TM0CNT

#define FB_TMxCNT_IRQ_DISABLE            (0 << 6)
#define FB_TMxCNT_IRQ_ENABLE             (1 << 6)

#define FB_TMxCNT_STOP                   (0 << 7)
#define FB_TMxCNT_START                  (1 << 7)


#endif // FB_GBA_HARDWARE_H
