#include "uds_app.h"
#include "Boot.h"
#include "TP.h"
#include "fls_app.h"
#include "uds_alg_hal.h"
#include "uds_testing_functions.h"
#include <stdio.h>
/*********************************************************/

/*UDS init*/
void UDS_Init() {
#ifdef EN_DELAY_TIME
	gs_stJumpAPPDelayTimeInfo.jumpToAPPDelayTime = UdsAppTimeToCount(DELAY_MAX_TIME_MS);
#endif

	UDS_ALG_HAL_Init();
}



/*uds main function. ISO14229*/
void UDS_MainFun() {

#ifdef EN_ALG_SW
	UDS_ALG_HAL_AddSWTimerTickCnt();
#endif

	if (UDS_IsS3ServerTimeout()) {
		/* Current session timed out. Go back to default session and remove privileges. */
		UDS_SetCurrentSession(DEFAULT_SESSION);
		UDS_SetSecurityLevel(NONE_SECURITY);
		Flash_InitDowloadInfo();
	}


	//TODO: change name of this variable
	//Determines whether a button press should send a simulated message
	static bool doSimulationRequest = TRUE;

	//Checks if the button was pressed
	uint32_t wasButtonPressed = PTC->PDIR & (1<<12);


	/* Check TP for data */
	UDSMessage incomingMessage = {
			.sourceID = 0,
			.dataLength = 0,
			.payload = {0},
			.serviceFunctionCallback = NULL_PTR
	};

	// TODO: Change the format of this bool evaluation
	if (!TP_ReadAFrameDataFromTP(&incomingMessage.sourceID, &incomingMessage.dataLength, incomingMessage.payload)
	     && !wasButtonPressed) {

			//The button has stopped being pressed, so another simulation request can be sent now
			doSimulationRequest = TRUE;
		return;
	}

	if(wasButtonPressed){
		APP_DebugPrintf(LOG_LEVEL_DEBUG, "Button pressed\n");
		//If button was pressed AND we want to send a simulationRequest, then send one
		if(doSimulationRequest){
			APP_DebugPrintf(LOG_LEVEL_DEBUG, "Begin simulated request\n");
			simulateIncomingMessage(&incomingMessage);
			doSimulationRequest = FALSE;
		}
	}

	UDS_SetIsRxUdsMsg(true);

	if (!UDS_IsCurDefaultSession()) {
		/*restart s3server time*/
		UDS_RestartS3Server();
	}

	/*save request id type.*/
	if (!UDS_SetRequestType(incomingMessage.sourceID)) {
		/* Message not from a listed client */
		return;
	}

	uint8 serviceCount;
	const UDSService *serviceLookupTable = UDS_GetUDSServiceInfo(&serviceCount);

	/*get UDS service ID*/
	byte requestSID = incomingMessage.payload[0u];

	APP_DebugPrintf(LOG_LEVEL_DEBUG, "Received UDS request from %x from %lx.\n", requestSID, incomingMessage.sourceID);

	for (int i = 0; i < serviceCount; i++) {

		UDSService service = serviceLookupTable[i];

		if (service.SID != requestSID) {
			continue;
		}

		if (!UDS_IsCurRxIdCanRequest(service.supReqMode)) {
			/* The service is not supported in the current addressing scheme (functional / physical). */
			APP_DebugPrintf(LOG_LEVEL_DEBUG, "\t Unsupported addressing scheme. Replying with 'Service Not Supported'.\n");
			UDS_SetNRC(incomingMessage.payload[0u], NRC_SNS, &incomingMessage);
			return;
		}

		if (!UDS_IsCurSessionCanRequest(service.sessionMode)) {
			/* The service is not supported in the current session mode.*/
			APP_DebugPrintf(LOG_LEVEL_DEBUG,
				"\tUnsupported in current diagnostic session. Replying with 'Service Not Supported'.\n");
			UDS_SetNRC(incomingMessage.payload[0u], NRC_SNS, &incomingMessage);
			return;
		}

		if (!UDS_IsCurSecurityLevelRequest(service.reqLevel)) {
			/* The service is not supported in the current security mode.*/
			APP_DebugPrintf(LOG_LEVEL_DEBUG, "\tUnsupported in current security level. Replying with 'Service Not Supported'.\n");
			UDS_SetNRC(incomingMessage.payload[0u], NRC_SNS, &incomingMessage);
			return;
		}

		/* Service found. Handle it. */

		/* Use the incoming message structure as a buffer for outgoing message. */
		service.serviceHandler((UDSService *)&service, &incomingMessage);

		/* Name change for clarity. */
		UDSMessage* outgoingMessage = &incomingMessage;

		if(doSimulationRequest == 0){

			//Prints info to console
			APP_DebugPrintf(LOG_LEVEL_DEBUG, "\n\nPrinting simulated message:\n\t\tID is: %x\n\t\tLength is: %x\n\t\tPayload is: %x\n\n",
							incomingMessage.sourceID,
							incomingMessage.dataLength,
							incomingMessage.payload);

			return;
		}

		outgoingMessage->sourceID = TP_GetConfigTxMsgID();



		(void)TP_WriteAFrameDataInTP(outgoingMessage->sourceID, outgoingMessage->serviceFunctionCallback, outgoingMessage->dataLength,
				outgoingMessage->payload);
		return;
	}

	/* No service found. Return service not found. */

	UDS_SetNRC(incomingMessage.payload[0], NRC_SNS, &incomingMessage);


}
