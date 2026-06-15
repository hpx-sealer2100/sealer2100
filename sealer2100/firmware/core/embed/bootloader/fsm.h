/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FSM_H__
#define __FSM_H__
#include <assert.h>
#include <stdint.h>

#include "memzero.h"
#include "messages-common.pb.h"
#include "messages.h"

void fsm_sendSuccess(const char *text);
void fsm_sendFailure(FailureType code, const char *text);
void fsm_sendButtonRequest(ButtonRequestType type);
uint16_t fsm_pollButtonAck(void);

// macros for easy use
extern uint8_t msg_resp[MSG_OUT_DECODED_SIZE];
#define RESP_INIT(TYPE)                                                      \
  TYPE *resp = (TYPE *)(void *)msg_resp;                                     \
  do {                                                                       \
    _Static_assert(sizeof(msg_resp) >= sizeof(TYPE), #TYPE " is too large"); \
    memzero(resp, sizeof(TYPE));                                             \
  } while (0)

#define CHECK_PARAM(cond, errormsg)                               \
  do {                                                            \
    if (!(cond)) {                                                \
      fsm_sendFailure(FailureType_Failure_DataError, (errormsg)); \
      return;                                                     \
    }                                                             \
  } while (0)

#define UNSUPPORT_IN_PRODUCTION_BUILD() do {                                        \
  if (boot_ctx.build == BUILD_MODE_PRODUCTION) {                                    \
      fsm_sendFailure(FailureType_Failure_UnexpectedMessage, ("Unknown message"));  \
      return;                                                                       \
  }                                                                                 \
}while(0)

#endif  // __FSM_H__
