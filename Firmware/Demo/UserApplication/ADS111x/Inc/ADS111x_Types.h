#ifndef _ADS111X_TYPES_H_
#define _ADS111X_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************
                            Includes
*************************************************************************/
#include "Std_Types.h"
/*************************************************************************
                            Macro definition
*************************************************************************/
#define ADS111X_DEVICE_ADDRESS          (uint16)(0x48<<1) /*!< The device's address. */
/*************************************************************************
                            Type definition
*************************************************************************/
typedef uint8 ADS111X_RegisterAddressType;
#define ADS111X_ADDRESS_CONVERSION_REG  (uint8)(0x00) /*!< The address of the conversion register. */
#define ADS111X_ADDRESS_COFIG_REG       (uint8)(0x01) /*!< The address of the config register. */
#define ADS111X_ADDRESS_LO_THRESH_REG   (uint8)(0x02) /*!< The address of the lower threshold register. */
#define ADS111X_ADDRESS_HI_THRESH_REG   (uint8)(0x03) /*!< The address of the higher threshold register. */
#define ADS111X_ADDRESS_END             (uint8)(0x04)

typedef uint8 ADS111X_StatusType;
#define ADS111X_STAT_CONVERSION_BUSY    0x00
#define ADS111X_STAT_CONVERSION_DONE    0x01
#define ADS111X_STAT_CONVERSION_START   0x01
#define ADS111X_STAT_END              0x02

typedef uint8 ADS111X_MultiplexerConfType;
#define ADS111X_MUX_AIN0_AIN1 0x00
#define ADS111X_MUX_AIN0_AIN3 0x01
#define ADS111X_MUX_AIN1_AIN3 0x02
#define ADS111X_MUX_AIN2_AIN3 0x03
#define ADS111X_MUX_AIN0_GND  0x04
#define ADS111X_MUX_AIN1_GND  0x05
#define ADS111X_MUX_AIN2_GND  0x06
#define ADS111X_MUX_AIN3_GND  0x07
#define ADS111X_MUX_END       0x08

typedef uint8 ADS111X_PGAConfType;
#define ADS111X_PGA_6V144     0x00
#define ADS111X_PGA_4V096     0x01
#define ADS111X_PGA_2V048     0x02
#define ADS111X_PGA_1V024     0x03
#define ADS111X_PGA_0V512     0x04
#define ADS111X_PGA_0V256     0x05
#define ADS111X_PGA_END       0x08

typedef uint8 ADS111X_ModeConfType;
#define ADS111X_MODE_CONTINUOUS_CONV  0x00
#define ADS111X_MODE_SINGLE_CONV      0x01
#define ADS111X_MODE_END              0x02

typedef uint8 ADS111X_DataRateType;
#define ADS111X_DATA_RATE_8SPS    0x00
#define ADS111X_DATA_RATE_16SPS   0x01
#define ADS111X_DATA_RATE_32SPS   0x02
#define ADS111X_DATA_RATE_64SPS   0x03
#define ADS111X_DATA_RATE_128SPS  0x04
#define ADS111X_DATA_RATE_250SPS  0x05
#define ADS111X_DATA_RATE_475SPS  0x06
#define ADS111X_DATA_RATE_860SPS  0x07
#define ADS111X_DATA_RATE_END     0x08

typedef uint8 ADS111X_ComparatorModeType;
#define ADS111X_COMP_MODE_TRADITIONAL 0x00
#define ADS111X_COMP_MODE_WINDOW      0x01
#define ADS111X_COMP_MODE_END         0x02

typedef uint8 ADS111X_ComparatorPolarityType;
#define ADS111X_COMP_POL_ACTIVE_LOW   0x00
#define ADS111X_COMP_POL_ACTIVE_HIGH  0x01
#define ADS111X_COMP_POL_END          0x02

typedef uint8 ADS111X_LatchingComparatorType;
#define ADS111X_COMP_LAT_NON_LATCHING 0x00
#define ADS111X_COMP_LAT_LATCHING     0x01
#define ADS111X_COMP_LAT_END          0x02

typedef uint8 ADS111X_ComparatorQueueType;
#define ADS111X_COMP_QUE_1_CONV   0x00
#define ADS111X_COMP_QUE_2_CONV   0x01
#define ADS111X_COMP_QUE_4_CONV   0x02
#define ADS111X_COMP_QUE_DISABLE  0x03
#define ADS111X_COMP_QUE_END      0x04

typedef struct
{
  ADS111X_StatusType stat;
  ADS111X_MultiplexerConfType mux;
  ADS111X_PGAConfType pga;
  ADS111X_ModeConfType mode;
  ADS111X_DataRateType dataRate;
  ADS111X_ComparatorModeType compMode;
  ADS111X_ComparatorPolarityType compPolarity;
  ADS111X_LatchingComparatorType compLatch;
  ADS111X_ComparatorQueueType compQueue;
}ADS111X_ConfigRegType;

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
