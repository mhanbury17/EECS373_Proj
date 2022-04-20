/*!
 * @file    Adafruit_DRV2605.c
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
 * @author  Joshua Nye (jnye)
 * @author  James Kelly (jkellymi)
 */

#include "Adafruit_DRV2605.h"

/* ------------------------------------- Global Variables -------------------------------------- */
extern I2C_HandleTypeDef* DRV2605_HI2C_INST1;
extern I2C_HandleTypeDef* DRV2605_HI2C_INST2;
extern I2C_HandleTypeDef* DRV2605_HI2C_INST3;
extern I2C_HandleTypeDef* DRV2605_HI2C_INST4;

/* --------------------------------- Function Implementations ---------------------------------- */
/*!
 * @brief   accesses 8-bit register and returns its contents
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   reg                 register to read from
 * @return  uint8_t             data contained in register
 */
uint8_t DRV2605_ReadRegister(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t reg)
{
    uint8_t buffer[1];
    HAL_I2C_Mem_Read(DRV2605_HI2C_INST, DRV2605_ADDR_R, reg, I2C_MEMADD_SIZE_8BIT,
        (uint8_t*)buffer, 1, HAL_MAX_DELAY);
    return buffer[0];
}

/*!
 * @brief   accesses 8-bit register and writes content to its address
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   reg                 register to write to
 * @param   val                 data to write to register
 */
void DRV2605_WriteRegister(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t reg, uint8_t data)
{
    uint8_t buffer[2] = {reg, data};
    HAL_I2C_Master_Transmit(DRV2605_HI2C_INST, DRV2605_ADDR_W, buffer, 2, HAL_MAX_DELAY);
}

/*!
 * @brief   haptic motor initialization sequence
 * @param   DRV2605_HI2C_INST   motor I2C instance
 */
void DRV2605_Init(I2C_HandleTypeDef* DRV2605_HI2C_INST)
{
    uint8_t id = DRV2605_ReadRegister(DRV2605_HI2C_INST, DRV2605_REG_STATUS);

    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_MODE, 0x00);                               // out of standby
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_RTPIN, 0x00);                              // no real-time-playback
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_WAVESEQ1, 0x01);                           // strong click
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_WAVESEQ2, 0x00);                           // end sequence
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_OVERDRIVE, 0x00);                          // no overdrive
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_SUSTAINPOS, 0x00);
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_SUSTAINNEG, 0x00);
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_BREAK, 0x00);
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_AUDIOMAX, 0x64);

    // ERM open loop

    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_FEEDBACK, 
        DRV2605_ReadRegister(DRV2605_HI2C_INST, DRV2605_REG_FEEDBACK) & 0x7F);                      // turn off N_ERM_LRA
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_CONTROL3, 
        DRV2605_ReadRegister(DRV2605_HI2C_INST, DRV2605_REG_CONTROL3) | 0x20);                      // turn on ERM_OPEN_LOOP
}

/*!
 * @brief   initializes all haptic motors, sets their default libraries and modes
 */
void DRV2605_Begin(void)
{
    DRV2605_Init(DRV2605_HI2C_INST1);
    DRV2605_Init(DRV2605_HI2C_INST2);
    DRV2605_Init(DRV2605_HI2C_INST3);
    DRV2605_Init(DRV2605_HI2C_INST4);

    DRV2605_SelectLibrary(DRV2605_HI2C_INST1, 1);
    DRV2605_SelectLibrary(DRV2605_HI2C_INST2, 1);
    DRV2605_SelectLibrary(DRV2605_HI2C_INST3, 1);
    DRV2605_SelectLibrary(DRV2605_HI2C_INST4, 1);

    DRV2605_SetMode(DRV2605_HI2C_INST1, DRV2605_MODE_INTTRIG);
    DRV2605_SetMode(DRV2605_HI2C_INST2, DRV2605_MODE_INTTRIG);
    DRV2605_SetMode(DRV2605_HI2C_INST3, DRV2605_MODE_INTTRIG);
    DRV2605_SetMode(DRV2605_HI2C_INST4, DRV2605_MODE_INTTRIG);
}

/*!
 * @brief   used to select the waveform effects library
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   lib                 library index
 */
void DRV2605_SelectLibrary(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t lib)
{
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_LIBRARY, lib);
}

/*!
 * @brief   sets the functional mode for the haptic motor as described in 7.4.2 of the datasheet
 *          (https://cdn-shop.adafruit.com/datasheets/DRV2605.pdf)
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   mode                functional mode
 */
void DRV2605_SetMode(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t mode)
{
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_MODE, mode);
}

/*!
 * @brief   sets the waveform to be played on the haptic motor 
 * @param   DRV2605_HI2C_INST   motor I2C instance
 * @param   slot                offset for the waveform sequence register
 * @param   waveform            waveform to be played on the haptic motor
 */
void DRV2605_SetWaveform(I2C_HandleTypeDef* DRV2605_HI2C_INST, uint8_t slot, uint8_t waveform)
{
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_WAVESEQ1 + slot, waveform);
}

/*!
 * @brief   plays the waveform on the haptic motor
 * @param   DRV2605_HI2C_INST   motor I2C instance
 */
void DRV2605_Go(I2C_HandleTypeDef* DRV2605_HI2C_INST)
{
    DRV2605_WriteRegister(DRV2605_HI2C_INST, DRV2605_REG_GO, 0x01);
}
