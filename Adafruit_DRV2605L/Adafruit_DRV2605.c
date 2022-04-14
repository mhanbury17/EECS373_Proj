/*!
 * @file    Adafruit_DRV2605.c
 * @brief   
 * @note    
 *
 * @author  Miles Hanbury (mhanbury)
 * @author  Joshua Nye (jnye)
 * @author  James Kelley (jkelly)
 */

#include "Adafruit_DRV2605.h"

/* ------------------------------------- Global Variables -------------------------------------- */
extern I2C_HandleTypeDef* DRV2605_HI2C_INST;

/* ========================================== setup() ========================================== */
/* ---------------------------------------- drv.begin() ---------------------------------------- */
/*!
 * @brief   accesses 8-bit register and returns its contents
 * @param   reg         register to read from
 * @return  uint8_t     data contained in register
 */
uint8_t DRV2605_ReadRegister(uint8_t reg)
{
    uint8_t buffer[1];
    HAL_I2C_Mem_Read(DRV2605_HI2C_INST, DRV2605_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
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
    HAL_I2C_Master_Transmit(DRV2605_HI2C_INST, DRV2605_ADDR, buffer, 2, HAL_MAX_DELAY);
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

    id = DRV2605_ReadRegister(DRV2605_REG_AUDIOMAX);
    //DRV2605_WriteRegister(DRV2605_REG_AUDIOMAX, 0x64);
    DRV2605_WriteRegister(DRV2605_REG_AUDIOMAX, 0x12);
    id = DRV2605_ReadRegister(DRV2605_REG_AUDIOMAX);

    // ERM open loop

    DRV2605_WriteRegister(DRV2605_REG_FEEDBACK, DRV2605_ReadRegister(DRV2605_REG_FEEDBACK) & 0x7F); // turn off N_ERM_LRA
    DRV2605_WriteRegister(DRV2605_REG_CONTROL3, DRV2605_ReadRegister(DRV2605_REG_CONTROL3) | 0x20); // turn on ERM_OPEN_LOOP
}

/* ------------------------------------ drv.selectLibrary(1) ----------------------------------- */
void DRV2605_SelectLibrary(uint8_t lib)
{
    DRV2605_WriteRegister(DRV2605_REG_LIBRARY, lib);
}

/* ----------------------------- drv.setMode(DRV2605_MODE_INTTRIG) ----------------------------- */
void DRV2605_SetMode(uint8_t mode)
{
    DRV2605_WriteRegister(DRV2605_REG_MODE, mode);
}

/* ========================================== loop() =========================================== */
/* ------------------------------------- drv.setWaveform() ------------------------------------- */
void DRV2605_SetWaveform(uint8_t slot, uint8_t waveform)
{
    DRV2605_WriteRegister(DRV2605_REG_WAVESEQ1 + slot, waveform);
}

/* ----------------------------------------- drv.go() ------------------------------------------ */
void DRV2605_Go(void)
{
    DRV2605_WriteRegister(DRV2605_REG_GO, 0x01);
}

void DRV2605_Stop(void)
{
    DRV2605_WriteRegister(DRV2605_REG_GO, 0x00);
}