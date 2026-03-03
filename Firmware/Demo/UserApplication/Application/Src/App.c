/*************************************************************************
                            Includes
*************************************************************************/
#include "Std_Types.h"
#include "App_Lcfg.h"
#include "App_Internal.h"
/*************************************************************************
                            Macro definition
*************************************************************************/

/*************************************************************************
                            Type definition
*************************************************************************/
typedef struct
{
  PinState_t currentState;
  PinState_t lastState;
}App_PinStateType;
/*************************************************************************
                            Variables
*************************************************************************/
App_PinStateType g_App_pinStates =
{
  PinState_Low, /* currentState */
  PinState_Low, /* lastState */
};
/*************************************************************************
                            Functions
*************************************************************************/

void Application_MainFunction(void)
{
  g_App_pinStates.lastState = g_App_pinStates.currentState;
  g_App_pinStates.currentState = (PinState_t)GET_PIN(IN_PORT, IN_PIN);
  
  if((GetCurrentState() == PinState_High) && (GetLastState() == PinState_Low))
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
/* Initialization */
void Application_Init(void)
{
  RESET_PIN(LED_PORT, LED_PIN);
}
