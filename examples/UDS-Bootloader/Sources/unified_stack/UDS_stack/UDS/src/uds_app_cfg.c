#include "uds_app_cfg.h"

// TODO: Do not echo the suppressPosRespMsgIndicaitonBit from the SBF parameter byte

/**
 * Holds the current server state.
 */
typedef struct {
	byte	 sessionMode;			/* Diagnostic Session Mode: Default, Programming, or Extended. */
	byte	 requestAddressingMode;	/* Addressing Mode: Physical or Functional (broadcast). */
	byte	 securityLevel;		    /* Unlocked security level. */
	tUdsTime S3ServerTime;		    /* Timeout value for leaving a non-default session. */
	tUdsTime authLockoutTime;  		/* Timeout value for resetting security request? FIXME: This is not used. */
} UDSState;

typedef struct {
	// TODO: Change types from uint32 to pointer size.
	uint32 startAddr; /*data start address*/
	uint32 dataLen;	  /*data len*/
} tDowloadDataInfo;

/***** XXX: UNUSED STRUCTS? *****/

/**
 * Security access information.
 */
typedef struct {
	byte subfunctionNumber;	/*subfunction number*/
	byte requestSession;		/*request session*/
	byte requestIDMode;		/*request id mode*/
	byte requestSecurityLevel; /*request security level*/
	void (*pfRoutine)(void);	/*routine*/
} tUDS_SecurityAccessInfo;

/*define write data subfunction*/
typedef struct {
	byte subfunction;		 /*subfunction*/
	byte requestSession;	 /*request session*/
	byte requestAddressingMode;	 /*request id mode*/
	byte requestLevel;		 /*request level*/
	void (*pfRoutine)(void); /*routine*/
} tUDS_WriteDataByIdentifierInfo;

/***** UNUSED STRUCTS? END *****/


// tCheckRoutineCtlInfo
typedef enum {
	ERASE_MEMORY_ROUTINE_CONTROL,	  /*check erase memory routine control*/
	CHECK_SUM_ROUTINE_CONTROL,		  /*check sum routine control*/
	CHECK_DEPENDENCY_ROUTINE_CONTROL, /*check dependency routine control*/
	GET_VERSION,					  /*get version*/
} tCheckRoutineCtlInfo;

/**
 * @brief 	Represents the different states in the authentication process
 * 			used by the Authentication Service.
 */
typedef enum {
	/**
	 *  @brief 	The initial (or reset) state of the authentication process.
	 *
	 *  This state is set during server initialization, or after an
	 *  authentication process is reset (through timeouts, de-authentications,
	 *  disconnects, etc.).
	 */
	STATE_INIT,

	/**
	 * @brief 	The state after the server challenge was sent, pending client POWN.
	 *
	 * This corresponds to the period after a successful "Request Challenge
	 * for Authentication" request and before a "Verify POWN" request on ACR.
	 */
	STATE_CHAL_SENT,

	/**
	 * @brief 	The state after the client certificate was verified, pending
	 * 			client POWN.
	 *
	 * This corresponds to the period after a successful "Verify Certificate"
	 * request and before a "POWN" request on APCE.
	 */
	STATE_CERT_VERIFIED_POWN_NECESSARY,

	/**
	 * @brief 	The state after the authentication is complete.
	 *
	 * The authentication process has successfully finished.
	 */
	STATE_AUTH_DONE,
} AuthState;

/**
 * @brief Communication Configuration received during Authentication Service.
 * TODO: Not sure what to do with this yet.
 */
typedef enum {
	CC_DEFAULT,
	CC_SECURE_COMM,
	CC_NO_SECURE_COMM,
	CC_INVALID, // Used as a range indicator; NOT A STATE
} CommConfig;

/**
 * @brief Authentication strategy: PKI, Symmetric CR, Asymmetric CR.
 * XXX: Not sure if this is necessary.
 */
typedef enum { AC_APCE = ACAPCE, AC_ACRAC = ACACRAC, AC_ACRSC = ACACRSC } AuthStrategy;

/**
 * @brief Authentication algorithm.
 * XXX: Not sure if this is necessary.
 */
typedef enum {
	NO_ALG	= 0,
	AES_256 = 1,
} AuthAlg;

/**
 * @brief Subfunction bytes supported by the Authentication Service.
 *
 * These subfunctions correspond to various authentication operations,
 * and stages, such as certificate validation, ownership proving,
 * deauthentication, and reading configuration.
 */
typedef enum {

	/**
	 * @brief Request to leave the authenticated state.
	 *
	 * This subfunction is used to deauthenticate the client,
	 * effectively resetting the authentication state.
	 */
	DEAUTH = 0x0u,

	/**
	 * @brief Alias for DEAUTH.
	 *
	 * The alternative name for the DEAUTH subfunction.
	 */
	DA = DEAUTH,

	/**
	 * @brief Initiate authentication by verifying the certificate.
	 *
	 * This subfunction initiates the authentication process by
	 * verifying the client's certificate.
	 */
	VERIFY_CERT_1D = 0x1u,

	/**
	 * @brief Alias for VERIFY_CERT_1D.
	 *
	 * The alternative name for the VERIFY_CERT_1D subfunction.
	 */
	VCU = VERIFY_CERT_1D,

	/**
	 * @brief Initiate authentication by verifying the certificate
	 * and generating a proof of ownership from the server.
	 *
	 * This process involves both certificate verification and
	 * server-side proof generation for the client's ownership.
	 */
	VERIFY_CERT_2D = 0x2u,

	/**
	 * @brief Alias for VERIFY_CERT_2D.
	 *
	 * The alternative name for the VERIFY_CERT_2D subfunction.
	 */
	VCB = VERIFY_CERT_2D,

	/**
	 * @brief Verify the proof of ownership from the client.
	 *
	 * This subfunction is used to validate the proof of ownership
	 * provided by the client as part of the authentication process.
	 */
	PROOF_OF_OWNERSHIP = 0x3u,

	/**
	 * @brief Alias for PROOF_OF_OWNERSHIP.
	 *
	 * The alternative name for the PROOF_OF_OWNERSHIP subfunction.
	 */
	POWN = PROOF_OF_OWNERSHIP,

	/**
	 * @brief Verify the certificate and extract information
	 * from the certificate to handle it according to its contents.
	 *
	 * This subfunction involves verifying the certificate and
	 * processing the extracted data based on its content.
	 */
	TRANSMIT_CERT = 0x4u,

	/**
	 * @brief Alias for TRANSMIT_CERT.
	 *
	 * The alternative name for the TRANSMIT_CERT subfunction.
	 */
	TC = TRANSMIT_CERT,

	/**
	 * @brief Initiate authentication by requesting the server to
	 * output a challenge.
	 *
	 * This subfunction requests the server to generate a challenge
	 * for the authentication process.
	 */
	REQ_CHAL_FOR_AUTH = 0x5u,

	/**
	 * @brief Alias for REQ_CHAL_FOR_AUTH.
	 *
	 * The alternative name for the REQ_CHAL_FOR_AUTH subfunction.
	 */
	RCFA = REQ_CHAL_FOR_AUTH,

	/**
	 * @brief Request the server to verify the proof of ownership
	 * for unidirectional authentication.
	 *
	 * This subfunction involves validating the client's proof of
	 * ownership in a unidirectional authentication scenario.
	 */
	VERIFY_POWN_1D = 0x6u,

	/**
	 * @brief Alias for VERIFY_POWN_1D.
	 *
	 * The alternative name for the VERIFY_POWN_1D subfunction.
	 */
	VPOWNU = VERIFY_POWN_1D,

	/**
	 * @brief Request the server to verify the client-side proof of
	 * ownership and provide server-side proof of ownership for
	 * bidirectional authentication.
	 *
	 * This subfunction is used for bidirectional authentication,
	 * where both client and server proofs of ownership are validated.
	 */
	VERIFY_POWN_2D = 0x7u,

	/**
	 * @brief Alias for VERIFY_POWN_2D.
	 *
	 * The alternative name for the VERIFY_POWN_2D subfunction.
	 */
	VPOWNB = VERIFY_POWN_2D,

	/**
	 * @brief Indicates the provided authentication configuration of the server.
	 *
	 * This subfunction provides details about the server's authentication
	 * configuration.
	 */
	AUTH_CONFIG = 0x8u,

	/**
	 * @brief Alias for AUTH_CONFIG.
	 *
	 * The alternative name for the AUTH_CONFIG subfunction.
	 */
	AC = AUTH_CONFIG,

	/* 0x09 to 0x7F ISO SAE Reserved */
} AuthSBF;


/**
 * ?: No idea what this is yet.
 */
/*********************** UDS App Config Data ************************/
typedef struct {
	byte calledPeriod; /*called uds period*/
	/*security request count. If over this security request count, locked server
	 * some time.*/
	byte	 securityRequestCnt;
	tUdsTime xLockTime; /*lock time*/
	tUdsTime xS3Server; /*s3 server time. */
} tUdsTimeInfo;

/* UDS time control information config table*/
const static tUdsTimeInfo gs_stUdsAppCfg = {1u, 3u, 10000u, 5000u};

/*uds app time to count*/
#define UdsAppTimeToCount(xTime) ((xTime) / gs_stUdsAppCfg.calledPeriod)

/*get UDS s3 watermark timer. return s3 * S3_TIMER_WATERMARK_PERCENT / 100*/
uint32 UDS_GetUDSS3WatermarkTimerMs(void) {
	const uint32 watermarkTimerMs = (gs_stUdsAppCfg.xS3Server * S3_TIMER_WATERMARK_PERCENT) / 100u;

	return (uint32)watermarkTimerMs;
}

#ifdef EN_DELAY_TIME
/* TODO: Handle the naming here later. */
typedef struct {
	bool isReceiveUDSMsg;
	uint32	jumpToAPPDelayTime;
} tJumpAppDelayTimeInfo;

static tJumpAppDelayTimeInfo gs_stJumpAPPDelayTimeInfo = {false, 0u};
#endif

