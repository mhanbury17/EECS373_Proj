/*!
 * @file    Adafruit_SPH0645.c
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

#include "Adafruit_SPH0645.h"

/* -------------------------------------- Global Variables ------------------------------------- */
extern SAI_HandleTypeDef* HSAI_BLOCK_A1;
extern SAI_HandleTypeDef* HSAI_BLOCK_A2;
extern SAI_HandleTypeDef* HSAI_BLOCK_B1;
extern SAI_HandleTypeDef* HSAI_BLOCK_B2;

static int SAMPLES_A1[SAMPLES];
static int SAMPLES_A2[SAMPLES];
static int SAMPLES_B1[SAMPLES];
static int SAMPLES_B2[SAMPLES];

/* ---------------------------------- Function Implementations --------------------------------- */
/*!
 * @brief   samples a microphone
 * @param   hsai    block to sample
 * @return  double  sample value
 */
int SPH0645_Sample(SAI_HandleTypeDef* hsai)
{
    int sample = 0;
    uint8_t buffer[1] = {0};
    while (sample == 0 || sample == -1)
    {
        HAL_StatusTypeDef res = HAL_SAI_Receive(&hsai, buffer, 1, 1);
        sample = (double)buffer[0];
    }
    return sample >>= 14;
}

/*!
 * @brief   samples all block sets
 */
void SPH0645_SampleAll(void)
{
    for (int i = 0; i < SAMPLES; i++)
    {
        SAMPLES_A1[i] = SPH0645_Sample(HSAI_BLOCK_A1);
        SAMPLES_A2[i] = SPH0645_Sample(HSAI_BLOCK_A2);
        SAMPLES_B1[i] = SPH0645_Sample(HSAI_BLOCK_B1);
        SAMPLES_B2[i] = SPH0645_Sample(HSAI_BLOCK_B2);
    }
}

/*!
 * @brief   gets the average of a sample set
 * @param   _samples    sample set
 * @return  double      average
 */
double SPH0645_GetAverage(int* _samples)
{
    double avg = 0;
    for (int i = 0; i < SAMPLES; i++) avg += (double)(_samples[i]);
    avg /= SAMPLES;
}

/*!
 * @brief   normalize sample set by subtracting the average from each sample
 * @param   _samples    sample set
 * @param   avg         average of set
 */
void SPH0645_Normalize(int* _samples, double avg)
{
    for (int i = 0; i < SAMPLES; i++) _samples[i] -= avg;
}

/*!
 * @brief   normalize all block sets
 */
void SPH0645_NormalizeAll()
{
    SPH0645_Normalize(SAMPLES_A1, SPH0645_GetAverage(SAMPLES_A1));
    SPH0645_Normalize(SAMPLES_A2, SPH0645_GetAverage(SAMPLES_A2));
    SPH0645_Normalize(SAMPLES_B1, SPH0645_GetAverage(SAMPLES_B1));
    SPH0645_Normalize(SAMPLES_B2, SPH0645_GetAverage(SAMPLES_B2));
}

/*!
 * @brief   gets the minumum value of a sample set
 * @param   _samples    sample set
 * @return  double      minimum value in set
 */
double SPH0645_GetMinSample(int* _samples)
{
    double min = _samples[0];
    for (int i = 1; i < SAMPLES; i++)
        if (_samples[i] < min) 
            min = _samples[i];
    return min;
}

/*!
 * @brief   gets the maximum value of a sample set
 * @param   _samples    sample set 
 * @return  double      maximum value in set
 */
double SPH0645_GetMaxSample(int* _samples)
{
    double max = _samples[0];
    for (int i = 1; i < SAMPLES; i++)
        if (_samples[i] > max) 
            max = _samples[i];
    return max;
}

/*!
 * @brief   squares two values and then returns the ratio of them
 * @param   a           numerator value
 * @param   b           denomanator value
 * @return  double      squared ratio
 */
