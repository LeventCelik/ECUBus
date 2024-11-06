/**
 * @file utils.h
 * @brief Utility function declarations and definitions.
 *
 * This header file declares utility functions that can be used across the UDS stack.
 * Functions include byte manipulation, logging utilities, and other general-purpose utilities.
 *
 * @author Levent Çelik
 * @date October 7 2024
 */

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif


#include "includes.h"


/* ========================================================================== */
/*                                Type Definitions                            */
/* ========================================================================== */



/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/**
 * @brief Splits a 16-bit unsigned integer into two bytes (MSB and LSB).
 *
 * @param value The 16-bit unsigned integer to split.
 * @param msb Pointer to store the Most Significant Byte.
 * @param lsb Pointer to store the Least Significant Byte.
 *
 * @return true if operation was successful, false otherwise.
 */
bool uint16_to_bytes(uint16 value, byte *msb, byte *lsb);

/**
 * @brief Combines two bytes into one 16-bit unsigned integer.
 *
 * @param msb The Most Significant Byte.
 * @param lsb The Least Significant Byte.
 *
 * @return The combined 16-bit unsigned integer.
 */
uint16 bytes_to_uint16(byte msb, byte lsb);


#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */
