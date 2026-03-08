/*************************************************************************
                            Includes
*************************************************************************/
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "App_Lcfg.h"
#include "App_Internal.h"
#include "Mcal.h"
/*************************************************************************
                            Macro definition
*************************************************************************/
#define MAX_PULSE_WIDTH   330u
#define CYCLE_TIME        330u
#define Get_App_TxBuffPduInfoPtr()      (&g_App_TxBuffer.bufferData)
#define Get_App_TxBuffSduPtr()          (g_App_TxBuffer.bufferData.SduDataPtr)
#define Get_App_TxBuffSduLen()          (g_App_TxBuffer.bufferData.SduLength)
#define Get_App_TxBuffStatus()          (g_App_TxBuffer.bufferStatus)
#define Get_App_TxBuffSize()            (g_App_TxBuffer.bufferSizeType)
#define Set_App_TxBuffSduLen(__LEN__)   (g_App_TxBuffer.bufferData.SduLength = __LEN__)
/*************************************************************************
                            Type definition
*************************************************************************/
typedef struct
{
  PinState_t  currentState;
  PinState_t  lastState;
  uint8       changed;
}App_PinStateType;
/*************************************************************************
                            Variables
*************************************************************************/
App_PinStateType g_App_pinStates =
{
  PinState_Low, /* currentState */
  PinState_Low, /* lastState */
  STD_LOW,      /* changed */
};

static uint32 g_App_PulseWidth = 0;
uint16 g_App_data_LED1 = 0;
uint16 g_App_data_LED2 = 0;
static float32 g_LED1 = 0;
static float32 g_LED2 = 0;

static uint8 g_App_TxBuff[MB_TX_BUFF_SIZE];
static BufferType g_App_TxBuffer;
static const PduIdType g_RxPduId = UART_PDU_ID;
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
/* Changing PWM data */
static inline void __PWM(void)
{
  if(IsStateChanged())
  {
    g_App_PulseWidth += 10u;

    if(g_App_PulseWidth == MAX_PULSE_WIDTH)
    {
      g_App_PulseWidth = 0;
    }
    Mcal_Tim_PWM_Config_pulseWidth(LED1_CHANNEL_IDX,g_App_PulseWidth);
    Mcal_Tim_PWM_Config_pulseWidth(LED2_CHANNEL_IDX,(CYCLE_TIME - g_App_PulseWidth));
  }
}
/* ADC */
static Std_ReturnType _ADC_readData(uint32 adcChannelIdx, uint16 *retData)
{
  Std_ReturnType ret = E_OK;
  *retData = Mcal_ADC_GetValue(adcChannelIdx, &ret);
  if(ret != E_OK) *retData = 0;
  return ret;
}
static void _ledSensing(uint32 ledChannelIdx, uint32 adcChannelIdx, uint16 *data)
{
  Std_ReturnType ret = E_NOT_OK;
  Mcal_Tim_PWM_Config_pulseWidth(ledChannelIdx, CYCLE_TIME); // Configure channel
  DELAY_MS(DAC_SETTLE_TIME_MS); // wait for DAC
  ret = _ADC_readData(adcChannelIdx, data); // Read from opto
  Mcal_Tim_PWM_Config_pulseWidth(ledChannelIdx, 0u); // Disable channel
}
static inline void __ADC(void)
{
  
  /* wait between each sample */
  DELAY_MS(EACH_MEAS_INTERVAL_MS);
  /* The first LED */
  _ledSensing(LED1_CHANNEL_IDX, LED1_ADC_CHANNEL_IDX, &g_App_data_LED1);
  /* The second LED */
  _ledSensing(LED2_CHANNEL_IDX, LED2_ADC_CHANNEL_IDX, &g_App_data_LED2);
  /* Convert to Raw data voltage for monitoring */
	g_LED1 = ADCRawToVoltage(g_App_data_LED1);
	g_LED2 = ADCRawToVoltage(g_App_data_LED2);
}
/* Send to PC*/
static inline void __sendDataToPC(void)
{
  Std_ReturnType ret = E_NOT_OK;
  Set_App_TxBuffSduLen(8); 
	Get_App_TxBuffSduPtr()[0] = 0x55;
	Get_App_TxBuffSduPtr()[1] = 0x55;
  memcpy(&(Get_App_TxBuffSduPtr()[2]), &g_App_data_LED1, sizeof(g_App_data_LED1));
  memcpy(&(Get_App_TxBuffSduPtr()[4]), &g_App_data_LED2, sizeof(g_App_data_LED2));
	Get_App_TxBuffSduPtr()[6] = 0x0D;
	Get_App_TxBuffSduPtr()[7] = 0x0A;
  ret =  Mcal_UART_TxData_DMA(g_RxPduId, (const PduInfoType*)(Get_App_TxBuffPduInfoPtr()));
}
/* Receive From PC */
void Application_RxIndication(PduIdType pduId, const PduInfoType* pduInfo)
{
  
}
/* Main Function */
void Application_MainFunction(void)
{
  __blinking();
  // __PWM();
  __ADC();
  __sendDataToPC();
}
/* Initialization */
void Application_Init(void)
{
  RESET_PIN(LED_PORT, LED_PIN);
  Mcal_Tim_PWM_Reconfig(LED1_CHANNEL_IDX, 0u, CYCLE_TIME);
  Mcal_Tim_PWM_Reconfig(LED2_CHANNEL_IDX, 0u, CYCLE_TIME);
  Mcal_Tim_PWM_Enable(LED1_CHANNEL_IDX);
  Mcal_Tim_PWM_Enable(LED2_CHANNEL_IDX);

  /* Tx buffer initialization */
  g_App_TxBuffer.bufferData.SduDataPtr = &g_App_TxBuff[0];
  g_App_TxBuffer.bufferData.SduLength = 0;
  g_App_TxBuffer.bufferStatus = BUFF_NOT_IN_USE;
  g_App_TxBuffer.bufferSizeType = sizeof(g_App_TxBuff);
}
