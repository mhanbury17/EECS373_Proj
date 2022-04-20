/*!
 * @file    Adafruit_SPH0645.h
 * @brief   Driver code for the Adafruit MEMs microphones used for sound localization
 * @note    This code is written with the intention of using I2S to communicate to the
 *          STM32 L4R5ZI-P board and uses the following pinout.
 * 
 *          PINOUT      LABEL           PORT/PIN
 *          ------------------------------------
 *          SAI1_SCK_A  Block A1        PE5
 *          SAI1_FS_A   --              PE4
 *          SAI1_SD_A   --              PE6
 *          SAI1_SCK_B  Block B1        PF8
 *          SAI1_FS_B   --              PF9
 *          SAI1_SD_B   --              PE3
 *          SAI2_SCK_A  Block A2        PB13
 *          SAI2_FS_A   --              PB12
 *          SAI2_SD_A   --              PB15
 *          SAI2_SCK_B  Block B2        PG2
 *          SAI2_FS_B   --              PG3
 *          SAI2_SD_B   --              PG5
 *
 * @author  Miles Hanbury (mhanbury)
 * @author  Joshua Nye (jnye)
 * @author  James Kelley (jkelly)
 */

#include "stm32l4xx_hal.h"

/* --------------------------------- Localization Definitions ---------------------------------- */
#define SAMPLES 1024

#define T1     (double)(2.80)
#define T2     (double)(1.15)
#define T3     (double)(1.50)
#define T4     (double)(2.60)
#define T5     (double)(2.10)

/* ------------------------------------ Function Prototypes ------------------------------------ */
/*!
 * @brief   samples a microphone
 * @param   hsai    block to sample
 * @return  int  sample value
 */
int SPH0645_Sample(SAI_HandleTypeDef* hsai);

/*!
 * @brief   samples all block sets
 */
void SPH0645_SampleAll(void);

/*!
 * @brief   gets the average of a sample set
 * @param   _samples    sample set
 * @return  double      average
 */
double SPH0645_GetAverage(int* _samples);

/*!
 * @brief   normalize sample set by subtracting the average from each sample
 * @param   _samples    sample set
 * @param   avg         average of set
 */
void SPH0645_Normalize(int* _samples, double avg);

/*!
 * @brief   normalize all block sets
 */
void SPH0645_NormalizeAll();

/*!
 * @brief   gets the minumum value of a sample set
 * @param   _samples    sample set
 * @return  double      minimum value in set
 */
double SPH0645_GetMinSample(int* _samples);

/*!
 * @brief   gets the maximum value of a sample set
 * @param   _samples    sample set 
 * @return  double      maximum value in set
 */
double SPH0645_GetMaxSample(int* _samples);

/*!
 * @brief   compares sample ranges to threshold to determine angle
 * @return  int         determined angle
 * @note    returns -1 if no angle is determined as to allow caller to determine default
 */
int SPH0645_GetAngle(void);

/* --------------------------------------------------------------------------------------------- */