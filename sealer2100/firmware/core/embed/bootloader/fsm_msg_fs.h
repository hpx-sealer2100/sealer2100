#ifndef __FSM_MSG_EMMC_H__
#define __FSM_MSG_EMMC_H__
#include "messages-fs.pb.h"

void fsm_msgFsStat(const FsStat* msg);
void fsm_msgFsFsStat(const FsFsStat *msg);
void fsm_msgFsMkdir(const FsMkdir* msg);
void fsm_msgFsRemove(const FsRemove* msg);
void fsm_msgFsRead(const FsRead* msg);
void fsm_msgFsWrite(const FsWrite* msg);
void fsm_msgFsChecksums(const FsChecksums* msg);

// legacy
void fsm_msgFsFileRead(const FsFileRead* msg);
void fsm_msgFsFileWrite(const FsFileWrite* msg);
#endif
