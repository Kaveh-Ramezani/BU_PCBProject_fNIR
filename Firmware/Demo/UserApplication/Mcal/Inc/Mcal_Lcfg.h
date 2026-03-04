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
/*************************************************************************
                          Type definition
*************************************************************************/
typedef struct
{
  TIM_HandleTypeDef*  htim;
  uint32              timerChannel;
}TIM_PWM_ChannelMappingType;
/*************************************************************************
                          Variables
*************************************************************************/
extern TIM_HandleTypeDef htim1;

extern const TIM_PWM_ChannelMappingType g_TIM_PWM_ChannelMapping[TIM_PWM_CHANNELS];


void Mcal_TIM_Init(void);

#ifdef __cplusplus
}
#endif

#endif