/*********************** UDS Information ************************/

/********************** Static Function Declarations ************************/
static tUdsTime UDS_GetUdsS3ServerTime(void);

static void UDS_SubUdsS3ServerTime(tUdsTime i_SubTime);

static tUdsTime UDS_GetUdsSecurityReqLockTime(void);

static void UDS_SubUdsSecurityReqLockTime(tUdsTime i_SubTime);


/***********************UDS Information Static Global
 * value************************/
/* UDS support Session mode?¡éRequestId and Security level config */
static UDSState serverState = {
	DEFAULT_SESSION, ERROR_REQUEST_ID, NONE_SECURITY, 0u, 0u,
};

static tUdsTime UDS_GetUdsS3ServerTime(void) { return (serverState.S3ServerTime); }

static void UDS_SubUdsS3ServerTime(tUdsTime i_SubTime) { serverState.S3ServerTime -= i_SubTime; }

static tUdsTime UDS_GetUdsSecurityReqLockTime(void) { return (serverState.authLockoutTime); }

static void UDS_SubUdsSecurityReqLockTime(tUdsTime i_SubTime) { serverState.authLockoutTime -= i_SubTime; }

/*Is security request lock timeout?*/
static inline bool UDS_IsSecurityRequestLockTimeout(void) { return serverState.authLockoutTime == 0; }

/***********************UDS Information Global function************************/
/*set current request id  SUPPORT_PHYSICAL_ADDR/SUPPORT_FUNCTION_ADDR */

/*restart s3server time*/
void UDS_RestartS3Server(void) { serverState.S3ServerTime = UdsAppTimeToCount(gs_stUdsAppCfg.xS3Server); }

/*set currrent session mode. DEFAULT_SESSION/PROGRAM_SESSION/EXTEND_SESSION */
void UDS_SetCurrentSession(const byte i_setSessionMode) { serverState.sessionMode = i_setSessionMode; }

/*********************************************************/

/********* Routine ID configuration **********/

/* Erase memory */
const static byte gs_aEraseMemoryRoutineControlId[] = {0x31, 0x01, 0xFF, 0x00};

/* Checksum */
const static byte gs_aCheckSumRoutineControlId[] = {0x31, 0x01, 0x02, 0x02};

/* Check programming dependency */
const static byte gs_aCheckProgrammingDependencyId[] = {0x31, 0x01, 0xFF, 0x01};

/* Write DID (finger print?) */
const static byte gs_aWriteFingerprintId[] = {0x2E, 0xF1, 0x5A};

/* Get bootloader version */
const static byte gs_aGetVersion[] = {0x31, 0x01, 0x03, 0xFF};



/********************** Helper Functions ************************/

/* Unnecessary wrapper around fsl_memcpy() */

/**********************UDS Service Correlation Subfunction Definitions************************/
// UDS service correlation subfunction definitions
// App memory copy

static void UDS_AppMemcopy(const void *i_pvSource, const uint16 i_CopyLen, void *o_pvDest);

/* Unnecessary wrapper around fsl_memset() */
static void UDS_AppMemset(const byte i_SetValue, const uint16 i_Len, void *m_pvSource);

/* Check routine control rights */
static bool UDS_IsCheckRoutineControlRight(const tCheckRoutineCtlInfo i_eCheckRoutineCtlId,
											const UDSMessage *	   m_pstPDUMsg);

// Check if erase memory routine control
static bool UDS_IsEraseMemoryRoutineControl(const UDSMessage *m_pstPDUMsg);

// Check if checksum routine control
static bool UDS_IsCheckSumRoutineControl(const UDSMessage *m_pstPDUMsg);

// Check programming dependency
static bool UDS_IsCheckProgrammingDependency(const UDSMessage *m_pstPDUMsg);

// Get version
static bool UDS_IsGetVersion(const UDSMessage *m_pstPDUMsg);

// Check write finger print rights
static bool UDS_IsWriteFingerprintRight(const UDSMessage *m_pstPDUMsg);

// Validate download data address
static bool UDS_IsDownloadDataAddrValid(const uint32 i_dataAddr, const uint32 i_dataLen);

// Check if received key is correct
static bool UDS_IsReceivedKeyRight(const byte *i_pReceivedKey, const byte *i_pTxSeed, const byte KeyLen);

// Perform checksum
// If checksum is correct, return TRUE; else return FALSE
static void UDS_DoCheckSum(byte i_TxStatus);

// Perform flash erase
static void UDS_DoEraseFlash(byte i_TxStatus);

// Perform check programming dependency
static bool UDS_DoCheckProgrammingDependency(void);

// Transmit confirmation message callback
static void UDS_TXConfrimMsgCallback(byte i_status);

// Perform response checksum
static void UDS_DoResponseChecksum(byte i_Status);

// Perform erase flash response
static void UDS_DoEraseFlashResponse(byte i_Status);

// Request more time for UDS service when needed
static void UDS_RequestMoreTime(const byte UDSServiceID, void (*pcallback)(byte));

// Perform MCU reset
static void UDS_DoResetMCU(byte i_Txstatus);

/* By Levent */

/**
 * @brief Inserts data and its length information to output array, keeping track of where to insert data
 * with the next operation (for the next "field" in the array).
 * This function is used to add calculated data to the response of UDS messages, e.g. certificates, etc.
 * Sets the first two bytes to data length, and the next required bytes to the provided data.
 *
 * @param arr 			The output array.
 * @param start 		Pointer to the starting index of data.
 * @param data			The data to be inserted.
 * @param data_length	Length of inserted data. limited by 2^16-1.
 *
 * @return true if operation was successful, false otherwise.
 */
static bool insertDataIntoOutput(byte *arr, size_t *start, byte *data, uint16 data_length);


/**
 * @brief Reads data from the input array, assigning the size of the data to *data_length. First two bytes
 * from start contains the field length. Fills the data_buffer's necessary amounts of bytes.
 * Updates start to the next field.
 *
 * @param arr			The input array.
 * @param start			Pointer to the starting index of the field.
 * @param data_buffer	The buffer to hold the read data.
 * @param data_length	Pointer to the size of the read data, read from the first two bytes from the start index.
 *
 * @return true if operation was successful, false otherwise.
 */
static bool readDataFromInput(byte *arr, size_t *start, byte *data_buffer, uint16 *data_length);

static AuthAlg parseAlgorithm(byte data[16]);
static bool AuthChalGen(byte *challenge, uint16 challenge_len);
static bool verifyClientPOWN(AuthAlg alg, byte *client_pown, uint16 client_pown_len, byte *client_chal,
								uint16 client_chal_len);
static byte   verifyClientCert(byte *client_cert, uint16 client_cert_len, byte *client_chal, uint16 client_chal_len);
static bool genServerPOWN(byte *server_pown_buffer, uint16 *server_pown_len, byte *client_chal,
							 uint16 client_chal_len, byte *eph_pub_key, uint16 eph_pub_key_len);
static bool readServerCert(byte *server_cert_buffer, uint16 *server_cert_len);
static bool genSessionKeyInfo(byte *session_key_info_buffer, uint16 session_key_info_len);
static bool genEphKeys(byte *eph_pub_key_buffer, uint16 eph_pub_key_len, byte *eph_priv_key_buffer,
						  uint16 eph_priv_key_len);
static AuthRetPar getAuthRetPar(AuthAlg alg);


/****************************** UDS Service Declarations ***************************************/

