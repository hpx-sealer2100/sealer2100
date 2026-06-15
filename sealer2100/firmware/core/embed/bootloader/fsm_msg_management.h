#ifndef __FSM_MSG_MANAGEMENT_H__
#define __FSM_MSG_MANAGEMENT_H__

#include "messages-management.pb.h"
void fsm_msgInitialize(const Initialize *msg);
void fsm_msgPing(const Ping *msg);
void fsm_msgGetFeatures(const GetFeatures *msg);
void fsm_msgEndSession(const EndSession *msg);
void fsm_msgDeviceInfoSettings(const DeviceInfoSettings *msg);
void fsm_msgReadSEPublicKey(const ReadSEPublicKey *msg);
void fsm_msgWriteSEPublicCert(const WriteSEPublicCert *msg);
void fsm_msgReadSEPublicCert(const ReadSEPublicCert *msg);
void fsm_msgSESignMessage(const SESignMessage *msg);
void fsm_msgSEInitialize(const SEInitialize *msg);
void fsm_msgSEInitializeDone(const SEInitializeDone *msg);
void fsm_msgSEBackToRomBoot(const SEBackToRomBoot *msg);
void fsm_msgSEWipeUserStorage(const SEWipeUserStorage *msg);
#endif
