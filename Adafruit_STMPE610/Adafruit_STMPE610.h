/*!
 * @file    Adafruit_STMPE610.h
 * @brief   Driver code for Adafruit STMPE610 Resistive Touch Screen Controller
 *          (https://www.adafruit.com/product/1571)
 * @note    This code is written with intention of using I2C to communicate to the
 *          STM32 L031K6 board and uses the following pinout on the resistive touch
 *          controller:
 *
 *          PINOUT  I2C             PORT/PIN
 *          --------------------------------
 *          [IN PROGRESS]
 *
 * @author  Miles Hanbury (mhanbury)
 * @author  Joshua Nye (jnye)
 * @author  James Kelley (jkelly)
 */

#include "stm32l0xx_hal.h"

/* ------------------------------ Register Address and Data Set ------------------------------- */
#define STMPE610_ADDR 0x82

#define STMPE610_SYS_CTRL1 0x03                                                                     /* Reset Control */
#define STMPE610_SYS_CTRL1_RESET 0x02
#define STMPE610_SYS_CTRL1_CLEAR 0x00

#define STMPE610_CHIP_ID 0x00                                                                       /* Clock Contrl */
#define STMPE610_SYS_CTRL2 0x04
#define STMPE610_SYS_CTRL2_EN_CLKS 0x08

#define STMPE610_TSC_CTRL 0x40                                                                      /* Touchscreen controller setup */
#define STMPE610_TSC_CTRL_EN 0x01
#define STMPE610_TSC_CTRL_XYZ 0x00
#define STMPE610_TSC_CTRL_XY 0x02

#define STMPE610_INT_CTRL 0x09                                                                      /* Interrupt control */
#define STMPE610_INT_CTRL_POL_HIGH 0x04
#define STMPE610_INT_CTRL_POL_LOW 0x00
#define STMPE610_INT_CTRL_EDGE 0x02
#define STMPE610_INT_CTRL_LEVEL 0x00
#define STMPE610_INT_CTRL_ENABLE 0x01
#define STMPE610_INT_CTRL_DISABLE 0x00

#define STMPE610_INT_EN 0x0A                                                                        /* Interrupt enable */
#define STMPE610_INT_EN_TOUCHDET 0x01
#define STMPE610_INT_EN_FIFOTH 0x02
#define STMPE610_INT_EN_FIFOOF 0x04
#define STMPE610_INT_EN_FIFOFULL 0x08
#define STMPE610_INT_EN_FIFOEMPTY 0x10
#define STMPE610_INT_EN_ADC 0x40
#define STMPE610_INT_EN_GPIO 0x80

#define STMPE610_INT_STA 0x0B                                                                       /* Interrupt status */
#define STMPE610_INT_STA_TOUCHDET 0x01

#define STMPE610_ADC_CTRL1 0x20                                                                     /* ADC control */
#define STMPE610_ADC_CTRL1_12BIT 0x08
#define STMPE610_ADC_CTRL1_10BIT 0x00

#define STMPE610_ADC_CTRL2 0x21                                                                     /* ADC control */
#define STMPE610_ADC_CTRL2_1_625MHZ 0x00
#define STMPE610_ADC_CTRL2_3_25MHZ 0x01
#define STMPE610_ADC_CTRL2_6_5MHZ 0x02

#define STMPE610_TSC_CFG 0x41                                                                       /* Touchscreen controller configuration */
#define STMPE610_TSC_CFG_1SAMPLE 0x00
#define STMPE610_TSC_CFG_2SAMPLE 0x40
#define STMPE610_TSC_CFG_4SAMPLE 0x80
#define STMPE610_TSC_CFG_8SAMPLE 0xC0
#define STMPE610_TSC_CFG_DELAY_10US 0x00
#define STMPE610_TSC_CFG_DELAY_50US 0x08
#define STMPE610_TSC_CFG_DELAY_100US 0x10
#define STMPE610_TSC_CFG_DELAY_500US 0x18
#define STMPE610_TSC_CFG_DELAY_1MS 0x20
#define STMPE610_TSC_CFG_DELAY_5MS 0x28
#define STMPE610_TSC_CFG_DELAY_10MS 0x30
#define STMPE610_TSC_CFG_DELAY_50MS 0x38
#define STMPE610_TSC_CFG_SETTLE_10US 0x00
#define STMPE610_TSC_CFG_SETTLE_100US 0x01
#define STMPE610_TSC_CFG_SETTLE_500US 0x02
#define STMPE610_TSC_CFG_SETTLE_1MS 0x03
#define STMPE610_TSC_CFG_SETTLE_5MS 0x04
#define STMPE610_TSC_CFG_SETTLE_10MS 0x05
#define STMPE610_TSC_CFG_SETTLE_50MS 0x06
#define STMPE610_TSC_CFG_SETTLE_100MS 0x07

#define STMPE610_FIFO_TH 0x4A                                                                       /* FIFO level to generate interrupt */

#define STMPE610_FIFO_SIZE 0x4C                                                                     /* Current filled level of FIFO */