// 0x10: Diagnostic session control
static void UDS_DigSession(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x85: Control DTC setting
static void UDS_ControlDTCSetting(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x28: Communication control
static void UDS_CommunicationControl(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x27: Security access
static void UDS_SecurityAccess(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x29: Authentication
static void UDS_Authentication(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x2E: Write data by identifier
static void UDS_WriteDataByIdentifier(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x34: Request download
static void UDS_RequestDownload(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x36: Transfer data
static void UDS_TransferData(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x37: Request transfer exit
static void UDS_RequestTransferExit(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x31: Routine control
static void UDS_RoutineControl(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x11: Reset ECU
static void UDS_ResetECU(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);

// 0x3E: Tester present
static void UDS_TesterPresent(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg);


/*********************** Global UDS Service Lookup Table************************/

const static UDSService gs_astUDSService[] = {

	/* TODO: Comment the structure of these. */

	// Diagnostic Session Control
	{0x10, DEFAULT_SESSION | PROGRAMMING_SESSION | EXTENDED_SESSION, SUPPORT_PHYSICAL_ADDR | SUPPORT_FUNCTION_ADDR,
	 NONE_SECURITY, UDS_DigSession},

	// ECU Reset
	{0x11, PROGRAMMING_SESSION, SUPPORT_PHYSICAL_ADDR | SUPPORT_FUNCTION_ADDR, SECURITY_LEVEL_1, UDS_ResetECU},

	// Security Access
	{0x27, PROGRAMMING_SESSION, SUPPORT_PHYSICAL_ADDR, NONE_SECURITY, UDS_SecurityAccess},

	// Communication Control
	{0x28, DEFAULT_SESSION | PROGRAMMING_SESSION | EXTENDED_SESSION, SUPPORT_PHYSICAL_ADDR | SUPPORT_FUNCTION_ADDR,
	 NONE_SECURITY, UDS_CommunicationControl},

	// Authentication
	{0x29, PROGRAMMING_SESSION, SUPPORT_PHYSICAL_ADDR, NONE_SECURITY, UDS_Authentication},

	// Write DID
	{0x2E, PROGRAMMING_SESSION, SUPPORT_PHYSICAL_ADDR, SECURITY_LEVEL_1, UDS_WriteDataByIdentifier},

	// Routine Control
	{0x31, PROGRAMMING_SESSION, SUPPORT_PHYSICAL_ADDR, SECURITY_LEVEL_1, UDS_RoutineControl},

	// Request Download
	{0x34, PROGRAMMING_SESSION, SUPPORT_PHYSICAL_ADDR, SECURITY_LEVEL_1, UDS_RequestDownload},

	// Transfer Data
	{0x36, PROGRAMMING_SESSION, SUPPORT_PHYSICAL_ADDR, SECURITY_LEVEL_1, UDS_TransferData},

	// Request Transfer Exit
	{0x37, PROGRAMMING_SESSION, SUPPORT_PHYSICAL_ADDR, SECURITY_LEVEL_1, UDS_RequestTransferExit},

	// Tester Present
	{0x3E, DEFAULT_SESSION | PROGRAMMING_SESSION | EXTENDED_SESSION, SUPPORT_PHYSICAL_ADDR | SUPPORT_FUNCTION_ADDR,
	 NONE_SECURITY, UDS_TesterPresent},

	// Control DTC Settings
	{0x85, DEFAULT_SESSION | PROGRAMMING_SESSION | EXTENDED_SESSION, SUPPORT_PHYSICAL_ADDR | SUPPORT_FUNCTION_ADDR,
	 NONE_SECURITY, UDS_ControlDTCSetting},
};


/********************** UDS Service and Subfunction Implementations ************************/

/*dig session*/
static void UDS_DigSession(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {
	byte requestSubfunction = 0u;

	ASSERT(NULL_PTR == m_pstPDUMsg); // This 'ASSERT' macro is the opposite of what ASSERT
									 // should do. Quite unintuitive.
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	requestSubfunction = m_pstPDUMsg->payload[1u];

	/*set send postive message*/
	m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;
	m_pstPDUMsg->payload[1u] = requestSubfunction;
	m_pstPDUMsg->dataLength	  = 2u;

	/*sub function*/
	switch (requestSubfunction) {
	case 0x01u: /*default mode*/
	case 0x81u:
		UDS_SetCurrentSession(DEFAULT_SESSION);

		if (0x81u == requestSubfunction) {
			m_pstPDUMsg->dataLength = 0u;
		}

		break;

	case 0x02u: /*program mode*/
	case 0x82u:
		UDS_SetCurrentSession(PROGRAMMING_SESSION);

		if (0x82u == requestSubfunction) {
			m_pstPDUMsg->dataLength = 0u;
		}

		/*restart s3server time*/
		UDS_RestartS3Server();

		break;

	case 0x03u: /*extend mode*/
	case 0x83u:
		UDS_SetCurrentSession(EXTENDED_SESSION);

		if (0x83u == requestSubfunction) {
			m_pstPDUMsg->dataLength = 0u;
		}

		/*restart s3server time*/
		UDS_RestartS3Server();
		break;

	default:
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SFNS, m_pstPDUMsg);
		break;
	}
}

/*control DTC setting*/
static void UDS_ControlDTCSetting(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {

	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	byte requestSubfunction = m_pstPDUMsg->payload[1u];

	switch (requestSubfunction) {
	case 0x01u:
	case 0x02u:
		m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;
		m_pstPDUMsg->payload[1u] = requestSubfunction;
		m_pstPDUMsg->dataLength	  = 2u;
		break;

	case 0x81u:
	case 0x82u:
		m_pstPDUMsg->dataLength = 0u;
		break;

	default:
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SFNS, m_pstPDUMsg);
		break;
	}
}

/*communication control*/
static void UDS_CommunicationControl(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {

	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	byte requestSubfunction = m_pstPDUMsg->payload[1u];

	switch (requestSubfunction) {
	case 0x0u:
	case 0x03u:
		m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;
		m_pstPDUMsg->payload[1u] = requestSubfunction;
		m_pstPDUMsg->dataLength	  = 2u;

		break;

	case 0x80u:
	case 0x83u:
		/*don't transmit uds message.*/
		m_pstPDUMsg->payload[0u] = 0u;
		m_pstPDUMsg->dataLength	  = 0u;

		break;

	default:
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SFNS, m_pstPDUMsg);

		break;
	}
}


/**
 * @param UDSService *i_pstUDSServiceInfo
 */
static void UDS_SecurityAccess(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {

	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	static byte s_aSeedBuf[SEC_PARAM] = {0u};
	byte requestSubfunction = m_pstPDUMsg->payload[1u];

	switch (requestSubfunction) {
	case 0x01:
		m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;


		if (UDS_ALG_HAL_GetRandom(SEC_PARAM, s_aSeedBuf)) {
			UDS_AppMemcopy(s_aSeedBuf, SEC_PARAM, &m_pstPDUMsg->payload[2u]);
			m_pstPDUMsg->dataLength = 2u + SEC_PARAM;
		} else {
			APP_DebugPrintf(LOG_LEVEL_ERROR, "Could not generate random number. Aborting security access.\n");
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_FPEORA, m_pstPDUMsg);
		}
		return;

	case 0x02:
		/*count random to key and check received key right?*/
		if (UDS_IsReceivedKeyRight(&m_pstPDUMsg->payload[2u], s_aSeedBuf, SEC_PARAM)) {
			m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;
			m_pstPDUMsg->dataLength = 2u;
			UDS_SetSecurityLevel(SECURITY_LEVEL_1);
			APP_DebugPrintf(LOG_LEVEL_INFO, "Security access granted. \n");
		} else {
			APP_DebugPrintf(LOG_LEVEL_ERROR, "Incorrect key received. Aborting security access.\n");
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_IK, m_pstPDUMsg);
		}

		/* Clear the key */
		UDS_AppMemset(0x1u, sizeof(s_aSeedBuf), s_aSeedBuf);

		break;

	default:
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SFNS, m_pstPDUMsg);
		APP_DebugPrintf(LOG_LEVEL_WARNING, "Received unsupported subfunction request during security access.\n");
		break;
	}
}

// Authentication
static void UDS_Authentication(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {

	/**
	 * TODO: DA 		✔
	 * TODO: VCU		✔
	 * TODO: VCB		✔
	 * TODO: POWN		✔
	 * TODO: TC			IN-PROGRESS
	 * TODO: RFCA		✔
	 * TODO: VPOWNU		✔
	 * TODO: VPOWNB		✔
	 * TODO: AC			✔
	 * TODO: Negative
	 *********************
	 * TODO: Establish session keys
	 * TODO: Handle IMLOIF errors
	 */

	ASSERT(NULL_PTR == i_pstUDSServiceInfo); // TODO: Refactor ASSERT to be intuitive.
	ASSERT(NULL_PTR == m_pstPDUMsg);		 // TODO: In fact, handle errors instead of bricking the ECU.
	/*************************************************************************/
	/*             Variables that hold the state of authentication           */
	/* Any additions to these should also be added to AUTH_CLEAN_STATE macro */
	/*************************************************************************/
	static AuthState	   state	   = STATE_INIT; // Where in the authentication step are we?
	static CommConfig comm_config = CC_DEFAULT; // What will happen after authentication?
	static AuthAlg		   algorithm   = AES_256;	 // Which algorithm will be used for crypto?
	static AuthStrategy	   auth_config = AC_ACRSC;	 // Which concept will be used for auth? TODO: Place somewhere else
	/**************************************************************************/
	static byte challenge[SEC_PARAM] = {0u};

	byte *ret_buf = m_pstPDUMsg->payload;

	// Assume success, handle negatives as they occur
	ret_buf[0u] = i_pstUDSServiceInfo->SID + 0x40u;
	byte SBF = ret_buf[1u];
	switch (SBF) {
	case DA: {
		m_pstPDUMsg->dataLength = 3;
		UDS_SetSecurityLevel(NONE_SECURITY); // TODO: Expand on this, add NRCs etc.
		ret_buf[2u] = DE_AUTHENTICATION_SUCCESSFUL;
		APP_DebugPrintf(LOG_LEVEL_INFO, "deAuthentication complete.\n");
		goto cleanup;
	}

	case VCB: {
		/**
		 * Verify Certificate Bidirectional
		 ***************************************************
		 * BYTE            				REQUEST
		 *
		 * #0              				SID (Service ID)
		 * #1              				SBF (Subfunction Byte)
		 * #2           		   		Comm Config (Communication Configuration)
		 * #3..4 (2)			   		Client Cert Length (m)
		 * #5..m+4 (m)	   				Client Cert
		 * #m+5..m+6 (2)   				Client Challenge Length (n)
		 * #m+7..n+m+6 (n) 				Client Challenge
		 *
		 ***************************************************
		 * BYTE         	   			RESPONSE
		 *
		 * #0       	       			Response SID (SID + 0x40)
		 * #1   	           			SBF
		 * #2	              			Auth Ret Param (Authentication Return Parameter)
		 * #3..4 (2)	   				Server Challenge Length (p)
		 * #5..p+4 (p)	   				Server Challenge
		 * #p+5..p+6 (2)   				Server Certificate Length (r)
		 * #p+7..r+p+6 (r)	 			Server Certificate
		 * #p+r+7..p+r+8 (2)   			Server POWN Length (s)
		 * #p+r+9..p+r+s+8 (s)	 		Server POWN
		 * #p+r+s+9..p+r+s+10 (2)		Server Ephemeral Pub. Key Length (q)
		 * #p+r+s+11..p+r+s+q+10 (q)	Server Ephemeral Pub. Key
		 */

		/* Fall through VCU to avoid code duplication */
	}
		/* no break */

	case VCU: {
		/**
		 * Verify Certificate Unidirectional
		 ***************************************************
		 * BYTE            REQUEST
		 *
		 * #0              SID (Service ID)
		 * #1              SBF (Subfunction Byte)
		 * #2              Comm Config (Communication Configuration)
		 * #3..4 (2)	   Client Cert Length (m)
		 * #5..m+4 (m)	   Client Cert
		 * #m+5..m+6 (2)   Client Challenge Length (n)
		 * #m+7..n+m+6 (n) Client Challenge
		 *
		 ***************************************************
		 * BYTE            RESPONSE
		 *
		 * #0              Response SID (SID + 0x40)
		 * #1              SBF
		 * #2              Auth Ret Param (Authentication Return Parameter)
		 * #3..4 (2)	   Server Challenge Length (p)
		 * #5..p+4 (p)	   Server Challenge
		 * #p+5..p+6 (2)   Server Ephemeral Pub. Key Length (q)
		 * #p+7..q+p+6 (q) Server Ephemeral Pub. Key
		 */

		// Read relevant data
		comm_config = (CommConfig)ret_buf[2u];
		if (comm_config >= CC_INVALID) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_CDUF, m_pstPDUMsg);
			goto cleanup;
		}

		// Read client cert
		byte  client_cert[MAX_CERT_LEN];
		size_t  start		   = 3u;
		uint16 client_cert_len;

		if (!readDataFromInput(ret_buf, &start, client_cert, &client_cert_len)) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_IMLOIF, m_pstPDUMsg);
			APP_DebugPrintf(LOG_LEVEL_ERROR, "Could not read data from input. Aborting authentication. \n");
			goto cleanup;
		}

		// Read client challenge
		byte  client_chal[SEC_PARAM];
		uint16 client_chal_len;

		if (!readDataFromInput(ret_buf, &start, client_chal, &client_chal_len)) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_IMLOIF, m_pstPDUMsg);
			APP_DebugPrintf(LOG_LEVEL_ERROR, "Could not read data from input. Aborting authentication. \n");
			goto cleanup;
		}

		// Verify client certificate
		byte client_cert_check_nrc = verifyClientCert(client_cert, client_cert_len, client_chal, client_chal_len);
		if (client_cert_check_nrc) {
			// Only entered when client_check_nrc != 0; i.e., failure.
			APP_DebugPrintf(LOG_LEVEL_ERROR, "Client certificate could not be verified. Aborting authentication. \n");
			UDS_SetNRC(i_pstUDSServiceInfo->SID, client_cert_check_nrc, m_pstPDUMsg);
			goto cleanup;
		}

		// TODO: Save the certificate after verifying.

		// Create challenge
		if (!AuthChalGen(challenge, SEC_PARAM)) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_CCF, m_pstPDUMsg);
			goto cleanup;
		}

		EphKeys eph_keys	  = {0};
		eph_keys.pub_key_len  = comm_config == CC_SECURE_COMM ? EPHEMERAL_PUB_KEY_LEN : 0;
		eph_keys.priv_key_len = comm_config == CC_SECURE_COMM ? EPHEMERAL_PRIV_KEY_LEN : 0;
		if (eph_keys.pub_key_len != 0) {
			byte temp_eph_pub_key[eph_keys.pub_key_len];
			byte temp_eph_priv_key[eph_keys.priv_key_len];

			if (!genEphKeys(temp_eph_pub_key, eph_keys.pub_key_len, temp_eph_priv_key, eph_keys.priv_key_len)) {
				APP_DebugPrintf(LOG_LEVEL_ERROR, "Ephemeral key creation failed. Aborting authentication.\n");
				UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SKCDF, m_pstPDUMsg);
				goto cleanup;
			}

			eph_keys.pub_key  = temp_eph_pub_key;
			eph_keys.priv_key = temp_eph_priv_key;

			// TODO: Record and use ephemeral keys.
		}

		byte *server_cert;
		uint16 server_cert_len = 0;
		byte *server_pown;
		uint16 server_pown_len = 0;
		if (SBF == VCB) {
			// Fallthrough from VCB. Send server certificate and POWN.

			// Read server certificate from storage.
			byte temp_server_cert[MAX_CERT_LEN];
			if (!readServerCert(temp_server_cert, &server_cert_len)) {
				APP_DebugPrintf(LOG_LEVEL_ERROR, "Certificate could not be read. Aborting authentication.\n");
				UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RTNA, m_pstPDUMsg);
				goto cleanup;
			}
			server_cert = temp_server_cert;

			// Create server POWN
			byte temp_server_pown[MAX_POWN_LEN];

			if (!genServerPOWN(temp_server_pown, &server_pown_len, client_chal, client_chal_len, eph_keys.pub_key,
							   eph_keys.pub_key_len)) {
				APP_DebugPrintf(LOG_LEVEL_ERROR, "POWN could not be generated. Aborting authentication.\n");
				UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RTNA, m_pstPDUMsg);
				goto cleanup;
			}
			server_pown = temp_server_pown;
		}

		// Build response

		m_pstPDUMsg->dataLength = SEC_PARAM + eph_keys.pub_key_len + 7;
		m_pstPDUMsg->dataLength += SBF == VCB ? server_cert_len + server_pown_len + 4
											: 0; // Fallthrough from VCB. Allocate space for POWN and response.
		ret_buf[2u]		   = CERTIFICATE_VERIFIED_OWNERSHIP_VERIFICATION_NECESSARY;
		start			   = 3;
		ret_buf[start]	   = (SEC_PARAM >> 8) & 0xFF; // MSB
		ret_buf[start + 1] = SEC_PARAM & 0xFF;		  // LSB
		UDS_AppMemcopy(challenge, SEC_PARAM, &ret_buf[start + 2]);
		start += SEC_PARAM + 2;

		if (SBF == VCB) {
			// Fallthrough from VCB. Add certificate and POWN to the response.
			ret_buf[start]	   = (server_cert_len >> 8) & 0xFF; // MSB
			ret_buf[start + 1] = server_cert_len & 0xFF;		// LSB
			UDS_AppMemcopy(server_cert, server_cert_len, &ret_buf[start + 2]);
			start += server_cert_len + 2;

			ret_buf[start]	   = (server_pown_len >> 8) & 0xFF; // MSB
			ret_buf[start + 1] = server_pown_len & 0xFF;		// LSB
			UDS_AppMemcopy(server_pown, server_pown_len, &ret_buf[start + 2]);
			start += server_pown_len + 2;
		}

		ret_buf[start]	   = (eph_keys.pub_key_len >> 8) & 0xFF; // MSB
		ret_buf[start + 1] = eph_keys.pub_key_len & 0xFF;		 // LSB
		UDS_AppMemcopy(eph_keys.pub_key, eph_keys.pub_key_len, &ret_buf[start + 2]);

		state = STATE_CERT_VERIFIED_POWN_NECESSARY;
		APP_DebugPrintf(LOG_LEVEL_INFO, "verifyCertificateUnidirectional complete.\n");
		return;
	}

	case POWN: {
		/**
		 * Proof of Ownership
		 ***************************************************
		 * BYTE            REQUEST
		 *
		 * #0              SID (Service ID)
		 * #1              SBF (Subfunction Byte)
		 * #2..3 (2)	   Client POWN Length (m)
		 * #4..m+3 (m)	   Client POWN
		 * #m+4..m+5 (2)   Client Ephemeral Pub. Key Length Length (n)
		 * #m+6..n+m+5 (n) Client Empemeral Pub. Key
		 *
		 ***************************************************
		 * BYTE            RESPONSE
		 *
		 * #0              Response SID (SID + 0x40)
		 * #1              SBF
		 * #2              Auth Ret Param (Authentication Return Parameter)
		 * #3..4 (2)	   Session Key Info Length (p)
		 * #5..p+4 (p)	   Session Key Info
		 */
		if (state != STATE_CERT_VERIFIED_POWN_NECESSARY) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RSE, m_pstPDUMsg);
			goto cleanup;
		}

		// Read and check client proof of ownership
		byte client_pown[MAX_POWN_LEN];
		size_t start = 3u;
		// TODO: (unit16) conversion is implicit and not necessary
		uint16 client_pown_len = ((uint16)ret_buf[start] << 8) | ret_buf[start + 1];
		UDS_AppMemcopy(&ret_buf[start + 2], client_pown_len, client_pown);

		if (!verifyClientPOWN(NO_ALG, client_pown, client_pown_len, NULL, 0)) {
			// TODO: Use the saved client certificate to verify client POWN.
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_OVF, m_pstPDUMsg);
			goto cleanup;
		}

		// Build response
		ret_buf[2u]			  = OVAC;
		m_pstPDUMsg->dataLength = 3;
		if (comm_config == CC_SECURE_COMM) {
			// Read public ephemeral key
			byte client_eph_pub_key[EPHEMERAL_PUB_KEY_LEN];
			start += client_pown_len + 2u;
			uint16 client_eph_pub_key_len = ((uint16)ret_buf[start] << 8) | ret_buf[start + 1];
			UDS_AppMemcopy(&ret_buf[start + 2], client_eph_pub_key_len, client_eph_pub_key);

			// TODO: Derive session keys

			// TODO: Add key info to response
		}

		UDS_SetSecurityLevel(SECURITY_LEVEL_1); // TODO: Expand on this, add NRC SARF, etc.
		// TODO: Redesign "Security level" completely for Auth service.
		state = STATE_AUTH_DONE;
		APP_DebugPrintf(LOG_LEVEL_INFO, "proofOfOwnership complete.\n");
		return;
	}

	case TC: {
		/**
		 * Transmit Certificate
		 ***************************************************
		 * BYTE            	REQUEST
		 *
		 * #0              	SID (Service ID)
		 * #1              	SBF (Subfunction Byte)
		 * #2..3 (2)	   	Certificate Evaluation ID
		 * #4..5 (2)		Certificate length (m)
		 * #6..m+5 (m)	   	Certificate
		 *
		 ***************************************************
		 * BYTE            RESPONSE
		 *
		 * #0              Response SID (SID + 0x40)
		 * #1              SBF
		 * #2              Auth Ret Param (Authentication Return Parameter)
		 */
		// TODO: SBF: transmitCertificate
		APP_DebugPrintf(LOG_LEVEL_INFO, "transmitCertificate complete.\n");
		return;
	}

	case RCFA: {
		/**
		 * Request Challenge For Authentication
		 ***************************************************
		 * BYTE            REQUEST
		 *
		 * #0              SID (Service ID)
		 * #1              SBF (Subfunction Byte)
		 * #2              Comm Config (Communication Configuration)
		 * #3..18 (16)     Alg Indicator (Algorithm Indicator)
		 *
		 ***************************************************
		 * BYTE            RESPONSE
		 *
		 * #0              Response SID (SID + 0x40)
		 * #1              SBF
		 * #2              Auth Ret Param (Authentication Return Parameter)
		 * #3..18 (16)	   Alg Indicator
		 * #19..20 (2)     Challenge Length (m)
		 * #21..m+20 (m)   Challenge
		 * #m+21..m+22 (2) Length of Needed Additional Parameter (n)
		 * #m+23..n+m+22 (n) Needed Additional Parameter
		 */

		// Read relevant data
		comm_config = (CommConfig)ret_buf[2u];
		if (comm_config >= CC_INVALID) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_CDUF, m_pstPDUMsg);
			goto cleanup;
		}
		byte algorithmIndicator[16];
		UDS_AppMemcopy(&ret_buf[3], 16, algorithmIndicator);
		algorithm = parseAlgorithm(algorithmIndicator);
		if (!algorithm) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_CDUF, m_pstPDUMsg);
			goto cleanup;
		}

		// Create challenge
		if (!AuthChalGen(challenge, SEC_PARAM)) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_CCF, m_pstPDUMsg);
			goto cleanup;
		}
		ret_buf[19] = (SEC_PARAM >> 8) & 0xFF; // MSB
		ret_buf[20] = SEC_PARAM & 0xFF;		   // LSB
		UDS_AppMemcopy(challenge, SEC_PARAM, &ret_buf[21]);

		// Add additional parameters as needed here
		uint16 addtnl_param_len = 0;

		// Build response
		m_pstPDUMsg->dataLength	= SEC_PARAM + addtnl_param_len + 23u;
		ret_buf[2u]				= getAuthRetPar(algorithm); // FIXME
		ret_buf[SEC_PARAM + 20] = 0u;
		ret_buf[SEC_PARAM + 21] = 0u;

		state = STATE_CHAL_SENT;
		APP_DebugPrintf(LOG_LEVEL_INFO, "requestChallengeForAuthentication complete.\n");
		return;
	}

	case VPOWNB: {
		/**
		 * Verify Proof of Ownership Bidirectional
		 ***************************************************
		 * BYTE            			REQUEST
		 *
		 * #0              			SID (Service ID)
		 * #1              			SBF (Subfunction Byte)
		 * #2..17 (16)     			Alg Indicator (Algorithm
		 *                 			Indicator)
		 * #18..19 (2)     			Client POWN len (m) [MSB, LSB]
		 * #20..m+19 (m)   			Client POWN
		 * #m+20..m+21 (2) 			Client Chal len (n)
		 * #m+22..n+m+21 (n)		Client Chal
		 * #n+m+22..n+m+23 (2)		Additional Param len (o)
		 * #n+m+24..n+m+o+23 (o)	Additional Param
		 *
		 ****************************************************
		 * BYTE						RESPONSE
		 *
		 * #0						SID
		 * #1						SBF
		 * #2						Auth Ret Param
		 * #3..18 (16)				Alg Indicator
		 * #19..20 (2)				Server POWN len (q) <- Different from VPOWNU
		 * #21..q+20 (q)			Server POWN <- Different from VPOWNU
		 * #q+21..q+22 (2)			Session key info len (p)
		 * #q+23..p+q+23 (p)		Session key info
		 */

		/* Fall through VPOWNU to avoid code duplication */
	}
		/* no break */

	case VPOWNU: {
		/**
		 * Verify Proof of Ownership Unidirectional
		 ***************************************************
		 * BYTE            			REQUEST
		 * #0              			SID (Service ID)
		 * #1              			SBF (Subfunction Byte)
		 * #2..17 (16)     			Alg Indicator (Algorithm
		 *                 			Indicator)
		 * #18..19 (2)     			Client POWN len (m) [MSB, LSB]
		 * #20..m+19 (m)   			Client POWN
		 * #m+20..m+21 (2) 			Client Chal len (n)
		 * #m+22..n+m+21 (n)		Client Chal
		 * #n+m+22..n+m+23 (2)		Additional Param len (o)
		 * #n+m+24..n+m+o+23 (o)	Additional Param
		 *
		 ****************************************************
		 * BYTE						RESPONSE
		 * #0						SID
		 * #1						SBF
		 * #2						Auth Ret Param
		 * #3..18 (16)				Alg Indicator
		 * #19..20 (2)				Session Key Info len (p)
		 * #21..p+20 (p)			Session Key Info
		 */

		if (state != STATE_CHAL_SENT) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RSE, m_pstPDUMsg);
			goto cleanup;
		}
		byte algorithmIndicator[16];
		UDS_AppMemcopy(&ret_buf[2], 16, algorithmIndicator);
		const AuthAlg temp_alg = parseAlgorithm(algorithmIndicator);

		if (algorithm != temp_alg) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_CDUF, m_pstPDUMsg);
			goto cleanup;
		}

		// Read and check client proof of ownership
		byte  client_pown[MAX_POWN_LEN];
		size_t  start		   = 18u;
		uint16 client_pown_len = ((uint16)ret_buf[start] << 8) | ret_buf[start + 1];
		UDS_AppMemcopy(&ret_buf[start + 2], client_pown_len, client_pown);

		// Read client challenge
		byte client_chal[SEC_PARAM];
		start += client_pown_len + 2u;
		uint16 client_chal_len = ((uint16)ret_buf[start] << 8) | ret_buf[start + 1];
		UDS_AppMemcopy(&ret_buf[start + 2], client_chal_len, client_chal);

		if (!verifyClientPOWN(algorithm, client_pown, client_pown_len, client_chal, client_chal_len)) {
			UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_OVF, m_pstPDUMsg);
			goto cleanup;
		}

		// Read additional parameters
		byte addtnl_params[MAX_ADDTNL_PARAM_LEN];
		start					= client_pown_len + client_chal_len + 22;
		uint16 addtnl_param_len = ((uint16)ret_buf[start] << 8) | ret_buf[start + 1];
		if (addtnl_param_len > 0) {
			UDS_AppMemcopy(&ret_buf[start + 2], addtnl_param_len, addtnl_params);
			// TODO: Handle additional parameters
		}

		size_t k_info_len = comm_config == CC_SECURE_COMM ? SESSION_KEY_INFO_LEN : 0;

		// Build response
		m_pstPDUMsg->dataLength = k_info_len + 21;
		ret_buf[2u]			  = OVAC;
		UDS_AppMemcopy(algorithmIndicator, 16, &ret_buf[3u]);
		size_t k_info_begin = 19; // TODO: Can mayhaps change to 'start'
		if (SBF == VPOWNB) {
			// Fallthrough from VPOWNB, handle server POWN.
			uint16 server_pown_len;
			byte  server_pown[MAX_POWN_LEN];
			genServerPOWN(server_pown, &server_pown_len, client_chal, client_chal_len, NULL, 0);

			ret_buf[19] = (server_pown_len >> 8) & 0xFF; // MSB
			ret_buf[20] = server_pown_len & 0xFF;		 // LSB
			UDS_AppMemcopy(server_pown, server_pown_len, &ret_buf[21]);

			// Fix key info position and total message length
			m_pstPDUMsg->dataLength += server_pown_len + 2;
			k_info_begin += server_pown_len + 2;
		}

		ret_buf[k_info_begin]	  = (k_info_len >> 8) & 0xFF; // MSB
		ret_buf[k_info_begin + 1] = k_info_len & 0xFF;		  // LSB

		if (k_info_len != 0) {
			byte session_key_info[k_info_len];
			if (!genSessionKeyInfo(session_key_info, k_info_len)) {
				UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SKCDF, m_pstPDUMsg);
				goto cleanup;
			}
			UDS_AppMemcopy(session_key_info, k_info_len, &ret_buf[k_info_begin + 2]);
			// TODO: Record and use session keys.
		}

		UDS_SetSecurityLevel(SECURITY_LEVEL_1); // TODO: Expand on this, add NRC SARF, etc.
		state = STATE_AUTH_DONE;

		APP_DebugPrintf(LOG_LEVEL_INFO, "verifyProofOfOwnershipUnidirectional complete.\n");
		return;
	}

	case AC: {
		/**
		 * Authentication Configuration
		 ***************************************************
		 * BYTE            REQUEST
		 *
		 * #0              SID (Service ID)
		 * #1              SBF (Subfunction Byte)
		 ***************************************************
		 * BYTE            RESPONSE
		 *
		 * #0              Response SID (SID + 0x40)
		 * #1              SBF
		 * #2              Auth Ret Param (Authentication Return Parameter)
		 */
		m_pstPDUMsg->dataLength = 3;
		ret_buf[2u]			  = auth_config;
		APP_DebugPrintf(LOG_LEVEL_INFO, "authenticationConfiguration complete.\n");
		return;
	}

	default: {
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SFNS, m_pstPDUMsg);
		APP_DebugPrintf(LOG_LEVEL_DEBUG, "Unknown subfunction: 0x%02X\n", SBF);
		return;
	}
	}

	// Sets the state variables to their defaults.
	APP_DebugPrintf(LOG_LEVEL_ERROR, "The code should never reach here. \n");