double RatioSquared(double a, double b)
{
    return (a*a)/(b*b);
}

/*!
 * @brief   compares sample ranges to threshold to determine angle
 * @return  int         determined angle
 * @note    returns -1 if no angle is determined as to allow caller to determine default
 */
int SPH0645_GetAngle(void)
{
    SPH0645_SampleAll();                                                                            // samples all microphones

    SPH0645_Normalize(SAMPLES_A1, SPH0645_GetAverage(SAMPLES_A1));                                  // normalize block A1 samples
    SPH0645_Normalize(SAMPLES_A2, SPH0645_GetAverage(SAMPLES_A2));                                  // normalize block A2 samples
    SPH0645_Normalize(SAMPLES_B1, SPH0645_GetAverage(SAMPLES_B1));                                  // normalize block B1 samples
    SPH0645_Normalize(SAMPLES_B2, SPH0645_GetAverage(SAMPLES_B2));                                  // normalize block B2 samples
    
    double rangeA1 = SPH0645_GetMaxSample(SAMPLES_A1) - SPH0645_GetMinSample(SAMPLES_A1);           // range of block A1 values
    double rangeA2 = SPH0645_GetMaxSample(SAMPLES_A2) - SPH0645_GetMinSample(SAMPLES_A2);           // range of block A2 values
    double rangeB1 = SPH0645_GetMaxSample(SAMPLES_B1) - SPH0645_GetMinSample(SAMPLES_B1);           // range of block B1 values
    double rangeB2 = SPH0645_GetMaxSample(SAMPLES_B2) - SPH0645_GetMinSample(SAMPLES_B2);           // range of block B2 values

    if      (RatioSquared(rangeA2,rangeA1)>T1 && RatioSquared(rangeB2,rangeB1)<T4 &&                // compare ranges to thresholds to determine angle
             RatioSquared(rangeB1,rangeB2)<T4 ||(RatioSquared(rangeA2,rangeA1)>T5 &&
             RatioSquared(rangeB2,rangeB1)<T2 && RatioSquared(rangeB1,rangeB2)<T2)) return 180;
    else if (RatioSquared(rangeA1,rangeA2)>T1 && RatioSquared(rangeB2,rangeB1)<T4 &&
             RatioSquared(rangeB1,rangeB2)<T4 ||(RatioSquared(rangeA2,rangeA1)>T5 &&
             RatioSquared(rangeB2,rangeB1)<T2 && RatioSquared(rangeB1,rangeB2)<T2)) return 0;
    else if (RatioSquared(rangeB2,rangeB1)>T1 && RatioSquared(rangeA2,rangeA1)<T4 &&
             RatioSquared(rangeA1,rangeA2)<T4 ||(RatioSquared(rangeB2,rangeB1)>T5 &&
             RatioSquared(rangeA2,rangeA1)<T2 && RatioSquared(rangeA1,rangeA2)<T2)) return 270;
    else if (RatioSquared(rangeB1,rangeB2)>T1 && RatioSquared(rangeA2,rangeA1)<T4 &&
             RatioSquared(rangeA1,rangeA2)<T4 ||(RatioSquared(rangeB2,rangeB1)>T5 &&
             RatioSquared(rangeA2,rangeA1)<T2 && RatioSquared(rangeA1,rangeA2)<T2)) return 90;
    else if (RatioSquared(rangeA1,rangeA2)>T3 && RatioSquared(rangeB1,rangeB2)>T3)  return 45;
    else if (RatioSquared(rangeA2,rangeA1)>T3 && RatioSquared(rangeB1,rangeB2)>T3)  return 135;
    else if (RatioSquared(rangeA2,rangeA1)>T3 && RatioSquared(rangeB2,rangeB1)>T3)  return 225;
    else if (RatioSquared(rangeA1,rangeA2)>T3 && RatioSquared(rangeB2,rangeB1)>T3)  return 315;
    else return -1;
}