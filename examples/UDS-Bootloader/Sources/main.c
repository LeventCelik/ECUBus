#include "TP.h"
#include "bootloader_main.h"
#include "flash.h"
#include "uds_testing_functions.h"
volatile int exit_code = 0;

can_message_t g_RXCANMsg;

void CAN_ISR_Callback(uint32_t instance, can_event_t eventType, uint32_t objIdx, void *driverState) {
	if (CAN_EVENT_RX_COMPLETE == eventType) {
		TP_DriverWriteDataInTP(g_RXCANMsg.id, g_RXCANMsg.length, g_RXCANMsg.data);
		CAN_Receive(&can_pal1_instance, 1, &g_RXCANMsg);
	} else if (CAN_EVENT_TX_COMPLETE == eventType) {
		TP_DoTxMsgSuccesfulCallback();
	} else {
	}
}

void AbortTransferMsg(void) { CAN_AbortTransfer(&can_pal1_instance, 2u); }

#define CAN_MSG_TYPE  (CAN_MSG_ID_STD)
#define RX_MAILBOX_ID (1u)
#define TX_MAILBOX_ID (2u)

const can_buff_config_t RXCANMsgConfig = {
	.enableFD = true, .enableBRS = true, .fdPadding = true, .idType = CAN_MSG_TYPE, .isRemote = false};

const can_buff_config_t TXCANMsgConfig = {
	.enableFD = false, .enableBRS = false, .fdPadding = false, .idType = CAN_MSG_TYPE, .isRemote = false};

#define MAX_WAIT_TIME_MS (5000u)
unsigned short g_usMaxDelayUdsMsgTime = MAX_WAIT_TIME_MS;
unsigned char  g_ucIsRxUdsMsg		  = false;

static void BSP_init(void) {
	/*clock init*/
	CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT, g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

	/*GPIO init*/
	PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);

	/*CAN init*/
	CAN_Init(&can_pal1_instance, &can_pal1_Config0);
	CAN_InstallEventCallback(&can_pal1_instance, &CAN_ISR_Callback, NULL);
	CAN_Receive(&can_pal1_instance, RX_MAILBOX_ID, &g_RXCANMsg);
	CAN_ConfigRxBuff(&can_pal1_instance, RX_MAILBOX_ID, &RXCANMsgConfig, RX_FUN_ID);
	CAN_SetRxFilter(&can_pal1_instance, RXCANMsgConfig.idType, RX_MAILBOX_ID, RX_ID_MASK);
	CAN_ConfigTxBuff(&can_pal1_instance, TX_MAILBOX_ID, &TXCANMsgConfig);

	INT_SYS_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);

	/*init flash*/
	InitFlash();
}

void SendMsgMainFun(void) {
	can_message_t txMsg;
	uint32		  msgId	 = 0u;
	uint32		  msgLen = 0u;
	status_t	  status = STATUS_SUCCESS;

	status = CAN_GetTransferStatus(&can_pal1_instance, TX_MAILBOX_ID);
	if (STATUS_SUCCESS == status) {
		if (true == TP_DriverReadDataFromTP(sizeof(txMsg.data), txMsg.data, &msgId, &msgLen)) {
			txMsg.id	 = msgId;
			txMsg.length = (uint8)msgLen;
			CAN_ConfigTxBuff(&can_pal1_instance, TX_MAILBOX_ID, &TXCANMsgConfig);

			CAN_Send(&can_pal1_instance, TX_MAILBOX_ID, &txMsg);
		}
	}
}

int main(void) {
	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!!
	 * ***/
#ifdef PEX_RTOS_INIT
	PEX_RTOS_INIT(); /* Initialization of the selected RTOS. Macro is defined by
						the RTOS component. */
#endif
	/*** End of Processor Expert internal initialization. ***/
	BOOTLOADER_MAIN_Init(&BSP_init, NULL_PTR);

	g_usMaxDelayUdsMsgTime = MAX_WAIT_TIME_MS;

	APP_DebugPrintf(LOG_LEVEL_INFO, "-->Enter S32K144 CAN/CAN FD(500K) bootloader <--\n");
	initSW2Button();

	for (;;) {
		BOOTLOADER_MAIN_Demo();
		SendMsgMainFun();
	} /* loop forever */

	/* please make sure that you never leave main */
	/*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS
	 * component. DON'T MODIFY THIS CODE!!! ***/
#ifdef PEX_RTOS_START
	PEX_RTOS_START(); /* Startup of the selected RTOS. Macro is defined by the
						 RTOS component. */
#endif
	/*** End of RTOS startup code.  ***/
	/*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
	for (;;) {
		if (exit_code != 0) {
			break;
		}
	}
	return exit_code;
	/*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/
