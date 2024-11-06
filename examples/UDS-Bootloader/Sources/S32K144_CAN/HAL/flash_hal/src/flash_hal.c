/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!
 * @file flash_hal.c
 *
 * @brief: Add your description here for this file.
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section Rule_X-Y Rule: X.Y (Advisory/Required)
 * Violates MISRA 2012 Advisory Rule X.Y, Rule description here.
 *
 * @par Version Histroy
 <pre><b>
 Version:   Author:       Date&&Time:      Revision Log: </b>
 V1.0.0  Tomlin Tang  2019-01-17 15:51:03  First Creat
 When you update, please do not forgot to del me and add your info at here.
 </pre>
 */

#include "flash_hal.h"
#include "flash.h"

/*******************************************************************************
 * User Include
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static boolean FLASH_HAL_Init(void);

static boolean FLASH_HAL_EraseSector(const uint32 i_startAddr, const uint32 i_noEraseSectors);

static boolean FLASH_HAL_WriteData(const uint32 i_startAddr, const uint8 *i_pDataBuf, const uint32 i_dataLen);

static boolean FLASH_HAL_ReadData(const uint32 i_startAddr, const uint32 i_readLen, uint8 *o_pDataBuf);

static void FLASH_HAL_Deinit(void);

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_HAL_Init
 * Description   : This function initial this module.
 *
 * Implements : FLASH_HAL_Init_Activity
 *END**************************************************************************/
