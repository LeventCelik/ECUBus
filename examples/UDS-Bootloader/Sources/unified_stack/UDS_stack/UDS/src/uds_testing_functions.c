#include "uds_testing_functions.h"

void readNewTestMessage(uint32_t * client_id, uint32_t * sid, uint32_t * sbf){
	static index = 0;

	while(test_messages_txt[index] != 0x0a){

	}

}

//Client ID - the ID associated with this CAN message, most common ID is 784, ID represents urgency
//#define TEST_CLIENT_ID (0x784)

//Service Identifier - byte associated with specific UDS requests
//#define TEST_SID (0x10)

//SubFunction byte - make sure this is applicable to the SID you choose
//#define TEST_SBF (0x1)

void simulateIncomingMessage(UDSMessage * incomingMessageSimulation){

	uint32_t TEST_CLIENT_ID;
	uint32_t TEST_SID;
	uint32_t TEST_SBF;

	readNewTestMessage(&TEST_CLIENT_ID, &TEST_SID, &TEST_SBF);

	//Set simulation message's test CAN ID
	incomingMessageSimulation->sourceID = TEST_CLIENT_ID;

	//Sets contents of the request
	//!!!! SID MUST BE FIRST, FOLLOWED BY SBF (if any) !!!!!
	incomingMessageSimulation->payload[0] = TEST_SID;
	incomingMessageSimulation->payload[1] = TEST_SBF;

	//Make sure DataLen is exactly the bytes (not including the client_id)
	//CAN interprets UDS request exactly, so bytes after dataLen will be ignored
	incomingMessageSimulation->dataLength = 2;
}


//Port which detects button press,
//The "C" is mutable, change it to a letter A-E to change ports
#define BUTTON_PORT PTC

//This is the index of the port which detects button press
//Port index of 75 = PORTC
//Range of ports is 73 - 77 (A-E)
#define BUTTON_PORT_INDEX 75


//Port PTC12, bit 12: read input from button0, SW2
#define PTC12 12

void initSW2Button(){


	//Enables the clock to the port BUTTON_PORT_INDEX is set to
	PCC->PCCn[BUTTON_PORT_INDEX] = PCC_PCCn_CGC_MASK;

	//This sets the button's port as input, reading a button press
	BUTTON_PORT->PDDR &= (~1<<PTC12);
	PORTC->PCR[12] = 0x00000110;
}
