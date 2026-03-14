/*************************************************************************
                            Includes
*************************************************************************/
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "App_Lcfg.h"
#include "App_Internal.h"
#include "Mcal.h"
#include "ADS111x.h"
/*************************************************************************
                            Macro definition
*************************************************************************/
#define MAX_PULSE_WIDTH   330u
#define CYCLE_TIME        330u

#define READ_CONFIG_CYCLE 100u

#define Get_App_TxBuffPtr()             (&g_App_TxBuffer)
#define Get_App_TxBuffPduInfoPtr()      (&g_App_TxBuffer.bufferData)
#define Get_App_TxBuffSduPtr()          (g_App_TxBuffer.bufferData.SduDataPtr)
#define Get_App_TxBuffSduLen()          (g_App_TxBuffer.bufferData.SduLength)
#define Get_App_TxBuffStatus()          (g_App_TxBuffer.bufferStatus)
#define Get_App_TxBuffSize()            (g_App_TxBuffer.bufferSizeType)
#define Set_App_TxBuffSduLen(__LEN__)   (g_App_TxBuffer.bufferData.SduLength = __LEN__)

#define Get_App_RxBuffPtr()             (&g_App_RxBuffer)
#define Get_App_RxBuffPduInfoPtr()      (&g_App_RxBuffer.bufferData)
#define Get_App_RxBuffSduPtr()          (g_App_RxBuffer.bufferData.SduDataPtr)
#define Get_App_RxBuffSduLen()          (g_App_RxBuffer.bufferData.SduLength)
#define Get_App_RxBuffStatus()          (g_App_RxBuffer.bufferStatus)
#define Get_App_RxBuffSize()            (g_App_RxBuffer.bufferSizeType)
#define Set_App_RxBuffSduLen(__LEN__)   (g_App_RxBuffer.bufferData.SduLength = __LEN__)
/*************************************************************************
                            Type definition
*************************************************************************/
typedef struct
{
  PinState_t  currentState;
  PinState_t  lastState;
  uint8       changed;
}App_PinStateType;

typedef struct
{
  uint16 intensity; /* The intensity of the LED in percentage. */
  uint32 adcChannelIdx; /* The ADC channel index */
}App_ChannelConfig;

typedef struct
{
  App_ChannelConfig* LED_ConfigPtr;
}App_Config;
/*************************************************************************
                            Variables
*************************************************************************/
App_PinStateType g_App_pinStates =
{
  PinState_Low, /* currentState */
  PinState_Low, /* lastState */
  STD_LOW,      /* changed */
};

App_ChannelConfig LED_Config[LED_CHANNELS] =
{
  { /* LED1 */
    100,  /* intensity*/
    0u,   /* adcChannelIndex */
  },
  { /* LED2 */
    100,  /* intensity */
    0u,   /* adcChannelIndex */
  },
};

App_Config g_App_configuration =
{
  LED_Config,  /* LED_ConfigPtr */
};

uint16 g_App_ledData[LED_CHANNELS+1] = {0};
Std_ReturnType g_stdRet = E_NOT_OK;

uint8 g_App_TxBuff[MB_TX_BUFF_SIZE];
uint8 g_App_RxBuff[MB_RX_BUFF_SIZE];
BufferType g_App_TxBuffer;
BufferType g_App_RxBuffer;

// static const PduIdType g_RxPduId = UART_PDU_ID;
static const PduIdType g_TxPduId = UART_PDU_ID;