cleanup:
	state		= STATE_INIT;
	comm_config = CC_DEFAULT;
	algorithm	= AES_256;
	// TODO: Clean certificate
	return;
}

/*write data by identifier*/
static void UDS_WriteDataByIdentifier(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {
	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	/*Is write fingerprint id right?*/
	if (true) //== UDS_IsWriteFingerprintRight(m_pstPDUMsg))
	{
		/*do write fingerprint*/
		// Flash_Savret_bufUMsg->aDataBuf[3u],
		// (m_pstPDUMsg->dataLength - 3u));
		m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;
		m_pstPDUMsg->payload[1u] = 0xF1u;
		m_pstPDUMsg->payload[2u] = 0x5Au;
		m_pstPDUMsg->dataLength	  = 3u;
	} else {
		/*don't have this routine control ID*/
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SFNS, m_pstPDUMsg);
	}
}

/*download data info*/
static tDowloadDataInfo gs_stDowloadDataInfo = {0u, 0u};

/* received block number */
static byte gs_RxBlockNum = 0;

/*request download*/
static void UDS_RequestDownload(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {
	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	if (m_pstPDUMsg->dataLength < (DOWLOAD_DATA_ADDR_LEN + DOWLOAD_DATA_LEN + 1u + 2u)) {
		APP_DebugPrintf(LOG_LEVEL_ERROR, "Invalid message length. Aborting Request Download. \n");
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_IMLOIF, m_pstPDUMsg);
		Flash_InitDowloadInfo();
		/*set request transfer data step(0x34 service)*/
		Flash_SetNextDownloadStep(FL_REQUEST_STEP);
		return;
	}



	/* Read data address */
	gs_stDowloadDataInfo.startAddr = 0u;
	for (int i = 0; i < DOWLOAD_DATA_ADDR_LEN; i++) {
		gs_stDowloadDataInfo.startAddr <<= 8u;
		/* 3u = N_PCI(1) + SID34(1) + dataFormatldentifier(1) */
		gs_stDowloadDataInfo.startAddr |= m_pstPDUMsg->payload[i + 3];
	}

	/* Read data length */
	gs_stDowloadDataInfo.dataLen = 0u;
	for (int i = 0u; i < DOWLOAD_DATA_LEN; i++) {
		gs_stDowloadDataInfo.dataLen <<= 8u;
		gs_stDowloadDataInfo.dataLen |= m_pstPDUMsg->payload[i + 7u];
	}

	/* Check validity of data address and length */
	if (!UDS_IsDownloadDataAddrValid(gs_stDowloadDataInfo.startAddr, gs_stDowloadDataInfo.dataLen)) {
		APP_DebugPrintf(LOG_LEVEL_ERROR, "Invalid data address or length. Aborting Request Download.\n");
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_ROOR, m_pstPDUMsg);
		Flash_InitDowloadInfo();
		/*set request transfer data step(0x34 service)*/
		Flash_SetNextDownloadStep(FL_REQUEST_STEP);
		return;
	}

	/*set wait transfer data step(0x34 service)*/
	Flash_SetNextDownloadStep(FL_TRANSFER_STEP);

	/*save received program addr and data len*/
	Flash_SaveDownloadDataInfo(gs_stDowloadDataInfo.startAddr, gs_stDowloadDataInfo.dataLen);

	/*fill postive message*/
	m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;
	m_pstPDUMsg->payload[1u] = 0x10u;
	m_pstPDUMsg->payload[2u] = 0x80u;
	m_pstPDUMsg->dataLength	  = 3u;

	/*set wait received block number*/
	gs_RxBlockNum = 1u;
}