#define STMPE610_FIFO_STA 0x4B                                                                      /* Current status of FIFO */
#define STMPE610_FIFO_STA_RESET 0x01
#define STMPE610_FIFO_STA_OFLOW 0x80
#define STMPE610_FIFO_STA_FULL 0x40
#define STMPE610_FIFO_STA_EMPTY 0x20
#define STMPE610_FIFO_STA_THTRIG 0x10

#define STMPE610_TSC_I_DRIVE 0x58                                                                   /* Touchscreen controller drive I */
#define STMPE610_TSC_I_DRIVE_20MA 0x00
#define STMPE610_TSC_I_DRIVE_50MA 0x01

#define STMPE610_TSC_DATA_X 0x4D                                                                    /* Data port for TSC data address */
#define STMPE610_TSC_DATA_Y 0x4F
#define STMPE610_TSC_FRACTION_Z 0x56

#define STMPE610_GPIO_SET_PIN 0x10                                                                  /* GPIO */
#define STMPE610_GPIO_CLR_PIN 0x11
#define STMPE610_GPIO_DIR 0x13
#define STMPE610_GPIO_ALT_FUNCT 0x17

/* -------------------------------- Initialization Command Set --------------------------------- */
static const uint8_t STMPE610_InitCMDs[] =
{
    STMPE610_SYS_CTRL2,         0x00,
    STMPE610_TSC_CTRL,          (STMPE610_TSC_CTRL_XYZ | STMPE610_TSC_CTRL_EN),
    STMPE610_INT_EN,            STMPE610_INT_EN_TOUCHDET,
    STMPE610_ADC_CTRL1,         (STMPE610_ADC_CTRL1_10BIT | (0x6 << 4)),
    STMPE610_ADC_CTRL2,         STMPE610_ADC_CTRL2_6_5MHZ,
    STMPE610_TSC_CFG,           (STMPE610_TSC_CFG_4SAMPLE | STMPE610_TSC_CFG_DELAY_1MS | STMPE610_TSC_CFG_SETTLE_5MS),
    STMPE610_TSC_FRACTION_Z,    0x6,
    STMPE610_FIFO_TH,           1,
    STMPE610_FIFO_STA,          STMPE610_FIFO_STA_RESET,
    STMPE610_FIFO_STA,          0,
    STMPE610_TSC_I_DRIVE,       STMPE610_TSC_I_DRIVE_50MA,
    STMPE610_INT_STA,           0xFF,
    STMPE610_INT_CTRL,          (STMPE610_INT_CTRL_POL_HIGH | STMPE610_INT_CTRL_ENABLE),
    0x00
};


/* ------------------------------------- Global Variables -------------------------------------- */
extern I2C_HandleTypeDef* STMPE610_HI2C_INST;

/* ---------------------------------------- Structures ----------------------------------------- */
typedef struct TSPoint_STRUCT {
	int16_t x;
	int16_t y;
	int16_t z;
} TSPoint;

/* -------------------------------- Read/Write Cycle Sequences --------------------------------- */
/*!
 * @brief   reads a byte from a register specified by reg
 * @param   reg         register to be read from
 * @return  uint8_t     information in register
 */
uint8_t STMPE610_ReadRegister8(uint8_t reg);

/*!
 * @brief   reads 2-bytes from a register specified by reg
 * @param   reg         register to be read from
 * @return  uint16_t    information in register
 */
uint16_t STMPE610_ReadRegister16(uint8_t reg);

/*!
 * @brief   writes a byte of data to a register specified by reg
 * @param   addr                address of register to write to
 * @param   data                data to put in register
 * @return  HAL_StatusTypeDef   hi2c instance
 */
HAL_StatusTypeDef STMPE_WriteRegister8(uint8_t addr, uint8_t data);

/* --------------------------------------- Initialization -------------------------------------- */
/*!
 * @brief   returns the current operating version
 * @return  uint16_t    version
 */
uint16_t STMPE610_GetVersion(void);

/*!
 * @brief   writes a series of commands and data to initialize the display
 * @note    must be run before attempting to do any other transactions at the start of a program
 */
void STMPE610_Init(void);

/* ----------------------------------- Touch Checks / Returns ---------------------------------- */
/*!
 * @brief   returns if the display has been touched
 * @return  uint8_t     returns true if touched
 */
uint8_t STMPE610_Touched();

/*!
 * @brief   returns if buffer is empty
 * @return  uint8_t     returns true if empty
 */
uint8_t STMPE610_BufferEmpty();

/*!
 * @brief   gets the point being currently touched
 * @return  TSPoint     point of touch
 */
TSPoint STMPE610_GetPoint();

/*!
 * @brief   returns if a defined area has been touched on the display
 * @param   point       point touched
 * @param   x           x position on display to check
 * @param   y           y position on display to check
 * @return  uint8_t     returns true if that area has been touched
 */
uint8_t STMPE610_TouchedArea(TSPoint point, int16_t x, int16_t y);

/*!
 * @brief   reset the point
 * @param   point   point to be reset
 */
void STMPE610_ClearPoint(TSPoint* point);
