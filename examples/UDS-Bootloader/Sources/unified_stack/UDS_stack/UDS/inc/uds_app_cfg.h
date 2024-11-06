#ifndef __UDS_APP_CFG_H__
#define __UDS_APP_CFG_H__

/***** Include statements *****/
#include "TP.h"
#include "includes.h"
#include "boot.h"
#include "fls_app.h"
#include "uds_alg_hal.h"
#include "watchdog_hal.h"
#include "utils.h"

/***** Versioning Information *****/
#define BOOTLOADER_TYPE (0x0Fu)
#define BOOTLOADER_SW_VERSION                                                                                          \
	{ BOOTLOADER_TYPE, 0x02u, 0x0u, 0x00u }
#define BOOTLOADER_HW_VERSION                                                                                          \
	{ BOOTLOADER_TYPE, 0x02u, 0x0u, 0x00u }

/************ Timer Definitions ************/
#ifndef S3_TIMER_WATERMARK_PERCENT
#define S3_TIMER_WATERMARK_PERCENT (90u)
#endif

#if (S3_TIMER_WATERMARK_PERCENT <= 0) || (S3_TIMER_WATERMARK_PERCENT >= 100)
#error "S3_TIMER_WATERMARK_PERCENT must be between 1 and 100, exclusive."
#endif

/***** UDS Structure Definitions *****/

#define UDS_PAYLOAD_LIMIT (150u)

typedef uint16 tUdsTime; // TODO: Rename

typedef struct {
	tUdsId	sourceID;
	tUdsLen dataLength;
	byte	payload[UDS_PAYLOAD_LIMIT];
	/*tx message call back*/
	void (*serviceFunctionCallback)(byte);
} UDSMessage;

typedef struct UDSServiceInfo {
	byte SID;		   // Service Identifier; e.g. 0x10, 0x27
	byte sessionMode; /*default session / program session / extend session*/
	byte supReqMode;  /*support physical / function addr*/
	byte reqLevel;	   /*request level.Lock/unlock*/
	void (*serviceHandler)(struct UDSServiceInfo *, UDSMessage *);
} UDSService;

typedef struct {
	byte *pub_key;
	uint16 pub_key_len;
	byte *priv_key;
	uint16 priv_key_len;
} EphKeys;

typedef struct {
	UDSService* table;
	uint8 size;
} UDSLookupTable;

typedef enum {
	REQUEST_ACCEPTED = 0x0u,
	RA				 = REQUEST_ACCEPTED,

	GENERAL_REJECT = 0x1u,
	GR			   = GENERAL_REJECT,

	AUTHENTICATION_CONFIGURATION_APCE = 0x02u,
	ACAPCE							  = AUTHENTICATION_CONFIGURATION_APCE,

	AUTHENTICATION_CONFIGURATION_ACR_WITH_ASYMMETRIC_CRYPTOGRAPHY = 0x03u,
	ACACRAC = AUTHENTICATION_CONFIGURATION_ACR_WITH_ASYMMETRIC_CRYPTOGRAPHY,

	AUTHENTICATION_CONFIGURATION_ACR_WITH_SYMMETRIC_CRYPTOGRAPHY = 0x04u,
	ACACRSC = AUTHENTICATION_CONFIGURATION_ACR_WITH_SYMMETRIC_CRYPTOGRAPHY,

	// 0x05 to 0x0F ISO SAE Reserved

	DE_AUTHENTICATION_SUCCESSFUL = 0x10u,
	DAS							 = DE_AUTHENTICATION_SUCCESSFUL,

	CERTIFICATE_VERIFIED_OWNERSHIP_VERIFICATION_NECESSARY = 0x11u,
	CVOVN												  = CERTIFICATE_VERIFIED_OWNERSHIP_VERIFICATION_NECESSARY,

	OWNERSHIP_VERIFIED_AUTHENTICATION_COMPLETE = 0x12u,
	OVAC									   = OWNERSHIP_VERIFIED_AUTHENTICATION_COMPLETE,

	CERTIFICATE_VERIFIED = 0x13u,
	CV					 = CERTIFICATE_VERIFIED,

	// 0x14 to 0x9F ISO SAE Reserved
	// 0xA0 to 0xCF Vehicle Manufacturer Specific
	// 0xD0 to 0xFE System Supplier Specific
	// 0xFF ISO SAE Reserved
} AuthRetPar; // Authentication Return Parameter



