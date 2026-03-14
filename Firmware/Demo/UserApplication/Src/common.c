#include "Std_Types.h"
#include <math.h>
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
void insertBits(uint16* outData, uint16 inBits, uint32 inBitsLen, uint32 inBitsPos)
{
  assert(inBitsPos<16);
  assert(inBitsLen<16);
  assert((inBitsLen + inBitsPos) <= 16);

  uint16 mask = (~(0xFFFF << inBitsLen))<<inBitsPos;
  uint16 clearedData = (*outData) & (~mask);
  *outData = (clearedData | (inBits << inBitsPos));
}