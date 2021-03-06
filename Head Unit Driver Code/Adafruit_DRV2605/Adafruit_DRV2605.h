/*!
 * @file    Adafruit_DRV2605.h
 * @brief   Driver code for the Adafruit DRV2605L Haptic Motor Controller breakout board
 *          as described by its datasheet (https://cdn-shop.adafruit.com/datasheets/DRV2605.pdf)
 * @ref     Initialization sequence and large part of defines taken from Adafruits github
 *          repository for this device (https://github.com/adafruit/Adafruit_DRV2605_Library)
 * @note    This code is written with the intention of using I2C to communicate to the
 *          STM32 L4R5ZI-P board and uses the following pinout.
 * 
 *          PINOUT  LABEL           PORT/PIN
 *          --------------------------------
 *          SCL     I2C1_SCL        PB8
 *          SDA     I2C1_SDA        PB9
 *          SCL     I2C2_SCL        PB10
 *          SDA     I2C2_SDA        PB11
 *          SCL     I2C3_SCL        PA7
 *          SDA     I2C3_SDA        PB4
 *          SCL     I2C4_SCL        PF14
 *          SDA     I2C4_SDA        PF15
 *
 * @author  Miles Hanbury (mhanbury)
 * @author  James Kelly (jkellymi)
 * @author  Joshua Nye (nyej)
 */

#include "stm32l4xx_hal.h"

/* -------------------------------- Adafruit DRV2605 Command Set ------------------------------- */
#define DRV2605_ADDR                0x5A                                                            // device address
#define DRV2605_ADDR_W              0xB4                                                            // device address for writing
#define DRV2605_ADDR_R              0xB5                                                            // device address for reading

#define DRV2605_REG_STATUS          0x00                                                            // status register
#define DRV2605_REG_MODE            0x01                                                            // mode register
#define DRV2605_MODE_INTTRIG        0x00                                                            // internal trigger mode
#define DRV2605_MODE_EXTTRIGEDGE    0x01                                                            // external edge trigger mode
#define DRV2605_MODE_EXTTRIGLVL     0x02                                                            // external level trigger mode
#define DRV2605_MODE_PWMANALOG      0x03                                                            // PWM/Analog input mode
#define DRV2605_MODE_AUDIOVIBE      0x04                                                            // audio-to-vibe mode
#define DRV2605_MODE_REALTIME       0x05                                                            // real-time playback (RTP) mode
#define DRV2605_MODE_DIAGNOS        0x06                                                            // diagnostics mode
#define DRV2605_MODE_AUTOCAL        0x07                                                            // auto calibration mode

#define DRV2605_REG_RTPIN           0x02                                                            // real-time playback input register
#define DRV2605_REG_LIBRARY         0x03                                                            // waveform library selection register
#define DRV2605_REG_WAVESEQ1        0x04                                                            // waveform sequence register 1
#define DRV2605_REG_WAVESEQ2        0x05                                                            // waveform sequence register 2
#define DRV2605_REG_WAVESEQ3        0x06                                                            // waveform sequence register 3
#define DRV2605_REG_WAVESEQ4        0x07                                                            // waveform sequence register 4
#define DRV2605_REG_WAVESEQ5        0x08                                                            // waveform sequence register 5
#define DRV2605_REG_WAVESEQ6        0x09                                                            // waveform sequence register 6
#define DRV2605_REG_WAVESEQ7        0x0A                                                            // waveform sequence register 7
#define DRV2605_REG_WAVESEQ8        0x0B                                                            // waveform sequence register 8

#define DRV2605_REG_GO              0x0C                                                            // Go register
#define DRV2605_REG_OVERDRIVE       0x0D                                                            // Overdrive time offset register
#define DRV2605_REG_SUSTAINPOS      0x0E                                                            // Sustain time offset, positive register
#define DRV2605_REG_SUSTAINNEG      0x0F                                                            // Sustain time offset, negative register
#define DRV2605_REG_BREAK           0x10                                                            // Brake time offset register
#define DRV2605_REG_AUDIOCTRL       0x11                                                            // Audio-to-vibe control register
#define DRV2605_REG_AUDIOLVL        0x12                                                            // Audio-to-vibe minimum input level register
#define DRV2605_REG_AUDIOMAX        0x13                                                            // Audio-to-vibe maximum input level register
#define DRV2605_REG_AUDIOOUTMIN     0x14                                                            // Audio-to-vibe minimum output drive register
#define DRV2605_REG_AUDIOOUTMAX     0x15                                                            // Audio-to-vibe maximum output drive register
#define DRV2605_REG_RATEDV          0x16                                                            // Rated voltage register
#define DRV2605_REG_CLAMPV          0x17                                                            // Overdrive clamp voltage register
#define DRV2605_REG_AUTOCALCOMP     0x18                                                            // Auto-calibration compensation result register
#define DRV2605_REG_AUTOCALEMP      0x19                                                            // Auto-calibration back-EMF result register
#define DRV2605_REG_FEEDBACK        0x1A                                                            // Feedback control register
#define DRV2605_REG_CONTROL1        0x1B                                                            // Control1 Register
#define DRV2605_REG_CONTROL2        0x1C                                                            // Control2 Register
#define DRV2605_REG_CONTROL3        0x1D                                                            // Control3 Register
#define DRV2605_REG_CONTROL4        0x1E                                                            // Control4 Register
#define DRV2605_REG_VBAT            0x21                                                            // Vbat voltage-monitor register
#define DRV2605_REG_LRARESON        0x22                                                            // LRA resonance-period register

/* ------------------------------------ Function Prototypes ------------------------------------ */
/*!
 * @brief   accesses 8-bit register and returns its contents
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   reg                 register to read from
 * @return  uint8_t             data contained in register
 */
uint8_t DRV2605_ReadRegister(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t reg);

/*!
 * @brief   accesses 8-bit register and writes content to its address
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   reg                 register to write to
 * @param   val                 data to write to register
 */
void DRV2605_WriteRegister(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t reg, uint8_t data);

/*!
 * @brief   haptic motor initialization sequence
 * @param   DRV2605_HI2C_INST   motor I2C instance
 */
void DRV2605_Init(I2C_HandleTypeDef* DRV2605_HI2C_INST);

/*!
 * @brief   initializes all haptic motors, sets their default libraries and modes
 */
void DRV2605_Begin(void);

/*!
 * @brief   used to select the waveform effects library
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   lib                 library index
 */
void DRV2605_SelectLibrary(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t lib);

/*!
 * @brief   sets the functional mode for the haptic motor as described in 7.4.2 of the datasheet
 *          (https://cdn-shop.adafruit.com/datasheets/DRV2605.pdf)
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   mode                functional mode
 */
void DRV2605_SetMode(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t mode);

/*!
 * @brief   sets the waveform to be played on the haptic motor 
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   slot                offset for the waveform sequence register
 * @param   waveform            waveform to be played on the haptic motor
 */
void DRV2605_SetWaveform(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t slot, uint8_t waveform);

/*!
 * @brief   plays the waveform on the haptic motor
 * @param   DRV2605_HI2C_INST   motor I2C instance
 */
void DRV2605_Go(I2C_HandleTypeDef* DRV2605_HI2C_INST);
