#ifndef _MCAL_LCFG_H_
#define _MCAL_LCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
                          Includes
*************************************************************************/
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "stm32h5xx.h"
#include "stm32h5xx_hal.h"
/*************************************************************************
                          Macro definition
*************************************************************************/
#define IN_BUTTON_Pin GPIO_PIN_13
#define IN_BUTTON_GPIO_Port GPIOC
#define OUT_LED_Pin GPIO_PIN_5
#define OUT_LED_GPIO_Port GPIOA
/* PWM */
#define TIM_PWM_CHANNELS        (2u)
/* ADC */
#define ADC_CHANNELS            (1u)
#define ADC_CONVERSION_TIMEOUT  (1000u)
/* UART */
#define UART_COUNTS             (1u)
#define UART_RX_BUFF_SIZE       (128u)
#define UART_TX_BUFF_SIZE       (128u)
#define UART_RX_PDU_COUNTS      (1u)
#define UART_TX_PDU_COUNTS      (1u)
#define UART_TX_MAX_TIMEOUT     (50u)
/*************************************************************************
                          Type definition
*************************************************************************/
typedef struct
{
  TIM_HandleTypeDef*  htim;
  uint32              timerChannel;
}TIM_PWM_ChannelMappingType;

typedef struct
{
  ADC_HandleTypeDef*  handler;
  uint32              channelNumber;
}ADC_ChannelMappingType;

typedef struct
{
  GPIO_TypeDef*       re_Port;  /* re port */
  GPIO_TypeDef*       de_Port;  /* de port */
  uint16              re_Pin;   /* re pin number */
  uint16              de_Pin;   /* de pin number */
  boolean             isPinReq; /* Indicates whether changing control pin state is required or not. */
}UART_RsProtocolContPinStructType;

typedef void(*ReceptionFunctionPtr)(PduIdType rxPduId, const PduInfoType* pduInfo);
typedef struct
{
  UART_HandleTypeDef*   huart;
  ReceptionFunctionPtr  funcPtr;
  PduIdType             rxPduId;
}UART_RxPduMappingStructType;

typedef struct
{
  UART_HandleTypeDef* huart;
}UART_TxPduMappingStructType;
/*************************************************************************
                          Variables
*************************************************************************/
extern TIM_HandleTypeDef htim1;
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef handle_GPDMA1_Channel0;

extern const TIM_PWM_ChannelMappingType g_TIM_PWM_ChannelMapping[TIM_PWM_CHANNELS];
extern const ADC_ChannelMappingType g_AinConfig[ADC_CHANNELS];

extern const UART_RsProtocolContPinStructType g_UART_RsProtocolPin[UART_COUNTS];
extern const UART_RxPduMappingStructType g_UART_RxPduMapping[UART_RX_PDU_COUNTS];
extern const UART_TxPduMappingStructType g_UART_TxPduMapping[UART_TX_PDU_COUNTS];

/* Timer Initialization */
void Mcal_TIM_Init(void);
/* ADC Initialization */
void Mcal_ADC_Init(void);
/* UART Initialization */
void Mcal_UART_Init(void);
/* DMA Initialization */


#ifdef __cplusplus
}
#endif

#endif
