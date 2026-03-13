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
#define ADC_BITS  12
#define ADC_RANGE_MAX ((1 << ADC_BITS) - 1)
#define ADC_VDDA      (3.3)

#define EACH_MEAS_INTERVAL_MS (90u)
#define DAC_SETTLE_TIME_MS    (3u)

#define GetLastState()                  (g_App_pinStates.lastState)
#define GetCurrentState()               (g_App_pinStates.currentState)
#define IsStateChangeHappend()          ((GetCurrentState() == PinState_High) && (GetLastState() == PinState_Low))
#define IsStateChanged()                (g_App_pinStates.changed == STD_HIGH)
#define ADCRawToVoltage(__VALUE__)      ((((float32)__VALUE__)/(float32)ADC_RANGE_MAX)*ADC_VDDA)
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
