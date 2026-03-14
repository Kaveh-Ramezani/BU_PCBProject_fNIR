/*************************************************************************
                            Includes
*************************************************************************/
#include "ADS111x.h"
#include "ADS111x_Internal.h"
#include "ADS111x_Types.h"
#include "Com_ADS111x.h"
/*************************************************************************
                            Macro definition
*************************************************************************/
#define IsDeviceConnected()	((Com_ADS111X_IsConnected() == E_OK) ? TRUE : FALSE) /*!< Checks if the MAX1704x IC is connected or not. */
/*************************************************************************
                            Type definition
*************************************************************************/

/*************************************************************************
                            Variables
*************************************************************************/
uint16 g_ADS111X_ConfigReg = 0x8583;
/*************************************************************************
                            Functions
*************************************************************************/
Std_ReturnType ADS111X_Set_ConfigRegister(ADS111X_ConfigRegType* configReg)
{
  /* Assertions */
  assert(configReg->mux < ADS111X_MUX_END);
  assert(configReg->pga < ADS111X_PGA_END);
  assert(configReg->mode < ADS111X_MODE_END);
  assert(configReg->dataRate < ADS111X_DATA_RATE_END);
  assert(configReg->compMode < ADS111X_COMP_MODE_END);
  assert(configReg->compPolarity < ADS111X_COMP_POL_END);
  assert(configReg->compLatch < ADS111X_COMP_LAT_END);
  assert(configReg->compQueue < ADS111X_COMP_QUE_END);
  /* Check the connection and do it*/
  if(IsDeviceConnected())
  {
    /* Local variables */
    uint16 data = 0;
    Std_ReturnType ret = E_NOT_OK;
    insert_CONFIG(&data, configReg);

    ret = Com_ADS111X_Write_ConfigRegister(data);
    if( ret == E_OK)
    {
      g_ADS111X_ConfigReg = data;
    }
    return ret;
  }
  return E_NOT_AVAILABLE;
}
Std_ReturnType ADS111X_Get_ConfigRegister(uint16* configReg)
{
  if(IsDeviceConnected())
  {
    return Com_ADS111X_Read_ConfigRegister(configReg);
  }
  return E_NOT_AVAILABLE;
}
Std_ReturnType ADS111X_Get_ConversionRegister(uint16* data)
{
  /* Check the connection and do it*/
  if(IsDeviceConnected())
  {
    return Com_ADS111X_Read_ConversionRegister(data);
  }
  return E_NOT_AVAILABLE;
}
Std_ReturnType ADS111X_StartConversion(void)
{
  /* Check the connection and do it*/
  if(IsDeviceConnected())
  {
    uint16 tx_Connfig = 0;
		tx_Connfig = g_ADS111X_ConfigReg;
    insertBits(&tx_Connfig, ADS111X_STAT_CONVERSION_START, OS_LEN, OS_POS);
    return Com_ADS111X_Write_ConfigRegister(tx_Connfig);
  }
  return E_NOT_AVAILABLE;
}

Std_ReturnType ADS111X_IsConversionFinished(boolean* stat)
{
  /* Check the connection and do it*/
  if(IsDeviceConnected())
  {
    /* Local Variables */
    uint16 rx_Connfig = 0;
    Std_ReturnType ret = E_NOT_OK;
    /* Read the config register*/
    ret = ADS111X_Get_ConfigRegister(&rx_Connfig);
    if(ret == E_OK)
    {
      *stat = (boolean)(rx_Connfig & 0x0001);
    }
    return ret;
  }
  return E_NOT_AVAILABLE;
}

boolean ADS111X_IsConnected(void)
{
  return IsDeviceConnected();
}

void ADS111X_Init(void)
{
  /* Set the initial config */
  Std_ReturnType ret = E_OK;
  ADS111X_ConfigRegType configReg = {0};
  configReg.stat = ADS111X_STAT_CONVERSION_BUSY;
  configReg.mux = ADS111X_MUX_AIN0_GND;
  configReg.pga = ADS111X_PGA_0V256;
  configReg.mode = ADS111X_MODE_SINGLE_CONV;
  configReg.dataRate = ADS111X_DATA_RATE_64SPS;
  configReg.compMode = ADS111X_COMP_MODE_TRADITIONAL;
  configReg.compPolarity = ADS111X_COMP_POL_ACTIVE_LOW;
  configReg.compLatch = ADS111X_COMP_LAT_NON_LATCHING;
  configReg.compQueue =ADS111X_COMP_QUE_DISABLE;
  ret = ADS111X_Set_ConfigRegister(&configReg);
  assert(ret == E_OK);
}
