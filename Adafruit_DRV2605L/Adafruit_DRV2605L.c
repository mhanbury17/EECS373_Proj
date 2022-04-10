/*!
 * @file    Adafruit_DRV2605L.c
 * @brief   
 * @note    
 *
 * @author  Miles Hanbury (mhanbury)
 * @author  Joshua Nye (jnye)
 * @author  James Kelley (jkelly)
 */


/**************************************************************************/
/*!
  @brief Setup HW using a specified Wire
  @param theWire Pointer to a TwoWire object, defaults to &Wire
  @return Return value from init()
*/
/**************************************************************************/
boolean Adafruit_DRV2605::begin(TwoWire *theWire) {
  if (i2c_dev)
    delete i2c_dev;
  i2c_dev = new Adafruit_I2CDevice(DRV2605_ADDR, theWire);
  return init();
}