static boolean FLASH_HAL_Init(void) {
	InitFlashAPI();

	return true;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_HAL_EraseSector
 * Description   : This function is erase flash sectors.
 * Parameters	  :  i_startAddr input for start flash address
 i_noEraseSectors input number of erase sectors
 * Implements : FLASH_HAL_Init_Activity
 *END**************************************************************************/
static boolean FLASH_HAL_EraseSector(const uint32 i_startAddr, const uint32 i_noEraseSectors) {
	boolean retstates	 = false; /* Store the driver APIs return code */
	uint8	ret			 = 0u;
	uint32	length		 = 0u;
	uint32	sectorLength = 0u;

	sectorLength = FLASH_HAL_Get1SectorBytes();

	length = i_noEraseSectors * sectorLength;

	ret = EraseFlashSector(i_startAddr, length);

	if (0u == ret) {
		retstates = true;
	}

	return retstates;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_HAL_WriteData
 * Description   : This function is write data in flash. if write write data
 successfull return TRUE, else return FALSE.
 * Parameters	  :  i_startAddr input for start flash address
 i_pDataBuf write data buf
 i_dataLen write data len
 * Implements : FLASH_HAL_Init_Activity
 *END**************************************************************************/

static boolean FLASH_HAL_WriteData(const uint32 i_startAddr, const uint8 *i_pDataBuf, const uint32 i_dataLen) {
	boolean retstates	 = false;
	uint8	lessWriteLen = 8u;
	uint8	aDataBuf[8u] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu};
	uint32	writeDataLen = 0u;
	uint8	index		 = 0u;

	DisableAllInterrupts();
	if (i_dataLen & (lessWriteLen - 1)) {
		/*if write data more than 8 bytes*/
		if (i_dataLen > lessWriteLen) {
			writeDataLen = i_dataLen - (i_dataLen & (lessWriteLen - 1));
			if (0u == WriteFlash(i_startAddr, i_pDataBuf, writeDataLen)) {
				retstates = true;
			} else {
				retstates = false;
			}

			if ((true == retstates)) {
				for (index = 0u; index < (i_dataLen & (lessWriteLen - 1)); index++) {
					aDataBuf[index] = i_pDataBuf[writeDataLen + index];
				}

				if (0u == WriteFlash(i_startAddr + writeDataLen, aDataBuf, 8u)) {
					retstates = true;
				}
			}
		} else {
			for (index = 0u; index < i_dataLen; index++) {
				aDataBuf[index] = i_pDataBuf[writeDataLen + index];
			}

			if (0u == WriteFlash(i_startAddr + writeDataLen, aDataBuf, 8u)) {
				retstates = true;
			}
		}
	} else {
		if (0u == WriteFlash(i_startAddr, i_pDataBuf, i_dataLen)) {
			retstates = true;
		}
	}

	EnableAllInterrupts();

	return retstates;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_HAL_ReadData
 * Description   : This function is read data in RAM. if read data successfull
 return TRUE, else return FALSE.
 * Parameters	  :  i_startAddr input for start flash address
 i_readLen read data length
 o_pDataBuf read data buf
 * Implements : FLASH_HAL_Init_Activity
 *END**************************************************************************/
static boolean FLASH_HAL_ReadData(const uint32 i_startAddr, const uint32 i_readLen, uint8 *o_pDataBuf) {
	FLS_DebugPrintf(LOG_LEVEL_DEBUG, "\n");

	// ReadFlashMemory(i_startAddr, i_readLen, o_pDataBuf);

	return true;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_HAL_Deinit
 * Description   : This function initial this module.
 *
 * Implements : FLASH_HAL_Deinit_Activity
 *END**************************************************************************/
static void FLASH_HAL_Deinit(void) { FLS_DebugPrintf(LOG_LEVEL_DEBUG, "\n"); }

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_HAL_RegisterFlashAPI
 * Description   : This function is register flash API. The API maybe download
 *from host and storage in RAM.
 *
 *END**************************************************************************/
boolean FLASH_HAL_RegisterFlashAPI(tFlashOperateAPI *o_pstFlashOperateAPI) {
	boolean result = false;

	if (NULL_PTR != o_pstFlashOperateAPI) {
		o_pstFlashOperateAPI->pfFlashInit	  = FLASH_HAL_Init;
		o_pstFlashOperateAPI->pfEraserSecotr  = FLASH_HAL_EraseSector;
		o_pstFlashOperateAPI->pfProgramData	  = FLASH_HAL_WriteData;
		o_pstFlashOperateAPI->pfReadFlashData = FLASH_HAL_ReadData;
		o_pstFlashOperateAPI->pfFlashDeinit	  = FLASH_HAL_Deinit;

		result = true;
	}

	return result;
}

#ifdef EN_APP_INFO_DATA_IN_NONE_FLASH
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_HAL_WriteAPPInfoData
 * Description   : This function is write data in EEPROM or DFLASH. if write
 write data successfull return TRUE, else return FALSE.
 * Parameters	  : i_startAddr input for start flash address
					i_pDataBuf write data buf
					i_dataLen write data len
 * Implements : FLASH_HAL_Init_Activity
 *END**************************************************************************/
boolean FLASH_HAL_WriteAPPInfoData(const uint32 i_startAddr, const uint8 *i_pDataBuf, const uint32 i_dataLen) {
	boolean writeStatus = false;

	if ((NULL_PTR == i_pDataBuf) || (0u == i_dataLen)) {
	}

	return writeStatus;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_HAL_ReadAPPInfoData
 * Description   : This function is read data in RAM. if read data successfull
 return TRUE, else return FALSE.
 * Parameters	  :  i_startAddr input for start flash address
						i_readLen read data length
						o_pDataBuf read data buf
 * Implements : FLASH_HAL_Init_Activity
 *END**************************************************************************/
boolean FLASH_HAL_ReadAPPInfoData(const uint32 i_startAddr, const uint32 i_readLen, uint8 *o_pDataBuf) {
	boolean readStatus = false;

	if ((0u == i_readLen) || (NULL_PTR == o_pDataBuf)) {
		return false;
	}

	return readStatus;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_HAL_EraseAPPInfoData
 * Description   : This function is erase APP information data.
 * Parameters	  :  i_startAddr input for start flash address
						i_noEraseSectors input number of erase sectors
 * Implements : FLASH_HAL_Init_Activity
 *END**************************************************************************/
boolean FLASH_HAL_EraseAPPInfoData(const uint32 i_startAddr, const uint32 i_eraseLength) {
	boolean eraseStatus = false;

	if (0u == i_eraseLength) {
		return false;
	}

	return eraseStatus;
}

#endif

/******************************************************************************
 * EOF
 *****************************************************************************/