/*transfer data*/
static void UDS_TransferData(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {
	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	if (Flash_GetCurDownloadStep() != FL_TRANSFER_STEP) {
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RSE, m_pstPDUMsg);
		APP_DebugPrintf(LOG_LEVEL_ERROR, "Download is not at transfer step. Aborting download.\n");
		goto cleanup;
	}

	if (m_pstPDUMsg->payload[1] != gs_RxBlockNum) {
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RSE, m_pstPDUMsg);
		APP_DebugPrintf(LOG_LEVEL_ERROR, "Received block number does not match (?). Aborting download.\n");
		goto cleanup;
	}

	gs_RxBlockNum++;

	if (!Flash_ProgramRegion(gs_stDowloadDataInfo.startAddr, &m_pstPDUMsg->payload[2], m_pstPDUMsg->dataLength - 2)) {
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_CNC, m_pstPDUMsg);
		APP_DebugPrintf(LOG_LEVEL_ERROR, "Data could not be written to specified region. Aborting download.\n");
		goto cleanup;
	}

	gs_stDowloadDataInfo.startAddr += m_pstPDUMsg->dataLength - 2;
	gs_stDowloadDataInfo.dataLen -= m_pstPDUMsg->dataLength - 2;

	if (gs_stDowloadDataInfo.dataLen == 0) {
		/* Received all data */
		gs_RxBlockNum = 0;
		Flash_SetNextDownloadStep(FL_EXIT_TRANSFER_STEP);
		APP_DebugPrintf(LOG_LEVEL_INFO, "Received all download data.\n");
	}

	m_pstPDUMsg->payload[0] = i_pstUDSServiceInfo->SID + 0x40;
	m_pstPDUMsg->dataLength = 4;
	return;

	cleanup:
		Flash_InitDowloadInfo();
		/*set request transfer data step(0x34 service)*/
		Flash_SetNextDownloadStep(FL_REQUEST_STEP);
		gs_RxBlockNum = 0u;
		return;

}

