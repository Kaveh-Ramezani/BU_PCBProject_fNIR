
/*************************************************************************
                                Includes
*************************************************************************/
#include "ADS111x_Internal.h"
/*************************************************************************
                                Macro definition
*************************************************************************/

/*************************************************************************
                                Type definition
*************************************************************************/

/*************************************************************************
                                Variables
*************************************************************************/

/*************************************************************************
                                Functions
*************************************************************************/
/* Generally used functions */
/**
 * The function __extractBits extracts a specified number of bits from a given
 * position in a 16-bit input data.
 * 
 * @param inData The `inData` parameter is a 16-bit unsigned integer from which we
 * want to extract a specific range of bits.
 * @param inBitsLen The `inBitsLen` parameter represents the length of the bits
 * that need to be extracted from the input data (`inData`). It should be less than
 * 16 bits.
 * @param inBitsPos The `inBitsPos` parameter represents the starting position of
 * the bits that you want to extract from the `inData` input. It indicates the
 * position of the least significant bit (LSB) of the desired bits within the
 * `inData` value.
 * 
 * @return The function `__extractBits` takes an input data `inData`, extracts a
 * specified number of bits `inBitsLen` starting from a specified position
 * `inBitsPos`, and returns the extracted bits.
 */
/*
static uint16 __extractBits(uint16 inData, uint32 inBitsLen, uint32 inBitsPos)
{
  assert(inBitsLen<16);
  assert(inBitsPos<16);
  assert((inBitsLen + inBitsPos) <= 16);

  uint16 mask = ~(0xFFFF << inBitsLen);
  return ((inData >> inBitsPos) & mask);
}
*/
/**********************************************************/
static inline void __insert_CONFIG_STAT( uint16* inData,
                                        ADS111X_StatusType stat)
{
  insertBits(inData, (uint16)stat, OS_LEN, OS_POS);
}
static inline void __insert_CONFIG_MUX( uint16* inData,
                                        ADS111X_MultiplexerConfType mux)
{
  insertBits(inData, (uint16)mux, MUX_LEN, MUX_POS);
}
static inline void __insert_CONFIG_PGA( uint16* inData,
                                        ADS111X_PGAConfType pga)
{
  insertBits(inData, (uint16)pga, PGA_LEN, PGA_POS);
}
static inline void __insert_CONFIG_MODE( uint16* inData,
                                        ADS111X_ModeConfType mode)
{
  insertBits(inData, (uint16)mode, MODE_LEN, MODE_POS);
}
static inline void __insert_CONFIG_DR( uint16* inData,
                                        ADS111X_ModeConfType dataRate)
{
  insertBits(inData, (uint16)dataRate, DR_LEN, DR_POS);
}
static inline void __insert_CONFIG_COMP_MODE( uint16* inData,
                                              ADS111X_ModeConfType compMode)
{
  insertBits(inData, (uint16)compMode, COMP_MODE_LEN, COMP_MODE_POS);
}
static inline void __insert_CONFIG_COMP_POL( uint16* inData,
                                              ADS111X_ComparatorPolarityType compPol)
{
  insertBits(inData, (uint16)compPol, COMP_POL_LEN, COMP_POL_POS);
}
static inline void __insert_CONFIG_COMP_LAT( uint16* inData,
                                              ADS111X_LatchingComparatorType compLat)
{
  insertBits(inData, (uint16)compLat, COMP_LAT_LEN, COMP_LAT_POS);
}
static inline void __insert_CONFIG_COMP_QUE( uint16* inData,
                                              ADS111X_ComparatorQueueType compQue)
{
  insertBits(inData, (uint16)compQue, COMP_QUE_LEN, COMP_QUE_POS);
}

void insert_CONFIG( uint16* inData, ADS111X_ConfigRegType* configReg)
{
  __insert_CONFIG_STAT(inData,      configReg->stat);
  __insert_CONFIG_MUX(inData,       configReg->mux);
  __insert_CONFIG_PGA(inData,       configReg->pga);
  __insert_CONFIG_MODE(inData,      configReg->mode);
  __insert_CONFIG_DR(inData,        configReg->dataRate);
  __insert_CONFIG_COMP_MODE(inData, configReg->compMode);
  __insert_CONFIG_COMP_POL(inData,  configReg->compPolarity);
  __insert_CONFIG_COMP_LAT(inData,  configReg->compLatch);
  __insert_CONFIG_COMP_QUE(inData,  configReg->compQueue);
}
