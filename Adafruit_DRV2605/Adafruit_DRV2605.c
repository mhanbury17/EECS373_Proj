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
 *          SCL     I2C1_SCL        PB_8
 *          SDA     I2C1_SDA        PB_9
 *          SCL     I2C2_SCL        PB_10
 *          SDA     I2C2_SDA        PB_11
 *          SCL     I2C3_SCL        PA_7
 *          SDA     I2C3_SDA        PB_4
 *          SCL     I2C4_SCL        PF_14
 *          SDA     I2C4_SDA        PF_15
 *
 * @author  Miles Hanbury (mhanbury)
 * @author  Joshua Nye (jnye)
 * @author  James Kelley (jkelly)
 */

#include "Adafruit_DRV2605.h"

/* ------------------------------------- Global Variables -------------------------------------- */
extern I2C_HandleTypeDef* DRV2605_HI2C_INST;

/* --------------------------------- Function Implementations ---------------------------------- */
/*!
 * @brief   accesses 8-bit register and returns its contents
 * @param   reg         register to read from
 * @return  uint8_t     data contained in register
 */
uint8_t DRV2605_ReadRegister(uint8_t reg)
{
    uint8_t buffer[1];
    HAL_I2C_Mem_Read(DRV2605_HI2C_INST, DRV2605_ADDR_R, reg, I2C_MEMADD_SIZE_8BIT,
        (uint8_t*)buffer, 1, HAL_MAX_DELAY);
    return buffer[0];
}

/*!
 * @brief   accesses 8-bit register and writes content to its address
 * @param   reg         register to write to
 * @param   val         data to write to register
 */
void DRV2605_WriteRegister(uint8_t reg, uint8_t data)
{
    uint8_t buffer[2] = {reg, data};
    HAL_I2C_Master_Transmit(DRV2605_HI2C_INST, DRV2605_ADDR_W, buffer, 2, HAL_MAX_DELAY);
}

/*!
 * @brief   haptic motor initialization sequence
 */
void DRV2605_Init(void)
{
    uint8_t id = DRV2605_ReadRegister(DRV2605_REG_STATUS);

    DRV2605_WriteRegister(DRV2605_REG_MODE, 0x00);                                                  // out of standby
    DRV2605_WriteRegister(DRV2605_REG_RTPIN, 0x00);                                                 // no real-time-playback
    DRV2605_WriteRegister(DRV2605_REG_WAVESEQ1, 0x01);                                              // strong click
    DRV2605_WriteRegister(DRV2605_REG_WAVESEQ2, 0x00);                                              // end sequence
    DRV2605_WriteRegister(DRV2605_REG_OVERDRIVE, 0x00);                                             // no overdrive
    DRV2605_WriteRegister(DRV2605_REG_SUSTAINPOS, 0x00);
    DRV2605_WriteRegister(DRV2605_REG_SUSTAINNEG, 0x00);
    DRV2605_WriteRegister(DRV2605_REG_BREAK, 0x00);
    DRV2605_WriteRegister(DRV2605_REG_AUDIOMAX, 0x64);

    // ERM open loop

    DRV2605_WriteRegister(DRV2605_REG_FEEDBACK, DRV2605_ReadRegister(DRV2605_REG_FEEDBACK) & 0x7F); // turn off N_ERM_LRA
    DRV2605_WriteRegister(DRV2605_REG_CONTROL3, DRV2605_ReadRegister(DRV2605_REG_CONTROL3) | 0x20); // turn on ERM_OPEN_LOOP
}

/*!
 * @brief   used to select the waveform effects library
 * @param   lib         library index
 */
void DRV2605_SelectLibrary(uint8_t lib)
{
    DRV2605_WriteRegister(DRV2605_REG_LIBRARY, lib);
}

/*!
 * @brief   sets the functional mode for the haptic motor as described in 7.4.2 of the datasheet
 *          (https://cdn-shop.adafruit.com/datasheets/DRV2605.pdf)
 * @param   mode        functional mode
 */
void DRV2605_SetMode(uint8_t mode)
{
    DRV2605_WriteRegister(DRV2605_REG_MODE, mode);
}

/*
 * @brief   sets the waveform to be played on the haptic motor 
 * @param   slot        offset for the waveform sequence register
 * @param   waveform    waveform to be played on the haptic motor
 */
void DRV2605_SetWaveform(uint8_t slot, uint8_t waveform)
{
    DRV2605_WriteRegister(DRV2605_REG_WAVESEQ1 + slot, waveform);
}

/*!
 * @brief   plays the waveform on the haptic motor
 */
void DRV2605_Go(void)
{
    DRV2605_WriteRegister(DRV2605_REG_GO, 0x01);
}