/*request transfer exit*/
static void UDS_RequestTransferExit(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {
	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	if (Flash_GetCurDownloadStep() != FL_EXIT_TRANSFER_STEP) {
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RSE, m_pstPDUMsg);
		APP_DebugPrintf(LOG_LEVEL_ERROR, "Download is not at exit step. Aborting download.\n");
		Flash_InitDowloadInfo();
		/*
		 * The code below is perhaps necessary. I didn't delete it, but I am not adding it before being sure.
		 * Flash_SetNextDownloadStep(FL_REQUEST_STEP);
		 * gsRxBlockNum = 0;
		 */
		return;
	}

	Flash_SetNextDownloadStep(FL_CHECKSUM_STEP);
	m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40;
	m_pstPDUMsg->dataLength	  = 1;
	return;
}

/*routine control*/
static void UDS_RoutineControl(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {
	uint8  ret			= false;
	uint32 ReceivedCrc	= 0u;
	uint8  aSWVersion[] = BOOTLOADER_SW_VERSION;
	uint8  aHWVersion[] = BOOTLOADER_HW_VERSION;
	uint8  offset		= 0u;

	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	UDS_RestartS3Server();

	/*Is erase memory routine control?*/
	if (true == UDS_IsEraseMemoryRoutineControl(m_pstPDUMsg)) {
		/*request client timeout time*/
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RCRRP, m_pstPDUMsg);

		m_pstPDUMsg->serviceFunctionCallback = &UDS_DoEraseFlash;
	}
	/*Is check sum routine control?*/
	else if (true == UDS_IsCheckSumRoutineControl(m_pstPDUMsg)) {
		ReceivedCrc = m_pstPDUMsg->payload[4u];
		ReceivedCrc = (ReceivedCrc << 8u) | m_pstPDUMsg->payload[5u];
		ReceivedCrc = (ReceivedCrc << 8u) | m_pstPDUMsg->payload[6u];
		ReceivedCrc = (ReceivedCrc << 8u) | m_pstPDUMsg->payload[7u];
		Flash_SavedReceivedCheckSumCrc(ReceivedCrc);

		/*request client timeout time*/
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RCRRP, m_pstPDUMsg);

		m_pstPDUMsg->serviceFunctionCallback = &UDS_DoCheckSum;
	}

	/*Is check programming dependency?*/
	else if (true == UDS_IsCheckProgrammingDependency(m_pstPDUMsg)) {
		/*Fill response*/
		m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;
		m_pstPDUMsg->dataLength	  = 5u;

		/*write application information in flash.*/
		ret = Flash_WriteFlashAppInfo();
		if (ret) {
			/*do check programming dependency*/
			ret = UDS_DoCheckProgrammingDependency();
			m_pstPDUMsg->payload[4u] = 0u;
		} else {
			m_pstPDUMsg->payload[4u] = 1u;

			UDS_DebugPrintf(LOG_LEVEL_ERROR, "%s: Write APP info or check dependency failed!\n", __func__);
		}
	}

	/*Is get version*/
	else if (true == UDS_IsGetVersion(m_pstPDUMsg)) {
		m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;
		m_pstPDUMsg->dataLength	  = 4u;

		/*fill sofware information*/
		offset						  = m_pstPDUMsg->dataLength;
		m_pstPDUMsg->payload[offset] = sizeof(aSWVersion);
		offset += 1u;

		fsl_memcpy(&m_pstPDUMsg->payload[offset], aSWVersion, sizeof(aSWVersion));
		offset += sizeof(aSWVersion);

		/*fill hardware version*/
		m_pstPDUMsg->payload[offset] = sizeof(aHWVersion);
		offset += 1u;

		fsl_memcpy(&m_pstPDUMsg->payload[offset], aHWVersion, sizeof(aHWVersion));
		offset += sizeof(aHWVersion);

		m_pstPDUMsg->dataLength = offset;
	}

	else {
		/*don't have this routine control ID*/
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SFNS, m_pstPDUMsg);
	}
}

/*reset ECU*/
static void UDS_ResetECU(UDSService *i_pstUDSServiceInfo, UDSMessage *m_pstPDUMsg) {
	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	/*If program data in flash successful, set Bootloader will jump to
	 * application flag*/
	Flash_EraseFlashDriverInRAM();

	/*If invalid application software in flash, then this step set application
	 * jump to bootloader flag*/
	Boot_SetDownloadAppSuccessful();

	m_pstPDUMsg->serviceFunctionCallback = &UDS_DoResetMCU;

	/*request client timeout time*/
	UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_RCRRP, m_pstPDUMsg);
}

/*Tester present service*/
static void UDS_TesterPresent(UDSService *	  i_pstUDSServiceInfo, // pst -> pointer to structure
							  UDSMessage *m_pstPDUMsg) {
	uint8 requestSubfunction = 0u;

	ASSERT(NULL_PTR == m_pstPDUMsg);
	ASSERT(NULL_PTR == i_pstUDSServiceInfo);

	requestSubfunction = m_pstPDUMsg->payload[1u];

	/*sub function*/
	switch (requestSubfunction) {
	case 0x00u: /*zero subFunction*/
		/*set send postive message*/
		m_pstPDUMsg->payload[0u] = i_pstUDSServiceInfo->SID + 0x40u;
		m_pstPDUMsg->payload[1u] = requestSubfunction;
		m_pstPDUMsg->dataLength	  = 2u;
		break;

	case 0x80u: /*program mode*/
		m_pstPDUMsg->dataLength = 0u;
		break;

	default:
		UDS_SetNRC(i_pstUDSServiceInfo->SID, NRC_SFNS, m_pstPDUMsg);
		break;
	}
}

