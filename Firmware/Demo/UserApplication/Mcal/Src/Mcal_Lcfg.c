/*************************************************************************
                          Includes
*************************************************************************/
#include "Std_Types.h"
#include "Mcal_Lcfg.h"
#include "App.h"
/*************************************************************************
                          Macro definition
*************************************************************************/

/*************************************************************************
                          Type definition
*************************************************************************/

/*************************************************************************
                          Variables
*************************************************************************/
TIM_HandleTypeDef htim1;
ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef handle_GPDMA1_Channel0;
DMA_HandleTypeDef handle_GPDMA1_Channel1;

/* The pwm channels configurations */
const TIM_PWM_ChannelMappingType g_TIM_PWM_ChannelMapping[TIM_PWM_CHANNELS] =
{
  /* 0 : LED1 */
  {
    &htim1 /* htim */, 
    TIM_CHANNEL_1 /* timerChannel */,
  },
  /* 1 : LED2 */
  {
    &htim1 /* htim */, 
    TIM_CHANNEL_2 /* timerChannel */,
  },
};

/* The ADC channels configurations */
const ADC_ChannelMappingType g_AinConfig[ADC_CHANNELS] =
{
  /* 0 : ADC1 _ Channel0*/
  {
    &hadc1, /* handler */
    ADC_CHANNEL_0,  /* channelNumber */
  },
};

/* The RSxxx protocols control pins configurations */
const UART_RsProtocolContPinStructType g_UART_RsProtocolPin[UART_COUNTS] =
{
  /* 0 : Main Slave */
  {
    NULL /* re_Port */,
    NULL /* de_Port */,
    NULL /* re_Pin */,
    NULL /* de_Pin */,
    FALSE /* isPinReq */
  },
};

/* The rx pdu mapping configuration */
const UART_RxPduMappingStructType g_UART_RxPduMapping[UART_RX_PDU_COUNTS] = 
{
  /* 0 : Main Slave */
  {
    &huart1 /* huart */,
    Application_RxIndication /* funcPtr */,
    0 /* rxPduId */,
  },
};
/* The tx pdu mapping configuration */
const UART_TxPduMappingStructType g_UART_TxPduMapping[UART_TX_PDU_COUNTS] =
{
  /* 0 : Main Slave */
  {
    &huart1 /* huart */,
  }
};
