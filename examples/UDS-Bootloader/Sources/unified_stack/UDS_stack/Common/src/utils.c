/**
 * @file utils.c
 * @brief Utility function implementations.
 *
 * This source file provides implementations for utility functions declared in utils.h.
 *
 * @author Levent Çelik
 * @date October 7 2024
 */

#include "utils.h"



bool uint16_to_bytes(uint16 value, byte *msb, byte *lsb) {
	if (msb == NULL_PTR || lsb == NULL_PTR) {
		APP_DebugPrintf(LOG_LEVEL_ERROR, "16-bit integer could not be divided into bytes: bytes are uninitialized. \n");
		return false;
	}

	*msb = (value >> 8) & 0xFF;
	*lsb = value & 0xFF;

	return true;
}


uint16 bytes_to_uint16(byte msb, byte lsb) {
	return ((uint16)msb << 8) | lsb;
}

