#ifndef _STD_TYPES_H_
#define _STD_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "stm32h5xx.h"
#include "stm32h5xx_hal.h"

/* Top definitions */
#ifndef STD_ON 
  #define STD_ON    0x01u /*!< Standard ON directive. */
#endif
#ifndef STD_OFF
  #define STD_OFF   0x00u /*!< The standard OFF directive. */
#endif

/* The real directives. */
#define USE_EXTERNAL_ADC STD_ON

/* The micro functions */
#define SET_PIN(GPIO, GPIO_PIN)   (HAL_GPIO_WritePin(GPIO, GPIO_PIN, GPIO_PIN_SET)) /*!< Sets an output pin. */
#define RESET_PIN(GPIO, GPIO_PIN) (HAL_GPIO_WritePin(GPIO, GPIO_PIN, GPIO_PIN_RESET)) /*!< Resets an output pin. */
#define GET_PIN(GPIO, GPIO_PIN)   (HAL_GPIO_ReadPin(GPIO, GPIO_PIN))
#define FREE(PTR)	{\
          if(PTR != NULL)\
          {\
            free(PTR);\
            PTR=NULL;\
          }\
          } /*!< FREEing abstraction. */

#define SECOND_TO_MS(__in)      (((uint32)__in)*1000)	/*!< converts input in seconds to output as miliseconds */
#define MINUTES_TO_MS(__in)     ((uint32)(SECOND_TO_MS(__in*60)*1000))
#define DELAY_MS(_ms_)  (HAL_Delay(_ms_)) /*!< Delay for specific miliseconds. */
#define GET_TICK()      (HAL_GetTick()) /*!< Gets the current tick of the tick timer. */

#ifndef MIN
  #define MIN(_x,_y)  (((_x) < (_y)) ? (_x) : (_y)) /*!< Returns the minimum of the inputs. */
#endif

#ifndef MAX
  #define MAX(_x,_y)  (((_x) > (_y)) ? (_x) : (_y)) /*!< Returns the maximum of the inputs. */
#endif

#ifndef NULL
  #define NULL 0 /*!< The standard NULL directive. */
#endif

#ifndef STD_LOW
  #define STD_LOW     0x00u /*!< The standard LOW directive. */
#endif

#ifndef STD_HIGH
  #define STD_HIGH  0x01u /*!< The standard HIGH directive. */
#endif

#ifndef STD_IDLE
  #define STD_IDLE    0X00u /*!< The standard IDLE directive. */
#endif

#ifndef STD_ACTIVE
  #define STD_ACTIVE  0x01u /*!< The standard ACTIVE directive. */
#endif

#ifndef NULL_PTR
  #define NULL_PTR    ((void *)0) /*!< The genearl null pointer. */
#endif

#ifndef FALSE
  #define FALSE		    (boolean)false /*!< Gerenal false. */
#endif

#ifndef TRUE
  #define TRUE        (boolean)true /*!< General true. */
#endif

/* 
 The below are the types name change. all the types have a better shape that
  the ones ins stdlib. Also, if the platform has been changed, the migration
  would be easy.
 */
typedef bool            boolean;
typedef int8_t          sint8;
typedef uint8_t         uint8;
typedef char            char_t;
typedef int16_t         sint16;
typedef uint16_t        uint16;
typedef int32_t         sint32;
typedef uint32_t        uint32;
typedef int64_t         sint64;
typedef uint64_t        uint64;
typedef uint_least8_t   uint8_least;
typedef uint_least16_t  uint16_least;
typedef uint_least32_t  uint32_least;
typedef int_least8_t    sint8_least;
typedef int_least16_t   sint16_least;
typedef int_least32_t   sint32_least;
typedef float           float32; 
typedef double          float64;  

typedef volatile int8_t   vint8_t;
typedef volatile uint8_t  vuint8_t;

typedef volatile int16_t  vint16_t;
typedef volatile uint16_t vuint16_t;

typedef volatile int32_t  vint32_t;
typedef volatile uint32_t vuint32_t;

typedef volatile int64_t  vint64_t;
typedef volatile uint64_t vuint64_t;

typedef uint8 Std_ReturnType; /*!< The standard return type. */
#define E_OK              0u /*!< OK */
#define E_NOT_OK          1u /*!< It returns not OK*/
#define E_BUSY            2u /*!< The functionality has not done, because the module is busy. */
#define E_TIMEOUT         3u /*!< The functionality has not been done, because of a timeout. */
#define E_NOT_AVAILABLE   4u /*!< The functionality is not available. */
#define E_MODULE_PROBLEM  5u /*!< The module have faced a problem. It can be used for error handling. */

typedef uint8 DataType; /*!< The standatrd datatypes. Some are missing. */
#define DATA_TYPE_UINT8   0x00 /*!< The 8 bits unsigned integer data type. */
#define DATA_TYPE_UINT16  0x01 /*!< The 16 bits unsigned integer data type. */
#define DATA_TYPE_UINT32  0x02 /*!< The 32 bits unsigned integer data type. */
#define DATA_TYPE_UINT64  0x03 /*!< The 64 bits unsigned integer data type. */
#define DATA_TYPE_FLOAT32 0x04 /*!< The 32 bits floating point number data type. */
#define DATA_TYPE_FLOAT64 0x05 /*!< The 64 bits floating point number data type. */
#define DATA_TYPE_SINT8   0x06 /*!< The 8 bits signed integer data type. */
#define DATA_TYPE_SINT16  0x07 /*!< The 16 bits signed integer data type. */
#define DATA_TYPE_SINT32  0x08 /*!< The 32 bits signed integer data type. */
#define DATA_TYPE_SINT64  0x09 /*!< The 64 bits signed integer data type. */
#define DATA_TYPE_END     0x0A /*!< End of the enumeration */

typedef uint8 PinState_t; /*!< State of a pin. */
#define PinState_Low		0x00
#define PinState_High		0x01
#define PinState_END    0x02

/* Global Functions*/
/**
 * The function insertBits inserts a specified number of bits at a specified
 * position in a 16-bit data.
 * 
 * @param outData The `outData` parameter is a pointer to a 16-bit unsigned integer
 * where the bits will be inserted.
 * @param inBits The `inBits` parameter represents the bits that you want to insert
 * into the `outData` at the specified position (`inBitsPos`) with the specified
 * length (`inBitsLen`).
 * @param inBitsLen The `inBitsLen` parameter represents the length of the bits to
 * be inserted into the `outData` variable. It specifies how many bits from the
 * `inBits` parameter should be inserted into the `outData` variable starting from
 * the position specified by `inBitsPos`.
 * @param inBitsPos The `inBitsPos` parameter represents the position at which the
 * input bits (`inBits`) should be inserted into the `outData` variable.
 */
void insertBits(uint16* outData, uint16 inBits, uint32 inBitsLen, uint32 inBitsPos);

#ifdef __cplusplus
}
#endif

#endif