uint8 g_itCnt = 0;
/*************************************************************************
                            Functions
*************************************************************************/
/* Blicking */
static inline void __blinking(void)
{
  g_App_pinStates.lastState = g_App_pinStates.currentState;
  g_App_pinStates.currentState = (PinState_t)GET_PIN(IN_PORT, IN_PIN);
  g_App_pinStates.changed = IsStateChangeHappend() ? STD_HIGH : STD_LOW;

  if(IsStateChanged())
  {
    if(GET_PIN(LED_PORT, LED_PIN) == PinState_High)
    {
      RESET_PIN(LED_PORT, LED_PIN);
    }
    else
    {
      SET_PIN(LED_PORT, LED_PIN);
    }
  }
}
/* ADC */
#if(USE_EXTERNAL_ADC == STD_ON)
boolean _isReadExtADCTriesReachedLimit(void)
{
  g_itCnt++;
  if((g_itCnt == READ_EXT_ADC_TRIES) && (g_stdRet != E_OK))
  {
    return TRUE;
  }
  else if(g_stdRet != E_OK)
  {
    DELAY_MS(READ_EXT_ADC_TRIES_INTERVAL);
  }
  return FALSE;
}
#endif
static Std_ReturnType _ADC_readData(uint32 adcChannelIdx, uint16 *retData)
{
  #if(USE_EXTERNAL_ADC == STD_OFF)
    *retData = Mcal_ADC_GetValue(adcChannelIdx, &g_stdRet);
    return g_stdRet;
  #else
    boolean convStat = FALSE;
    UNUSED(adcChannelIdx);
    /* Ask to start conversion */
    g_stdRet = ADS111X_StartConversion();
    if(g_stdRet != E_OK)
    {
      return g_stdRet;
    }
    /* Check if the conversion has been finished. */
		ResetGlobalIterationCounter();
    do{
      g_stdRet = ADS111X_IsConversionFinished(&convStat);
      if(_isReadExtADCTriesReachedLimit())
      {
        break;
      }
    }while((g_stdRet != E_OK) && (convStat == FALSE));
    if(g_stdRet != E_OK)
    {
      return g_stdRet;
    }
    /* Get the conversion register. */
    ResetGlobalIterationCounter();
    do{
      g_stdRet = ADS111X_Get_ConversionRegister(retData);
      if(_isReadExtADCTriesReachedLimit())
      {
        break;
      }
    }while(g_stdRet != E_OK);
    return g_stdRet;
  #endif
}
static void _ledSensing(uint32 chIdx, App_Config* conf)
{
  Std_ReturnType ret = E_NOT_OK;
  /*
    1. Enable the LED
    2. Wait for predefined DAC settle time
    3. Read ADC
    4. Disable the LED
  */
  uint16 data = 0;
  float32 DC = ((float32)(conf->LED_ConfigPtr[chIdx].intensity)/100.0)*((float32)CYCLE_TIME);
  Mcal_Tim_PWM_Config_pulseWidth(chIdx, DC); // Configure channel
  DELAY_MS(DAC_SETTLE_TIME_MS); // wait for DAC
  // 1 
  ret = _ADC_readData(conf->LED_ConfigPtr[chIdx].adcChannelIdx, &g_App_ledData[chIdx]); // Read ADC
	if(ret != E_OK)
	{
		g_App_ledData[chIdx] = 0;
    Mcal_Tim_PWM_Config_pulseWidth(chIdx, 0u); // Disable channel
    return;
	}
  // 2 
  ret = _ADC_readData(conf->LED_ConfigPtr[chIdx].adcChannelIdx, &data); // Read ADC
	if(ret != E_OK)
	{
		g_App_ledData[chIdx] = 0;
    Mcal_Tim_PWM_Config_pulseWidth(chIdx, 0u); // Disable channel
    return;
	}
  g_App_ledData[chIdx] = ((data + g_App_ledData[chIdx])>>1);
  Mcal_Tim_PWM_Config_pulseWidth(chIdx, 0u); // Disable channel
}
static void _darkSensing(void)
{
  Std_ReturnType ret = E_NOT_OK;
  /* 
  1. Wait for predefined DAC settle time
  2. Read ADC
  */
  DELAY_MS(DAC_SETTLE_TIME_MS); // wait for DAC
  ret = _ADC_readData(0, &g_App_ledData[LED_CHANNELS]); // Read ADC
	if(ret != E_OK)
	{
		g_App_ledData[LED_CHANNELS] = 0;
	}
}
static inline void __sensing(void)
{
  /* Sensing LED */
  for (uint32 idx = 0; idx< LED_CHANNELS; idx++)
  {
    _ledSensing(idx, &g_App_configuration); 
  }
  /* Dark Sensing */
  _darkSensing();
}
/* Send to PC*/
static inline void __prepareSendDataRequest(void)
{
  Set_App_TxBuffSduLen(10); 
	Get_App_TxBuffSduPtr()[0] = 0x55;
	Get_App_TxBuffSduPtr()[1] = 0x55;
  memcpy(&(Get_App_TxBuffSduPtr()[2]), g_App_ledData, sizeof(g_App_ledData));
	Get_App_TxBuffSduPtr()[8] = 0x0D;
	Get_App_TxBuffSduPtr()[9] = 0x0A;
}
static inline void __sendDataToPC(void)
{
  __prepareSendDataRequest();
  Mcal_UART_TxData_DMA(g_TxPduId, (const PduInfoType*)(Get_App_TxBuffPduInfoPtr()));
}
/* Read Configurations from PC */
void __extractReceivedConfiguration(const PduInfoType* pduInfo)
{
  /*
  Byte0: 0xAA                               Byte1: 0xAA
  Byte2: LSB(LED1 Intensity)                Byte3: MSB(LED1 Intensity)
  Byte4: LSB(LED2 Intensity)                Byte5: MSB(LED2 Intensity)
  Byte6: 0x0D                               Byte7: 0X0A
  */
  /* Check the size */
  if(pduInfo->SduLength != 8)
  {
    return;
  }
  /* Check 0xAAs*/
  if(!((pduInfo->SduDataPtr[0] == 0xAA) && (pduInfo->SduDataPtr[1] == 0xAA)))
  {
    return;
  }
  /* Check end of message */
  if(!((pduInfo->SduDataPtr[pduInfo->SduLength-2] == 0x0D) &&
       (pduInfo->SduDataPtr[pduInfo->SduLength-1] == 0x0A)))
  {
    return;
  }
  /* Extract */
  for (uint32 idx = 0; idx < LED_CHANNELS; idx++)
  {
    g_App_configuration.LED_ConfigPtr[idx].intensity = *(uint16*)(&(pduInfo->SduDataPtr[(idx*2) + 2]));
  }
}
void Application_RxIndication(PduIdType pduId, const PduInfoType* pduInfo)
{
  if(pduInfo == NULL)
  {
    return;
  }
  if(pduInfo->SduDataPtr == NULL)
  {
    return;
  }
  if(pduId != g_TxPduId)
  {
    return;
  }
  __extractReceivedConfiguration(pduInfo);
}
/* Main Function */
void Application_MainFunction(void)
{
  __blinking();
	__sensing();
  // __readConfigFromPC();
	__sendDataToPC();
}
/* Initialization */
void Application_Init(void)
{
  /* Turn OFF blinking LED */
  RESET_PIN(LED_PORT, LED_PIN);
  
  /* Initial Configuration of LED channels */
  for(uint32 idx = 0; idx < LED_CHANNELS; idx++)
  {
    Mcal_Tim_PWM_Reconfig(idx, 0u, CYCLE_TIME);
	  Mcal_Tim_PWM_Enable(idx);
  }

  /* Tx buffer initialization */
  g_App_TxBuffer.bufferData.SduDataPtr = &g_App_TxBuff[0];
  g_App_TxBuffer.bufferData.SduLength = 0;
  g_App_TxBuffer.bufferStatus = BUFF_NOT_IN_USE;
  g_App_TxBuffer.bufferSizeType = sizeof(g_App_TxBuff);
  
  /* Rx buffer initialization */
  g_App_RxBuffer.bufferData.SduDataPtr = &g_App_RxBuff[0];
  g_App_RxBuffer.bufferData.SduLength = 0;
  g_App_RxBuffer.bufferStatus = BUFF_NOT_IN_USE;
  g_App_RxBuffer.bufferSizeType = sizeof(g_App_RxBuff);
}
