#ifndef _MCAL_LCFG_H_
#define _MCAL_LCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
                          Includes
*************************************************************************/
#include "Std_Types.h"
#include "stm32h5xx.h"
#include "stm32h5xx_hal.h"
/*************************************************************************
                          Macro definition
*************************************************************************/
#define IN_BUTTON_Pin GPIO_PIN_13
#define IN_BUTTON_GPIO_Port GPIOC
#define OUT_LED_Pin GPIO_PIN_5
#define OUT_LED_GPIO_Port GPIOA

#define TIM_PWM_CHANNELS    2u
#define ADC_CHANNELS        1u
#define ADC_CONVERSION_TIMEOUT  (1000u)
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
/*************************************************************************
                          Variables
*************************************************************************/
extern TIM_HandleTypeDef htim1;
extern ADC_HandleTypeDef hadc1;

extern const TIM_PWM_ChannelMappingType g_TIM_PWM_ChannelMapping[TIM_PWM_CHANNELS];
extern const ADC_ChannelMappingType g_AinConfig[ADC_CHANNELS];

/* Timer Initialization */
void Mcal_TIM_Init(void);
/* ADC Initialization */
void Mcal_ADC_Init(void);

#ifdef __cplusplus
}
#endif

#endif
