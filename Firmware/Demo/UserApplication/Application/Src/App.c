/*************************************************************************
                            Includes
*************************************************************************/
#include "Std_Types.h"
#include "App_Lcfg.h"
#include "App_Internal.h"
#include "Mcal.h"
/*************************************************************************
                            Macro definition
*************************************************************************/
#define MAX_PULSE_WIDTH   330u
#define CYCLE_TIME        330u
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
};

uint32 g_App_PulseWidth = 180;
/*************************************************************************
                            Functions
*************************************************************************/
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

void Application_MainFunction(void)
{
  __blinking();
  __PWM();
}
/* Initialization */
void Application_Init(void)
{
  RESET_PIN(LED_PORT, LED_PIN);
  Mcal_Tim_PWM_Reconfig(LED1_CHANNEL_IDX,g_App_PulseWidth,CYCLE_TIME);
  Mcal_Tim_PWM_Reconfig(LED2_CHANNEL_IDX,g_App_PulseWidth,CYCLE_TIME);
  Mcal_Tim_PWM_Enable(LED1_CHANNEL_IDX);
  Mcal_Tim_PWM_Enable(LED2_CHANNEL_IDX);
}
