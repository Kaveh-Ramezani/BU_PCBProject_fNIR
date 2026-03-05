/*************************************************************************
                          Includes
*************************************************************************/
#include "Std_Types.h"
#include "Mcal_Lcfg.h"
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
