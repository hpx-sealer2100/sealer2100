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
#include "fsm.h"
#include <stddef.h>
#include "messages-common.pb.h"
#include "messages.h"
#include "messages.pb.h"

// message methods

uint8_t msg_resp[MSG_OUT_DECODED_SIZE] __attribute__((aligned));
size_t msg_resp_size = sizeof(msg_resp);

void fsm_sendSuccess(const char *text) {
  RESP_INIT(Success);
  if (text) {
    resp->has_message = true;
    strlcpy(resp->message, text, sizeof(resp->message));
  }
  msg_write(MessageType_MessageType_Success, resp);
}

void fsm_sendFailure(FailureType code, const char *text) {
  RESP_INIT(Failure);
  resp->has_code = true;
  resp->code = code;
  if (!text) {
    switch (code) {
      case FailureType_Failure_UnexpectedMessage:
        text = "Unexpected message";
        break;
      case FailureType_Failure_ButtonExpected:
        text = "Button expected";
        break;
      case FailureType_Failure_DataError:
        text = "Data error";
        break;
      case FailureType_Failure_ActionCancelled:
        text = "Action cancelled by user";
        break;
      case FailureType_Failure_PinExpected:
        text = "PIN expected";
        break;
      case FailureType_Failure_PinCancelled:
        text = "PIN cancelled";
        break;
      case FailureType_Failure_PinInvalid:
        text = "PIN invalid";
        break;
      case FailureType_Failure_InvalidSignature:
        text = "Invalid signature";
        break;
      case FailureType_Failure_ProcessError:
        text = "Process error";
        break;
      case FailureType_Failure_NotEnoughFunds:
        text = "Not enough funds";
        break;
      case FailureType_Failure_NotInitialized:
        text = "Device not initialized";
        break;
      case FailureType_Failure_PinMismatch:
        text = "PIN mismatch";
        break;
      case FailureType_Failure_WipeCodeMismatch:
        text = "Wipe code mismatch";
        break;
      case FailureType_Failure_InvalidSession:
        text = "Invalid session";
        break;
      case FailureType_Failure_FirmwareError:
        text = "Firmware error";
        break;
    }
  }
  if (text) {
    resp->has_message = true;
    strlcpy(resp->message, text, sizeof(resp->message));
  }
  msg_write(MessageType_MessageType_Failure, resp);
}

void fsm_sendButtonRequest(ButtonRequestType type) {
  RESP_INIT(ButtonRequest);
  resp->has_code = type;
  resp->code = type;
  msg_write(MessageType_MessageType_ButtonRequest, resp);
  msg_tiny_id = 0xFFFF;
}