/* MCU hard reset by Watchdog */
static void UDS_DoResetMCU(uint8 Txstatus) {
	if (TX_MSG_SUCCESSFUL == Txstatus) {
		/*reset ECU*/
		WATCHDOG_HAL_SystemRest();
		while (true) {
			/*wait watch dog reset mcu*/
		}
	}
}

/**********************UDS service correlation other function
 * realizing************************/
const UDSService *UDS_GetUDSServiceInfo(uint8 *size_buffer) {
	ASSERT(NULL_PTR == size_buffer);

	*size_buffer = sizeof(gs_astUDSService) / sizeof(gs_astUDSService[0u]);

	return gs_astUDSService;
}

/* If Rx UDS msg, set UDS layer received message TURE */
void UDS_SetIsRxUdsMsg(const bool i_setValue) {
#ifdef EN_DELAY_TIME
	gs_stJumpAPPDelayTimeInfo.isReceiveUDSMsg = i_setValue;
#endif
}

bool UDS_IsRxUdsMsg(void) {
#ifdef EN_DELAY_TIME
	return gs_stJumpAPPDelayTimeInfo.isReceiveUDSMsg;
#else
	return true;
#endif
}

/* Send a negative response with the specified NRC. */
void UDS_SetNRC(const byte requestSID, NRC nrc, UDSMessage *m_pstPDUMsg) {
	ASSERT(NULL_PTR == m_pstPDUMsg);

	m_pstPDUMsg->payload[0u] = NEGATIVE_SID;
	m_pstPDUMsg->payload[1u] = requestSID;
	m_pstPDUMsg->payload[2u] = nrc;
	m_pstPDUMsg->dataLength	  = 3;
}

/*Is current session DEFAULT return true, else return false.*/
bool UDS_IsCurDefaultSession(void) {
	return serverState.sessionMode == DEFAULT_SESSION;
}

/*Is S3server timeout?*/
bool UDS_IsS3ServerTimeout(void) {
	return serverState.S3ServerTime == 0;
}

/*Is current session can request?*/
bool UDS_IsCurSessionCanRequest(byte i_sessionMode) {
	// Check if all bits in curSessionMode are set in i_sessionMode
	return (i_sessionMode & serverState.sessionMode) == serverState.sessionMode;
}

/*save received request id. If received physical/function/none physical and function
 * ID set received physical/function/error ID.*/
bool UDS_SetRequestType(const tUdsId clientID) {
	if (clientID == TP_GetConfigRxMsgPHYID()) {
		serverState.requestAddressingMode = SUPPORT_PHYSICAL_ADDR;
		return true;
	}
	if (clientID == TP_GetConfigRxMsgFUNID()) {
		serverState.requestAddressingMode = SUPPORT_FUNCTION_ADDR;
		return true;
	}
	serverState.requestAddressingMode = ERROR_REQUEST_ID;
	return false;
}

/**
 * Is the request originating from an ID who is allowed to make requests?
 */
bool UDS_IsCurRxIdCanRequest(byte i_serRequestIdMode) {
	// Check if all the bits in requestIdMode are set in i_serRequestIdMode
	return (i_serRequestIdMode & serverState.requestAddressingMode) == serverState.requestAddressingMode;
}

/*set security level*/
void UDS_SetSecurityLevel(const byte i_setSecurityLevel) { serverState.securityLevel = i_setSecurityLevel; }

/*Is current security level can request?*/
bool UDS_IsCurSecurityLevelRequest(byte i_securityLevel) {
	// Check if any bits set in gs_stUdsInfo.securityLevel are also set in i_securityLevel
	return (i_securityLevel & serverState.securityLevel) != 0;
}

/**********************UDS service correlation subfunction
 * realizing************************/
/*app memcopy*/
static void UDS_AppMemcopy(const void *i_pvSource, const uint16 i_CopyLen, void *o_pvDest) {
	ASSERT(NULL_PTR == i_pvSource);
	ASSERT(NULL_PTR == o_pvDest);

	fsl_memcpy(o_pvDest, i_pvSource, i_CopyLen);
}

/*app memset*/
static void UDS_AppMemset(const byte i_SetValue, const uint16 i_Len, void *m_pvSource) {
	ASSERT(NULL_PTR == m_pvSource);

	fsl_memset(m_pvSource, i_SetValue, i_Len);
}

/*check random is right?*/
static bool UDS_IsReceivedKeyRight(const byte *i_pReceivedKey, const byte *i_pTxSeed, const byte keyLen) {
	ASSERT(NULL_PTR == i_pReceivedKey);
	ASSERT(NULL_PTR == i_pTxSeed);

	uint8 index					= 0u;
	byte aPlainText[SEC_PARAM] = {0u};
	UDS_ALG_HAL_DecryptData(i_pReceivedKey, keyLen, aPlainText);

	while (index < SEC_PARAM) {
		if (aPlainText[index] != i_pTxSeed[index]) {
			return false;
		}
		index++;
	}
	return true;
}

static bool UDS_IsCheckRoutineControlRight(const tCheckRoutineCtlInfo i_eCheckRoutineCtlId,
											const UDSMessage *	   m_pstPDUMsg) {
	ASSERT(NULL_PTR == m_pstPDUMsg);

	int  findCnt;
	const byte *pDestRoutineCltId;

	switch (i_eCheckRoutineCtlId) {
	case ERASE_MEMORY_ROUTINE_CONTROL:
		pDestRoutineCltId = gs_aEraseMemoryRoutineControlId;
		findCnt = sizeof(gs_aEraseMemoryRoutineControlId);
		break;

	case CHECK_SUM_ROUTINE_CONTROL:
		pDestRoutineCltId = gs_aCheckSumRoutineControlId;
		findCnt = sizeof(gs_aCheckSumRoutineControlId);
		break;

	case CHECK_DEPENDENCY_ROUTINE_CONTROL:
		pDestRoutineCltId = gs_aCheckProgrammingDependencyId;
		findCnt = sizeof(gs_aCheckProgrammingDependencyId);
		break;

	case GET_VERSION:
		pDestRoutineCltId = gs_aGetVersion;
		findCnt			  = sizeof(gs_aGetVersion);
		break;

	default:
		return false;
	}

	if ((NULL_PTR == pDestRoutineCltId) || (m_pstPDUMsg->dataLength < findCnt)) {
		return false;
	}

	for (int i = 0; i < findCnt; i++) {
		if (m_pstPDUMsg->payload[i] != pDestRoutineCltId[i]) {
			return false;
		}
	}

	return true;

}

static bool UDS_IsEraseMemoryRoutineControl(const UDSMessage *m_pstPDUMsg) {
	return UDS_IsCheckRoutineControlRight(ERASE_MEMORY_ROUTINE_CONTROL, m_pstPDUMsg);
}

static bool UDS_IsCheckSumRoutineControl(const UDSMessage *m_pstPDUMsg) {
	return UDS_IsCheckRoutineControlRight(CHECK_SUM_ROUTINE_CONTROL, m_pstPDUMsg);
}

static bool UDS_IsCheckProgrammingDependency(const UDSMessage *m_pstPDUMsg) {
	return UDS_IsCheckRoutineControlRight(CHECK_DEPENDENCY_ROUTINE_CONTROL, m_pstPDUMsg);
}

static bool UDS_IsGetVersion(const UDSMessage *m_pstPDUMsg) {
	return UDS_IsCheckRoutineControlRight(GET_VERSION, m_pstPDUMsg);
}

/*Is write fingerprint right?*/
static bool UDS_IsWriteFingerprintRight(const UDSMessage *m_pstPDUMsg) {
	ASSERT(NULL_PTR == m_pstPDUMsg);

	uint8 WriteFingerprintIdLen = sizeof(gs_aWriteFingerprintId);

	if (m_pstPDUMsg->dataLength < WriteFingerprintIdLen) {
		return false;
	}

	for (int i = 0; i < WriteFingerprintIdLen; i++) {
		if (m_pstPDUMsg->payload[i] != gs_aWriteFingerprintId[i]) {
			return false;
		}
	}

	return true;
}

/*Is download data address valid?*/
static bool UDS_IsDownloadDataAddrValid(const uint32 i_dataAddr, const uint32 i_dataLen) {
	// TODO: This is deeply flawed. I will change this after I make sense of it.
	bool	 res			= false;
	tAPPType requestAPPType = APP_INVLID_TYPE;

	if (Flash_IsFlashDriverData(i_dataAddr, i_dataLen)) {
		return true;
	}

	requestAPPType = APP_A_TYPE;
	if (!Flash_IsValidAddr(APP_A_TYPE, i_dataAddr, i_dataLen)) {
		return false;
	}

	if (!Flash_IsAPPTypeErased(requestAPPType)) {
		FLS_DebugPrintf(LOG_LEVEL_ERROR, "APP Type(%d) is not erased!\n", requestAPPType);
		return false;
	}


	if (Flash_IsFlashDriverData(i_dataAddr, i_dataLen)) {
		res = true;
	} else {
		requestAPPType = APP_A_TYPE;
		res			   = Flash_IsValidAddr(APP_A_TYPE, i_dataAddr, i_dataLen);
#ifdef EN_SUPPORT_APP_B
		if (true != res) {
			requestAPPType = APP_B_TYPE;
			res			   = Flash_IsValidAddr(APP_B_TYPE, i_dataAddr, i_dataLen);
		}
#endif
		if (res) {
			res = Flash_IsAPPTypeErased(requestAPPType);
			if (!res) {
				FLS_DebugPrintf(LOG_LEVEL_ERROR, "APP Type(%d) is not erased!\n", requestAPPType);
			}
		}
	}

	return res;
}

