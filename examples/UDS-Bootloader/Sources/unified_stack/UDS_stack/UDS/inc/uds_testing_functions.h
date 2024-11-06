#ifndef __UDS_TESTING_FUNCTIONS__
#define __UDS_TESTING_FUNCTIONS__

#include "test_messages.h"

#include "uds_app_cfg.h"
#include "S32K144.h"

void simulateIncomingMessage(UDSMessage * incomingMessageSimulation);

void initSW2Button();

void readTestMessages();

#endif
