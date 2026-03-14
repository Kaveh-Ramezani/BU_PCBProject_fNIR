/*************************************************************************
                              Includes
*************************************************************************/
#include "Std_Types.h"
#include "ADS111x_Types.h"
#include "Com_ADS111x.h"
#include "stm32h5xx.h"
#include "stm32h5xx_hal.h"
/*************************************************************************
                            Macro definition
*************************************************************************/
#define I2C_IS_CONNECTED_TRIALS_COUNT 10u

#define IsDeviceConnected()	((Com_MAX1704X_IsConnected() == E_OK) ? TRUE : FALSE) /*!< Checks if the MAX1704x IC is connected or not. */
#define I2CHandle() (ads111x_i2cHandle)
/*************************************************************************
                            Type definition
*************************************************************************/

/*************************************************************************
                            Variables
*************************************************************************/
I2C_HandleTypeDef* ads111x_i2cHandle; /*!< This global variable caries the I2c handler pointer of the MAX1704X IC. */
/*************************************************************************
                            Functions
*************************************************************************/
/* General Function*/
/**
 * The function __mergeTwoUint8 takes an array of two uint8 values and merges them into a single uint16
 * value.
 * 
 * @param inData The parameter "inData" is a pointer to an array of uint8 values.
 * 
 * @return a 16-bit unsigned integer (uint16).
 */
static inline uint16 __mergeTwoUint8(uint8* inData)
{
    return (((uint16)inData[0]) << 8) | (uint16)inData[1];
}
/**
 * The function breaks a 16-bit unsigned integer into two 8-bit unsigned integers.
 * 
 * @param inData The input parameter "inData" is a 16-bit unsigned integer.
 * @param outData The parameter "outData" is a pointer to an array of uint8 values.
 */
static inline void __breakUint16ToTwoUint8(uint16 inData, uint8* outData)
{
    outData[0] = (inData>>8)&0xFF;
    outData[1] = (inData)&0xFF;
}
/* Helper Functions */
/**
 * The function __writeRegAddress writes a register address to an ADS111X device using I2C
 * communication.
 * 
 * @param regAddress The `regAddress` parameter in the `__writeRegAddress` function is of type
 * `ADS111X_RegisterAddressType`, which represents the register address to be written to in the ADS111X
 * device.
 * 
 * @return The function `__writeRegAddress` is returning a value of type `Std_ReturnType`, which is the
 * result of the `HAL_I2C_Master_Transmit` function call.
 */
static Std_ReturnType __writeRegAddress(ADS111X_RegisterAddressType regAddress)
{
  return (Std_ReturnType)HAL_I2C_Master_Transmit( I2CHandle(),
                                                  ADS111X_DEVICE_ADDRESS,
                                                  &regAddress,
                                                  1u,
                                                  I2C_TIMEOUT);
}
/**
 * The function __readRegister reads data from a register using I2C communication.
 * 
 * @param outData The `outData` parameter is a pointer to a uint8 type, which is used to store the data
 * received from the I2C device after a read operation.
 * 
 * @return The function `__readRegister` is returning a value of type `Std_ReturnType`, which is the
 * result of calling the `HAL_I2C_Master_Receive` function with the specified parameters.
 */
static Std_ReturnType __readRegister(uint8* outData)
{
  return (Std_ReturnType)HAL_I2C_Master_Receive(  I2CHandle(),
                                                  ADS111X_DEVICE_ADDRESS,
                                                  outData,
                                                  2u,
                                                  I2C_TIMEOUT);
}
/**
 * The function __writeRegister transmits data over I2C using the HAL library.
 * 
 * @param txData The `txData` parameter in the `__writeRegister` function is a pointer to a uint8 type,
 * which is used to pass the data that needs to be transmitted over I2C. In this case, it is being
 * passed to the HAL_I2C_Master_Transmit function to send
 * 
 * @return The function `__writeRegister` is returning the result of the `HAL_I2C_Master_Transmit`
 * function call, which is of type `Std_ReturnType`.
 */
static Std_ReturnType __writeRegister(uint8 *txData, uint16 size)
{
  return (Std_ReturnType)HAL_I2C_Master_Transmit( I2CHandle(),
                                                  ADS111X_DEVICE_ADDRESS,
                                                  txData,
                                                  size,
                                                  I2C_TIMEOUT);
}

/**
 * The function __writeADS111XRegister writes a 16-bit data value to a specified register address.
 * 
 * @param regAddress The `regAddress` parameter is the address of the register in the ADS111X device
 * that you want to write to.
 * @param inData `inData` is a 16-bit unsigned integer that contains the data to be written to the
 * specified register in the ADS111X device.
 * 
 * @return The function `__writeADS111XRegister` is returning a value of type `Std_ReturnType`, which
 * is either `E_NOT_OK` or the return value from the function `__writeRegister`.
 */
