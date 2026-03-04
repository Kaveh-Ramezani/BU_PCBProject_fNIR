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
#define MASTER_TIM_PERIOD_NS  (10u)
/*************************************************************************
                            Type definition
*************************************************************************/

/*************************************************************************
                            Variables
*************************************************************************/

/*************************************************************************
                            Functions
*************************************************************************/
static inline void __setPulseWidth(uint32 channelIndex, uint32 pulseWidth_ns)
{
	__HAL_TIM_SET_COMPARE(g_TIM_PWM_ChannelMapping[channelIndex].htim,
				      g_TIM_PWM_ChannelMapping[channelIndex].timerChannel, pulseWidth_ns);
}

static inline void __setCycleTime_ns(uint32 channelIndex, uint32 cycleTime_ns)
{
	__HAL_TIM_SetAutoreload(g_TIM_PWM_ChannelMapping[channelIndex].htim,
                          (cycleTime_ns/MASTER_TIM_PERIOD_NS)-1);
}

void Mcal_Tim_PWM_Config_pulseWidth(uint32 channelIndex, uint32 pulseWidth_ns)
{
	pulseWidth_ns = pulseWidth_ns/MASTER_TIM_PERIOD_NS;
  pulseWidth_ns = (pulseWidth_ns == 0) ?  1 : pulseWidth_ns;
  __setPulseWidth(channelIndex, pulseWidth_ns);
}

void Mcal_Tim_PWM_Reconfig(uint32 channelIndex, uint32 pulseWidth_ns, uint32 cycleTime_ns)
{
  /* The master is generating a clock with 10ms */
  assert(channelIndex < TIM_PWM_CHANNELS);

  HAL_TIM_PWM_Stop(g_TIM_PWM_ChannelMapping[channelIndex].htim,
    g_TIM_PWM_ChannelMapping[channelIndex].timerChannel);
	__setCycleTime_ns(channelIndex, cycleTime_ns);
  Mcal_Tim_PWM_Config_pulseWidth(channelIndex, pulseWidth_ns);
}

void Mcal_Tim_PWM_Enable(uint32 channelIndex)
{
  assert(channelIndex < TIM_PWM_CHANNELS);
  HAL_TIM_PWM_Start(g_TIM_PWM_ChannelMapping[channelIndex].htim,
      g_TIM_PWM_ChannelMapping[channelIndex].timerChannel);
}

void Mcal_Tim_PWM_Disable(uint32 channelIndex)
{
  assert(channelIndex < TIM_PWM_CHANNELS);

  if(g_TIM_PWM_ChannelMapping[channelIndex].htim->Instance->CR1 & TIM_CR1_CEN)
  {
    /* If it was enabled */
    HAL_TIM_PWM_Stop(g_TIM_PWM_ChannelMapping[channelIndex].htim,
      g_TIM_PWM_ChannelMapping[channelIndex].timerChannel);
  }
}

void Mcal_TIM_Init(void)
{

}