typedef void (*tpfFlashOperateMoreTimecallback)(uint8);

/* For erasing or programming flash were timeout callback */
static tpfFlashOperateMoreTimecallback gs_pfFlashOperateMoreTimecallback = NULL_PTR;

static void RequestMoreTimeCallback(byte i_TxStatus) {
	if (TX_MSG_SUCCESSFUL == i_TxStatus) {
		UDS_RestartS3Server();
	}

	if (NULL_PTR != gs_pfFlashOperateMoreTimecallback) {
		gs_pfFlashOperateMoreTimecallback(i_TxStatus);
		gs_pfFlashOperateMoreTimecallback = NULL_PTR;
	}
}

static void UDS_RequestMoreTime(const byte UDSServiceID, void (*pcallback)(byte)) {
	ASSERT(NULL_PTR == pcallback);

	UDSMessage stMsgBuf = {0};

	stMsgBuf.sourceID = TP_GetConfigTxMsgID();
	UDS_SetNRC(UDSServiceID, NRC_RCRRP, &stMsgBuf);
	stMsgBuf.serviceFunctionCallback = &RequestMoreTimeCallback;
	gs_pfFlashOperateMoreTimecallback  = pcallback;

	(void)TP_WriteAFrameDataInTP(stMsgBuf.sourceID, stMsgBuf.serviceFunctionCallback, stMsgBuf.dataLength,
								 stMsgBuf.payload);
}

/*do check sum. If check sum right return true, else return false.*/
static void UDS_DoCheckSum(byte TxStatus) {
	if (TX_MSG_SUCCESSFUL == TxStatus) {
		/*need request client delay time for flash checking flash data*/
		Flash_SetOperateFlashActiveJob(FLASH_CHECKING, &UDS_DoResponseChecksum, 0x31u, &UDS_RequestMoreTime);
	}
}

/*do response checksum*/
static void UDS_DoResponseChecksum(byte i_Status) {
	byte  aResponseBuf[8u] = {0u};
	uint32  TxDataLen		= 0u;

	TxDataLen		 = sizeof(gs_aCheckSumRoutineControlId) / sizeof(gs_aCheckSumRoutineControlId[0u]);
	aResponseBuf[0u] = gs_aCheckSumRoutineControlId[0u] + 0x40u;

	for (int i = 1; i < TxDataLen; i++) {
		/* TODO: There are several such loops, which should be memcpy instead. */
		aResponseBuf[i] = gs_aCheckSumRoutineControlId[i];
	}

	if (i_Status) {
		aResponseBuf[TxDataLen] = 0u;
	} else {
		aResponseBuf[TxDataLen] = 1u;
		UDS_DebugPrintf(LOG_LEVEL_ERROR, "Check CRC failed!\n");
	}

	(void)TP_WriteAFrameDataInTP(TP_GetConfigTxMsgID(), NULL_PTR, TxDataLen + 1, aResponseBuf);
}

/*do erase flash response*/
static void UDS_DoEraseFlashResponse(byte i_Status) {
	byte  aResponseBuf[8u] = {0u};
	uint8 TxDataLen		 = sizeof(gs_aEraseMemoryRoutineControlId) / sizeof(gs_aEraseMemoryRoutineControlId[0u]);
	aResponseBuf[0u] = gs_aEraseMemoryRoutineControlId[0u] + 0x40u;

	for (int i = 1; i < TxDataLen; i++) {
		aResponseBuf[i] = gs_aEraseMemoryRoutineControlId[i];
	}

	if (i_Status) {
		aResponseBuf[TxDataLen] = 0u;
	} else {
		aResponseBuf[TxDataLen] = 1u;
	}

	(void)TP_WriteAFrameDataInTP(TP_GetConfigTxMsgID(), NULL_PTR, TxDataLen + 1, aResponseBuf);
}

/*do erase flash*/
static void UDS_DoEraseFlash(byte TxStatus) {
	if (TX_MSG_SUCCESSFUL == TxStatus) {
		/*do erase flash need request client delay timeout*/
		Flash_SetOperateFlashActiveJob(FLASH_ERASING, &UDS_DoEraseFlashResponse, 0x31, &UDS_RequestMoreTime);
	}
}

/*do check programming dependency*/
static bool UDS_DoCheckProgrammingDependency(void) {
	return Flash_IsReadAppInfoFromFlashValid() && Flash_IsAppInFlashValid();
}

static bool insertDataIntoOutput(byte *arr, size_t *start, byte *data, uint16 data_length) {
	if (arr == NULL || start == NULL || (data_length == 0 && data == NULL)) {
		APP_DebugPrintf(LOG_LEVEL_ERROR, "Inserting data into array failed. NULL input parameter.\n");
		return false;
	}

	byte msb, lsb;
	if (!uint16_to_bytes(data_length, &msb, &lsb)) {
		APP_DebugPrintf(LOG_LEVEL_ERROR, "Inserting data into array failed.\n");
		return false;
	}

	arr[(*start)++] = msb;
	arr[(*start)++] = lsb;
	UDS_AppMemcopy(data, data_length, &arr[*start]);
	*start += data_length;

	return true;
}

static bool readDataFromInput(byte *arr, size_t *start, byte *data_buffer, uint16 *data_length) {
	if (arr == NULL || start == NULL || data_buffer == NULL || data_length == NULL) {
		APP_DebugPrintf(LOG_LEVEL_ERROR, "Reading data from array failed. NULL input parameter(s).\n");
		return false;
	}

	byte msb = arr[(*start)++];
	byte lsb = arr[(*start)++];

	*data_length = bytes_to_uint16(msb, lsb);
	UDS_AppMemcopy(&arr[*start], *data_length, data_buffer);
	*start += *data_length;

	return true;
}

// Parse the BER encoded OID value
static AuthAlg parseAlgorithm(byte data[16]) {
	// TODO: BER Encoding of OID values
	return AES_256;
}

// XXX: Currently generates 16 bytes of randomness.
static bool AuthChalGen(byte *challenge, uint16 challenge_len) {
	// TODO: Challenge creation according to ISO/IEC 9798-2 or 9798-4
	return UDS_ALG_HAL_GetRandom(challenge_len, challenge);
}

// XXX: Currently returns POWN == 1
static bool verifyClientPOWN(AuthAlg alg, byte *client_pown, uint16 client_pown_len, byte *client_chal,
								uint16 client_chal_len) {
	// TODO: Arrive here with Auth-POWN and Auth-VPOWN. Handle alg = NO_ALG and chal = NULL cases.
	return client_pown_len == 1u && client_pown[0] == 1u;
}
// XXX: Currently checks cert == 1
static byte verifyClientCert(byte *client_cert, uint16 client_cert_len, byte *client_chal, uint16 client_chal_len) {
	// TODO: Return 0 on success, NRC on failure.
	return !(client_cert_len == 1u && client_chal[0u] == 1u);
}

static bool genServerPOWN(byte *server_pown_buffer, uint16 *server_pown_len, byte *client_chal,
							 uint16 client_chal_len, byte *eph_pub_key, uint16 eph_pub_key_len) {
	// TODO
	server_pown_buffer[0] = 1u;
	*server_pown_len	  = 1u;
	return TRUE;
}

static bool readServerCert(byte *server_cert_buffer, uint16 *server_cert_len) {
	// TODO
	server_cert_buffer[0] = 1u;
	*server_cert_len	  = 1u;
	return TRUE;
}

static bool genSessionKeyInfo(byte *session_key_info_buffer, uint16 session_key_info_len) {
	// TODO
	return true;
}
static bool genEphKeys(byte *eph_pub_key_buffer, uint16 eph_pub_key_len, byte *eph_priv_key_buffer,
						  uint16 eph_priv_key_len) {
	// TODO
	return TRUE;
}

static AuthRetPar getAuthRetPar(AuthAlg alg) {
	switch (alg) {
	case AES_256:
		return ACACRSC;
	default:
		return ACACRSC;
	}
}

/*********************************************************/
/**********************UDS service other module call function realizing************************/

/*transmitted confirm message callback*/
static void UDS_TXConfrimMsgCallback(byte i_status) {
	if (TX_MSG_SUCCESSFUL == i_status) {
		UDS_SetCurrentSession(PROGRAMMING_SESSION);
		UDS_SetSecurityLevel(NONE_SECURITY);

		/*restart s3server time*/
		UDS_RestartS3Server();
	}
}

/*write message to host basd on UDS for request enter bootloader mode*/
bool UDS_TxMsgToHost(void) {
	UDSMessage stUdsAppMsg = {0u, 0u, {0u}, NULL_PTR};
	bool		   ret		   = false;

	stUdsAppMsg.sourceID					  = TP_GetConfigTxMsgID();
	stUdsAppMsg.dataLength				  = 2;
	stUdsAppMsg.payload[0u]			  = 0x50u;
	stUdsAppMsg.payload[1u]			  = 0x02u;
	stUdsAppMsg.serviceFunctionCallback = UDS_TXConfrimMsgCallback;

	ret = TP_WriteAFrameDataInTP(stUdsAppMsg.sourceID, stUdsAppMsg.serviceFunctionCallback, stUdsAppMsg.dataLength,
								 stUdsAppMsg.payload);

	return ret;
}

/*uds time control*/
void UDS_SystemTickCtl(void) {
	if (UDS_GetUdsS3ServerTime()) {
		UDS_SubUdsS3ServerTime(1u);
	}

	if (UDS_GetUdsSecurityReqLockTime()) {
		UDS_SubUdsSecurityReqLockTime(1u);
	}

#ifdef EN_DELAY_TIME
	if (true != UDS_IsRxUdsMsg()) {
		if (gs_stJumpAPPDelayTimeInfo.jumpToAPPDelayTime) {
			gs_stJumpAPPDelayTimeInfo.jumpToAPPDelayTime--;
		} else {
			/*max timeout time out*/
			Boot_JumpToAppOrNot();
		}
	}
#endif
}

/***************************End file********************************/