static Std_ReturnType __writeADS111XRegister( ADS111X_RegisterAddressType regAddress,
                                              uint16 inData)
{
  Std_ReturnType ret = E_NOT_OK;
  uint8 txData[3] = {0,0,0};
  /* Break the data into an array */
  txData[0] = regAddress;
  __breakUint16ToTwoUint8(inData, &txData[1]);
  /* Write register address. */
  ret = __writeRegister(txData, sizeof(txData));
  return ret;
}
/**
 * The function __readADS111XRegister reads a register from an ADS111X device and merges the data into
 * a uint16 output.
 * 
 * @param regAddress The `regAddress` parameter in the `__readADS111XRegister` function represents the
 * address of the register that needs to be read from the ADS111X device.
 * @param outData The `outData` parameter in the `__readADS111XRegister` function is a pointer to a
 * `uint16` variable where the data read from the specified register will be stored after the function
 * execution.
 * 
 * @return The function `__readADS111XRegister` returns a value of type `Std_ReturnType`, which is
 * either `E_OK` or `E_NOT_OK` depending on the success of the operations within the function.
 */
static Std_ReturnType __readADS111XRegister(  ADS111X_RegisterAddressType regAddress,
                                              uint16* outData)
{
  Std_ReturnType ret = E_NOT_OK;
  uint8 rxData[2] = {0};
  /* Write register address. */
  ret = __writeRegAddress(regAddress);
  if (ret != E_OK)
  {
    return ret;
  }
  /* Read the register. */
  ret = __readRegister(&rxData[0]);
  if (ret == E_OK)
  {
    *outData = __mergeTwoUint8(rxData);
  }
  return ret;
}


Std_ReturnType Com_ADS111X_Read_ConversionRegister(uint16* outData)
{
  return __readADS111XRegister(ADS111X_ADDRESS_CONVERSION_REG, outData);
}
Std_ReturnType Com_ADS111X_Write_ConfigRegister(uint16 inData)
{
  return __writeADS111XRegister(ADS111X_ADDRESS_COFIG_REG, inData);
}
Std_ReturnType Com_ADS111X_Read_ConfigRegister(uint16* outData)
{
  return __readADS111XRegister(ADS111X_ADDRESS_COFIG_REG, outData);
}
Std_ReturnType Com_ADS111X_Read_LoThreshRegister(uint16* outData)
{
  return __readADS111XRegister(ADS111X_ADDRESS_LO_THRESH_REG, outData);
}
Std_ReturnType Com_ADS111X_Write_LoThreshRegister(uint16 inData)
{
  return __writeADS111XRegister(ADS111X_ADDRESS_LO_THRESH_REG, inData);
}
Std_ReturnType Com_ADS111X_Read_HiThreshRegister(uint16* outData)
{
  return __readADS111XRegister(ADS111X_ADDRESS_HI_THRESH_REG, outData);
}
Std_ReturnType Com_ADS111X_Write_HiThreshRegister(uint16 inData)
{
  return __writeADS111XRegister(ADS111X_ADDRESS_HI_THRESH_REG, inData);
}




/**
 * The function `Com_ADS111X_IsConnected` checks if the MAX1704X device is
 * connected via I2C communication.
 * 
 * @return The function `Com_ADS111X_IsConnected` is returning a `Std_ReturnType`,
 * which is likely an enum or typedef representing a status or error code. The
 * return value is the result of calling `HAL_I2C_IsDeviceReady` with parameters
 * `I2CHandle`, `ADS111X_DEVICE_ADDRESS`, I2C_IS_CONNECTED_TRIALS_COUNT, and `I2C_TIMEOUT`.
 */
Std_ReturnType Com_ADS111X_IsConnected(void)
{
	return (Std_ReturnType)HAL_I2C_IsDeviceReady( I2CHandle(),
						                    ADS111X_DEVICE_ADDRESS,
                                I2C_IS_CONNECTED_TRIALS_COUNT,
                                I2C_TIMEOUT);
}
/**
 * The function `Com_ADS111x_Init` initializes the ADS111x device using the
 * provided I2C handle.
 * 
 * @param hI2C The `hI2C` parameter is a pointer to an `I2C_HandleTypeDef`
 * structure, which is typically used to handle I2C communication in STM32
 * microcontrollers.
 */
void Com_ADS111X_Init(I2C_HandleTypeDef* hI2C)
{
  ads111x_i2cHandle = hI2C;
}
