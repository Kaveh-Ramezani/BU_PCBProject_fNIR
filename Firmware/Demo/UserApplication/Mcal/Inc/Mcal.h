#ifndef _MCAL_H_
#define _MCAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
                        Includes
*************************************************************************/
#include "Std_Types.h"
#include "ComStack_Types.h"
/*************************************************************************
                        Macro definition
*************************************************************************/
#define MCU_ENDIAN_TYPE MCU_LITTLE_ENDIAN
#define MCU_BIG_ENDIAN  0x00
#define MCU_LITTLE_ENDIAN 0x01

/*************************************************************************
                        Type definition
*************************************************************************/

/*************************************************************************
                        Variables
*************************************************************************/

/*************************************************************************
                        Functions
*************************************************************************/
/******* INTERRUPTS *******/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void GPDMA1_Channel0_IRQHandler(void);
void USART1_IRQHandler(void);
/* Generals */
void Error_Handler(void);
void Mcal_Init(void);
/* Timer */
/* Timer / PWM */
void Mcal_Tim_PWM_Config_pulseWidth(uint32 channelIndex, uint32 pulseWidth_ns);
void Mcal_Tim_PWM_Reconfig(uint32 channelIndex, uint32 pulseWidth_ns, uint32 cycleTime_ns);
void Mcal_Tim_PWM_Disable(uint32 channelIndex);
void Mcal_Tim_PWM_Enable(uint32 channelIndex);
/* ADC */
uint16 Mcal_ADC_GetValue(uint32 index, Std_ReturnType* ret);
/* UART */
Std_ReturnType Mcal_UART_TxData_DMA(PduIdType txPduId, const PduInfoType* pduInfo);
Std_ReturnType Mcal_UART_TxData(PduIdType txPduId, const PduInfoType* pduInfo);
void Mcal_UART_Rx_ISR(UART_HandleTypeDef* huart, uint16 dataSize);
void Mcal_UART_Tx_ISR(UART_HandleTypeDef* huart);

#ifdef __cplusplus
}
#endif

#endif
