/*!
 * @file    Adafruit_STMPE610.c
 * @brief   Driver code for Adafruit STMPE610 Resistive Touch Screen Controller
 *          (https://www.adafruit.com/product/1571)
 * @ref     Initialization sequence and large part of defines taken from Adafruits github
 *          repository for this device (https://github.com/adafruit/Adafruit_STMPE610)
 * @note    This code is written with intention of using I2C to communicate to the
 *          STM32 L031K6 board and uses the following pinout on the resistive touch
 *          controller:
 *
 *          PINOUT  LABEL           PORT/PIN
 *          --------------------------------
 *          3v3      -> Vin
 *          D5       -> SCL
 *
 * @author  Miles Hanbury (mhanbury)
 * @author  James Kelly (jkellymi)
 * @author  Joshua Nye (nyej)
 */

#include "Adafruit_STMPE610.h"

/* ------------------------------------- Global Variables -------------------------------------- */
extern I2C_HandleTypeDef* STMPE610_HI2C_INST;

/* -------------------------------- Read/Write Cycle Sequences --------------------------------- */
/*!
 * @brief   reads a byte from a register specified by reg
 * @param   reg         register to be read from
 * @return  uint8_t     information in register
 */
uint8_t STMPE610_ReadRegister8(uint8_t reg)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t buff[1];
    ret = HAL_I2C_Mem_Read(STMPE610_HI2C_INST, STMPE610_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
        (uint8_t*)buff, 1, HAL_MAX_DELAY);
    return buff[0];
}

/*!
 * @brief   reads 2-bytes from a register specified by reg
 * @param   reg         register to be read from
 * @return  uint16_t    information in register
 */
uint16_t STMPE610_ReadRegister16(uint8_t reg)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t buff[2];
    ret = HAL_I2C_Mem_Read(STMPE610_HI2C_INST, STMPE610_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
        (uint8_t*)buff, 2, HAL_MAX_DELAY);
    return (((uint16_t)buff[0] << 8) | (uint16_t)buff[1]);
}

/*!
 * @brief   writes a byte of data to a register specified by reg
 * @param   addr                address of register to write to
 * @param   data                data to put in register
 * @return  HAL_StatusTypeDef   hi2c instance
 */
HAL_StatusTypeDef STMPE610_WriteRegister8(uint8_t addr, uint8_t data)
{
    uint8_t buff[2];
    buff[0] = addr;
    buff[1] = data;
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_I2C_Master_Transmit(STMPE610_HI2C_INST, STMPE610_ADDR, buff, 2, HAL_MAX_DELAY);
    return ret;
}

/* --------------------------------------- Initialization -------------------------------------- */
/*!
 * @brief   returns the current operating version
 * @return  uint16_t    version
 */
uint16_t STMPE610_GetVersion(void)
{
    return STMPE610_ReadRegister16((uint8_t)0x00);
}

/*!
 * @brief   writes a series of commands and data to initialize the display
 * @note    must be run before attempting to do any other transactions at the start of a program
 */
void STMPE610_Init(void)
{
    if(STMPE610_GetVersion() != 0x811)
    	return 0;                                                    // check version = 0x811 to make sure i2c is working

    STMPE610_WriteRegister8((uint8_t*)STMPE610_SYS_CTRL1, (uint8_t*)STMPE610_SYS_CTRL1_RESET);      // software reset
    HAL_Delay(20);

    for(uint8_t i = 0; i < 65; i++) STMPE610_ReadRegister8(i);                                      // read registers for initialization procedure

    uint8_t cmd_idx = 0;                                                                            // initialization command set index
    while (*(STMPE610_InitCMDs + cmd_idx) != 0x00)                                                  // runs through initialization sequence until terminator is reached
    {
        uint8_t addr = *(STMPE610_InitCMDs + cmd_idx++);
        uint8_t data = *(STMPE610_InitCMDs + cmd_idx++);
        STMPE610_WriteRegister8(addr, data);
    }
}

/* ----------------------------------- Touch Checks / Returns ---------------------------------- */
/*!
 * @brief   returns if the display has been touched
 * @return  uint8_t     returns true if touched
 */
uint8_t STMPE610_Touched()
{
    return (STMPE610_ReadRegister8(STMPE610_TSC_CTRL) & 0x80);
}

/*!
 * @brief   returns if buffer is empty
 * @return  uint8_t     returns true if empty
 */
uint8_t STMPE610_BufferEmpty()
{
    return (STMPE610_ReadRegister8(STMPE610_FIFO_STA) & STMPE610_FIFO_STA_EMPTY);
}

/*!
 * @brief   gets the point being currently touched
 * @return  TSPoint     point of touch
 */
TSPoint STMPE610_GetPoint()
{
    TSPoint point = {0, 0, 0};
    uint8_t data[4];
    while(!STMPE610_BufferEmpty())
        for(uint8_t i = 0; i < 4; ++i)
            data[i] = STMPE610_ReadRegister8(0xD7);
    point.y = ((data[0] << 4) | ((data[1] & 0xF0) >> 4));
    point.y = (int16_t)((((float)point.y - 400.0)/3700.0) * 270);                                   // Grab the y coordinate and convert to pixels
    point.x = (((data[1] & 0x0F) << 8) | data[2]);
    point.x = (int16_t)((((float)point.x - 400.0)/3700.0) * 370);                                   // Grab the x coordinate and convert to pixels
    point.z = data[3]; // Grab the Z coordinate
    STMPE610_WriteRegister8((uint8_t)STMPE610_INT_STA, (uint8_t *)0xFF);                            // reset all ints
    return point;
}

/**
 * @brief   returns if a defined area has been touched on the display
 * @param   point       point touched
 * @param   x           x position on display to check
 * @param   y           y position on display to check
 * @return  uint8_t     returns true if that area has been touched
 */
uint8_t STMPE610_TouchedArea(TSPoint* point, int16_t x, int16_t y)
{
    if(x > (point->x - 20) && x < (point->x + 20) && y > (point->y - 20) && y < (point->y + 20))
    {
        return 1;
    }
    return 0;
}

/**
 * @brief   reset the point
 * @param   point   point to be reset
 */
void STMPE610_ClearPoint(TSPoint* point)
{
    point->x = -21;
    point->y = -21;
    point->z = 0;
}