/**************** NEGATIVE RESPONSE CODES ****************/

#define NEGATIVE_SID (0x7Fu) // Negative response identifier

typedef enum {
    /* General reject */
    NRC_GR = 0x10,

    /* Service not supported */
    NRC_SNS = 0x11,

    /* Subfunction not supported */
    NRC_SFNS = 0x12,

    /* Incorrect message length or invalid format */
    NRC_IMLOIF = 0x13,

    /* Response too long */
    NRC_RTL = 0x14,

    /* 0x15 to 0x20 ISOSAE Reserved */

    /* Busy, repeat request */
    NRC_BRR = 0x21,

    /* Conditions not correct */
    NRC_CNC = 0x22,

    /* 0x23 ISOSAE Reserved */

    /* Request sequence error */
    NRC_RSE = 0x24,

    /* No response from subnet component */
    NRC_NRFSC = 0x25,

    /* Failure prevents execution of requested action */
    NRC_FPEORA = 0x26,

    /* 0x27 to 0x30 ISOSAE Reserved */

    /* Request out of range */
    NRC_ROOR = 0x31,

    /* 0x32 ISOSAE Reserved */

    /* Security access denied */
    NRC_SAD = 0x33,

    /* Authentication required */
    NRC_AR = 0x34,

    /* Invalid key */
    NRC_IK = 0x35,

    /* Exceed number of attempts */
    NRC_ENOA = 0x36,

    /* Required time delay not expired */
    NRC_RTDNE = 0x37,

    /* Secure data transmission required */
    NRC_SDTR = 0x38,

    /* Secure data transmission not allowed */
    NRC_SDTNA = 0x39,

    /* Secure data transmission failed */
    NRC_SDTF = 0x3A,

    /* 0x3B to 0x4F ISOSAE Reserved */

    /* CVF (Certificate verification failed) - Invalid time period */
    NRC_CVF_ITP = 0x50,

    /* CVF - Invalid signature */
    NRC_CVF_IS = 0x51,

    /* CVF - Invalid chain of trust */
    NRC_CVF_ICOT = 0x52,

    /* CVF - Invalid type */
    NRC_CVF_IT = 0x53,

    /* CVF - Invalid format */
    NRC_CVF_IF = 0x54,

    /* CVF - Invalid content */
    NRC_CVF_IC = 0x55,

    /* CVF - Invalid scope (Original mnemonic is CVFIS) */
    NRC_CVF_ISC = 0x56,

    /* CVF - Invalid certificate (revoked) (Original mnemonic is CVFIC) */
    NRC_CVF_ICR = 0x57,

    /* Ownership verification failed */
    NRC_OVF = 0x58,

    /* Challenge calculation failed */
    NRC_CCF = 0x59,

    /* Setting access rights failed */
    NRC_SARF = 0x5A,

    /* Session key creation/derivation failed */
    NRC_SKCDF = 0x5B,

    /* Configuration data usage failed */
    NRC_CDUF = 0x5C,

    /* DeAuthentication failed */
    NRC_DAF = 0x5D,

    /* 0x5E to 0x6F ISOSAE Reserved */

    /* Upload/download not accepted */
    NRC_UDNA = 0x70,

    /* Transfer data suspended */
    NRC_TDS = 0x71,

    /* General programming failure */
    NRC_GPF = 0x72,

    /* Wrong block sequence counter */
    NRC_WBSC = 0x73,

    /* 0x74 to 0x77 ISOSAE Reserved */

    /* Request correctly received, response pending */
    NRC_RCRRP = 0x78,

    /* 0x79 to 0x7D ISOSAE Reserved */

    /* SubFunction not supported in active session */
    NRC_SFNSIAS = 0x7E,

    /* Service not supported in active session */
    NRC_SFSIAS = 0x7F,

    /* 0x80 ISOSAE Reserved */

    /* RPM too high */
    NRC_RPMTH = 0x81,

    /* RPM too low */
    NRC_RPMTL = 0x82,

    /* Engine is running */
    NRC_EIR = 0x83,

    /* Engine is not running */
    NRC_EINR = 0x84,

    /* Engine run time too low */
    NRC_ERTTL = 0x85,

    /* Temperature too high */
    NRC_TEMPTH = 0x86,

    /* Temperature too low */
    NRC_TEMPTL = 0x87,

    /* Vehicle speed too high */
    NRC_VSTH = 0x88,

    /* Vehicle speed too low */
    NRC_VSTL = 0x89,

    /* Throttle/pedal too high */
    NRC_TPTH = 0x8A,

    /* Throttle/pedal too low */
    NRC_TPTL = 0x8B,

    /* Transmission range not in neutral */
    NRC_TRNIN = 0x8C,

    /* Transmission range not in gear */
    NRC_TRNIG = 0x8D,

    /* 0x8E ISOSAE Reserved */

    /* Brake switch(es) not closed */
    NRC_BSNC = 0x8F,

    /* Shifter lever not in park */
    NRC_SLNIP = 0x90,

    /* Torque converter clutch locked */
    NRC_TCCL = 0x91,

    /* Voltage too high */
    NRC_VTH = 0x92,

    /* Voltage too low */
    NRC_VTL = 0x93,

    /* Resource Temporarily not available */
    NRC_RTNA = 0x94,

    /* 0x9F to 0xEF Reserved by ISO 14229-1-2020 for future definition */

    /* 0xF0 to 0xFE Vehicle manufacturer specific */

    /* 0xFF ISOSAE Reserved */

} NRC;


