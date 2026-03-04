#ifndef _APP_INTERNAL_H_
#define _APP_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************
                            Includes
*************************************************************************/
#include "App_Lcfg.h"
/*************************************************************************
                            Macro definition
*************************************************************************/
#define GetLastState()          (g_App_pinStates.lastState)
#define GetCurrentState()       (g_App_pinStates.currentState)
#define IsStateChangeHappend()  ((GetCurrentState() == PinState_High) && (GetLastState() == PinState_Low))
#define IsStateChanged()        (g_App_pinStates.changed == STD_HIGH)
/*************************************************************************
                            Type definition
*************************************************************************/

/*************************************************************************
                            Variables
*************************************************************************/

/*************************************************************************
                            Functions
*************************************************************************/


#ifdef __cplusplus
}
#endif

#endif
