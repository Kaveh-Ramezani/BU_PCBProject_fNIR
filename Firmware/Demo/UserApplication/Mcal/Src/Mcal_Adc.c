/*************************************************************************
                            Includes
*************************************************************************/
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "Mcal_Lcfg.h"
#include "Mcal.h"
/*************************************************************************
                            Macro definition
*************************************************************************/
#define GetAnalogInputHandler(__IDX__)        (g_AinConfig[__IDX__].handler) /*!< Get the analog input handler. */
#define GetAnalogInputChannelNumber(__IDX__)  (g_AinConfig[__IDX__].channelNumber) /*!< Get the analog channel. */
/*************************************************************************
                            Type definition
*************************************************************************/

/*************************************************************************
                            Variables
*************************************************************************/

/*************************************************************************
                            Functions
*************************************************************************/

/**
 * The function `Mcal_Analog_GetValue` reads analog values from a specified index
 * using ADC and returns the averaged result.
 * 
 * @param index The `index` parameter in the `Mcal_Analog_GetValue` function
 * represents the index of the analog input channel for which you want to get the
 * ADC conversion value. It is used to configure the ADC channel and perform ADC
 * operations on the specified input channel.
 * @param ret The `ret` parameter in the `Mcal_Analog_GetValue` function is a
 * pointer to a variable of type `Std_ReturnType`. This parameter is used to store
 * the return status of various function calls within the `Mcal_Analog_GetValue`
 * function. The function updates the value pointed
 * 
 * @return The function `Mcal_Analog_GetValue` returns a `uint16` value, which is
 * the result of the analog input value after processing and averaging.
 */
uint16 Mcal_ADC_GetValue(uint8 index, Std_ReturnType* ret)
{
  /* Assertions */
  assert(index < ADC_CHANNELS);
  /* Local variables*/
  uint64 retData = 0;
  /* Start the ADC */
  *ret = (Std_ReturnType)HAL_ADC_Start(GetAnalogInputHandler(index));
  if(*ret != E_OK)
  {
    return 0;
  }
  /* Poll for the end of conversion */
  *ret = (Std_ReturnType)HAL_ADC_PollForConversion(GetAnalogInputHandler(index),
                          ADC_CONVERSION_TIMEOUT);
  if(*ret != E_OK)
  {
    return 0;
  }
  /* Get the value and stop the ADC */
  retData = (((uint64)HAL_ADC_GetValue(GetAnalogInputHandler(index))) >> 2);
  HAL_ADC_Stop(GetAnalogInputHandler(index));
  
  return retData;
}

void Mcal_ADC_Init(void)
{
  /* First of all, calibration first. */
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
}