/*********** Diagnostic Session Modes ***********/
#define DEFAULT_SESSION (1u << 0u) // Default session
#define PROGRAMMING_SESSION (1u << 1u) // Programming Session
#define EXTENDED_SESSION	(1u << 2u) // Extended session

/*********** Security Levels *************/
#define NONE_SECURITY	 (1u << 0u) /*none security can request*/
#define SECURITY_LEVEL_1 ((1 << 1u) | NONE_SECURITY) /*security level 1 request*/
#define SECURITY_LEVEL_2 ((1u << 2u) | SECURITY_LEVEL_1) /*security level 2 request*/

#define DOWLOAD_DATA_ADDR_LEN (4u) /*dowload data addr len*/
#define DOWLOAD_DATA_LEN	  (4u) /*dowload data len*/

/*support function/physical ID request*/
#define ERROR_REQUEST_ID		  (0u) /*received ID failled*/
#define SUPPORT_PHYSICAL_ADDR (1u << 0u) /*support physical ID request */
#define SUPPORT_FUNCTION_ADDR (1u << 1u) /*support function ID request*/

/*********************************************************/

/*set currrent session mode. DEFAULT_SESSION/PROGRAM_SESSION/EXTEND_SESSION */
extern void UDS_SetCurrentSession(const byte i_setSessionMode);

/*Is current session DEFAULT return TRUE, else return FALSE.*/
extern bool UDS_IsCurDefaultSession(void);

/*Is S3server timeout?*/
extern bool UDS_IsS3ServerTimeout(void);

/*restart s3server time*/
extern void UDS_RestartS3Server(void);

/*Is current session can request?*/
extern bool UDS_IsCurSessionCanRequest(byte i_sessionMode);

/*save received request id. If receved physical/function/none phy
 and function ID set rceived physicali/function/erro ID.*/
extern bool UDS_SetRequestType(const uint32 clientID);

/*Is current received id can request?*/
extern bool UDS_IsCurRxIdCanRequest(byte i_serRequestIdMode);

/*set security level*/
extern void UDS_SetSecurityLevel(const byte i_setSecurityLevel);

/*Is current security level can request?*/
extern bool UDS_IsCurSecurityLevelRequest(byte i_securityLevel);

/* Getter for UDS Services lookup table */
const UDSService *UDS_GetUDSServiceInfo(byte *m_pSupServItem);

/* If Rx UDS msg, set g_ucIsRxUdsMsg TURE */
extern void UDS_SetIsRxUdsMsg(const bool i_setValue);

extern bool UDS_IsRxUdsMsg(void);

/*set negative erro code*/
extern void UDS_SetNRC(const uint8 i_UDSServiceNum, const uint8 i_errorCode, UDSMessage *m_pstPDUMsg);

/*uds time control*/
extern void UDS_SystemTickCtl(void);

/*get UDS s3 watermark timer. return s3 * 5 * 1000/ 8*/
extern uint32 UDS_GetUDSS3WatermarkTimerMs(void);

/*write message to host basd on UDS for request enter bootloader mode*/
extern bool UDS_TxMsgToHost(void);

#endif /*__UDS_APP_CFG_H__*/
/***************************End file********************************/
