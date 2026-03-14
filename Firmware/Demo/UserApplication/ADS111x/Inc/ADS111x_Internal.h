#ifndef _ADS111X_INTERNAL_H_
#define _ADS111X_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************
                                Includes
*************************************************************************/
#include "ADS111x_Types.h"
/*************************************************************************
                                Macro definition
*************************************************************************/
#define REGISTER_ADDRESS_POINTER_LEN  2u /*!< The length of the register address pointer. */
#define REGISTER_ADDRESS_POINTER_POS  0u /*!< The position of the register address pointer. */
#define OS_LEN                        1u /*!< The length of the operational status or single-shot conversion start. */
#define OS_POS                        15u /*!< The position of the OS register. */
#define MUX_LEN                       3u /*!< The length of the MUX register. */
#define MUX_POS                       12u /*!< The position of the MUX registers in config register. */
#define PGA_LEN                       3u /*!< The length of the PGA register. */
#define PGA_POS                       9u /*!< The position of the PGA registers in config register. */
#define MODE_LEN                      1u /*!< The length of the MODE register. */
#define MODE_POS                      8u /*!< The position of the MODE register in config register. */
#define DR_LEN                        3u /*!< The length of the data rate register. */
#define DR_POS                        5u /*!< The position of the data rate register in config register. */
#define COMP_MODE_LEN                 1u /*!< The length of the comparator mode register. */
#define COMP_MODE_POS                 4u /*!< The position of the comparator mode register in config register. */
#define COMP_POL_LEN                  1u /*!< The length of the comparator polarity register. */
#define COMP_POL_POS                  3u /*!< The position of the comparator polarity register in config register. */
#define COMP_LAT_LEN                  1u /*!< The length of the latching comparator register. */
#define COMP_LAT_POS                  2u /*!< The position of the latching comparator register in config register. */
#define COMP_QUE_LEN                  2u /*!< The length of the comprator queue disable register. */
#define COMP_QUE_POS                  0u /*!< The position of the comprator queue disable register in config register. */
/*************************************************************************
                                Type definition
*************************************************************************/

/*************************************************************************
                                Variables
*************************************************************************/

/*************************************************************************
                                Functions
*************************************************************************/
void insert_CONFIG(uint16* inData, ADS111X_ConfigRegType* configReg);



#ifdef __cplusplus
}
#endif

#endif
