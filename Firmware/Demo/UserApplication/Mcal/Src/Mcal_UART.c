/*************************************************************************
                          Includes
*************************************************************************/
#include "Std_Types.h"
#include "Mcal_Lcfg.h"
#include "ComStack_Types.h"
#include "stm32h5xx.h"
#include "stm32h5xx_hal.h"
/*************************************************************************
                          Macro definition
*************************************************************************/

/*************************************************************************
                          Type definition
*************************************************************************/

/*************************************************************************
                          Variables
*************************************************************************/
uint8 g_UART_RxBuff[UART_RX_BUFF_SIZE];
uint8 g_UART_TxBuff[UART_TX_BUFF_SIZE];
PduInfoType g_UART_RxPduInfo = {0, g_UART_RxBuff};
PduInfoType g_UART_TxPduInfo = {0, g_UART_TxBuff};
boolean g_txReady = TRUE;
/*************************************************************************
                            Functions
*************************************************************************/
static inline void __switchToRxMode(uint16 pduId)
{
  if(g_UART_RsProtocolPin[pduId].isPinReq)
  {
    RESET_PIN(g_UART_RsProtocolPin[pduId].de_Port,
      g_UART_RsProtocolPin[pduId].de_Pin);
    RESET_PIN(g_UART_RsProtocolPin[pduId].re_Port,
      g_UART_RsProtocolPin[pduId].re_Pin);
  }
}
static inline void __switchToTxMode(uint16 pduId)
{
  if(g_UART_RsProtocolPin[pduId].isPinReq)
  {
    SET_PIN(g_UART_RsProtocolPin[pduId].de_Port,
      g_UART_RsProtocolPin[pduId].de_Pin);
    SET_PIN(g_UART_RsProtocolPin[pduId].re_Port,
      g_UART_RsProtocolPin[pduId].re_Pin);
  }
}
void Mcal_UART_Rx_ISR(UART_HandleTypeDef* huart, uint16 dataSize)
{
  /* This function is only for the Slave port that is handled by the DMA */
  uint8 i = 0;
  for(i = 0; i < UART_RX_PDU_COUNTS; i++)
  {
    if(g_UART_RxPduMapping[i].huart == huart)
    {
      g_UART_RxPduInfo.SduLength = dataSize;
      g_UART_RxPduMapping[i].funcPtr(i, (const PduInfoType*)(&g_UART_RxPduInfo));
    }
  }
  /* Only the 0 index is receiving through the DMA. */
  HAL_UARTEx_ReceiveToIdle_DMA(g_UART_RxPduMapping[0].huart,
    g_UART_RxPduInfo.SduDataPtr, UART_RX_BUFF_SIZE);
  __HAL_DMA_DISABLE_IT(&handle_GPDMA1_Channel0, DMA_IT_HT); // TODO: make this configurable
}
void Mcal_UART_Tx_ISR(UART_HandleTypeDef* huart)
{
  /* Switch to Rx mode for the slave UART */
  if(g_UART_TxPduMapping[0].huart == huart)
  {
    __switchToRxMode(0);
  }
}
Std_ReturnType Mcal_UART_TxData(PduIdType txPduId, const PduInfoType* pduInfo)
{
  Std_ReturnType ret;
  /* Check the pduId */
  if(txPduId >= UART_TX_PDU_COUNTS)
  {
    return E_NOT_OK;
  }
  /* switch to Tx mode */
  __switchToTxMode(txPduId);
  /* transmit data*/
  ret =  (Std_ReturnType)HAL_UART_Transmit(g_UART_TxPduMapping[txPduId].huart,
                          pduInfo->SduDataPtr, pduInfo->SduLength,
                          UART_TX_MAX_TIMEOUT);
  /* switch to Rx mode*/
  __switchToRxMode(txPduId);
  /* return */
  return ret;        
}
Std_ReturnType Mcal_UART_TxData_DMA(PduIdType txPduId,
                const PduInfoType* pduInfo)
{
  /* copy data to Mcal buffers */
  g_UART_TxPduInfo.SduLength = pduInfo->SduLength;
  memcpy(g_UART_TxPduInfo.SduDataPtr, pduInfo->SduDataPtr, pduInfo->SduLength);
  /* switch to Tx mode */
  __switchToTxMode(txPduId);
  /* transmit data */
  return HAL_UART_Transmit_DMA(g_UART_TxPduMapping[txPduId].huart,
          g_UART_TxPduInfo.SduDataPtr, g_UART_TxPduInfo.SduLength);
}
Std_ReturnType Mcal_UART_TxReqRxResp(PduIdType txPduId,
                const PduInfoType* txPduInfo, BufferType* rxBuffer)
{
  /* NOTE:: The SduLength in the rxPduInfo should be equal to the maximum size of the tx data. */
  Std_ReturnType ret = E_NOT_OK;
  /* Switch to Tx mode */
  __switchToTxMode(txPduId);
  /* Send request */
  while(g_UART_TxPduMapping[txPduId].huart->gState != HAL_UART_STATE_READY);
  ret = (Std_ReturnType)HAL_UART_Transmit(g_UART_TxPduMapping[txPduId].huart,
          txPduInfo->SduDataPtr, txPduInfo->SduLength, UART_TX_MAX_TIMEOUT);
  if(ret != E_OK)
  {
    return ret;
  }
  /* Change to Rx mode */
  __switchToRxMode(txPduId);
  /* Receive response */
  ret = (Std_ReturnType)HAL_UARTEx_ReceiveToIdle(g_UART_TxPduMapping[txPduId].huart,
        rxBuffer->bufferData.SduDataPtr, rxBuffer->bufferSizeType,
        &rxBuffer->bufferData.SduLength, UART_TX_MAX_TIMEOUT);
  /* Return */
  __switchToTxMode(txPduId);
  return ret;
}
void Mcal_UART_Init(void)
{
  HAL_UARTEx_ReceiveToIdle_DMA(g_UART_RxPduMapping[0].huart,
    g_UART_RxPduInfo.SduDataPtr, UART_RX_BUFF_SIZE);
  __HAL_DMA_DISABLE_IT(&handle_GPDMA1_Channel1, DMA_IT_HT); // TODO: make this configurable
}
