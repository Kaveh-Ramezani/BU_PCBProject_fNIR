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
  STD_LOW,      /* changed */
};

uint32 g_App_PulseWidth = 0;
uint16 g_App_data_LED1 = 0;
uint16 g_App_data_LED2 = 0;
float32 g_LED1 = 0;
float32 g_LED2 = 0;
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

static Std_ReturnType _ADC_readData(uint32 adcChannelIdx, uint16 *retData)
{
  Std_ReturnType ret = E_OK;
  *retData = Mcal_ADC_GetValue(adcChannelIdx, &ret);
  if(ret != E_OK) *retData = 0;
  return ret;
}

static inline void __ADC(void)
{
  Std_ReturnType ret = E_NOT_OK;
  Mcal_Tim_PWM_Config_pulseWidth(LED1_CHANNEL_IDX, CYCLE_TIME);
  DELAY_MS(3); // wait for DAC
  ret = _ADC_readData(LED1_ADC_CHANNEL_IDX, &g_App_data_LED1);
  Mcal_Tim_PWM_Config_pulseWidth(LED1_CHANNEL_IDX, 0u);
  
  DELAY_MS(90); // wait for DAC

  Mcal_Tim_PWM_Config_pulseWidth(LED2_CHANNEL_IDX, CYCLE_TIME);
  DELAY_MS(3); // wait for DAC
  ret = _ADC_readData(LED1_ADC_CHANNEL_IDX, &g_App_data_LED2);
  Mcal_Tim_PWM_Config_pulseWidth(LED2_CHANNEL_IDX, 0u);
	g_LED1 = (((float32)g_App_data_LED1)/4095.0)*3.3;
	g_LED2 = (((float32)g_App_data_LED2)/4095.0)*3.3;
}

void Application_MainFunction(void)
{
  __blinking();
  // __PWM();
  __ADC();
}
/* Initialization */
void Application_Init(void)
{
  RESET_PIN(LED_PORT, LED_PIN);
  Mcal_Tim_PWM_Reconfig(LED1_CHANNEL_IDX, 0u, CYCLE_TIME);
  Mcal_Tim_PWM_Reconfig(LED2_CHANNEL_IDX, 0u, CYCLE_TIME);
  Mcal_Tim_PWM_Enable(LED1_CHANNEL_IDX);
  Mcal_Tim_PWM_Enable(LED2_CHANNEL_IDX);
}
