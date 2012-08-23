/*
 * Free RIL implementation for Samsung Android-based smartphones.
 * Copyright (C) 2012  Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Messages.h>
#include <limits.h>
#include <stdlib.h>

#include "RequestHandler.h"
#include "Request.h"
#include "RIL.h"

using namespace SamsungIPC;

void RequestHandler::handleCurrentCalls(Request *request) {
    Message *reply = m_ril->execute(new Messages::CallGetCallList);
    Messages::CallGetCallListReply *complete = message_cast<Messages::CallGetCallListReply>(reply);

    if(complete == NULL) {
        Log::error("Got unexpected message in response to CallGetCallList: %s", reply->inspect().c_str());

        request->complete(RIL_E_GENERIC_FAILURE);
    } else {
        m_callIds.clear();
        m_callIds.reserve(complete->count());

        size_t call_pointers_size = sizeof(RIL_Call *) * complete->count();
        RIL_Call **call_pointers = (RIL_Call **) malloc(call_pointers_size);

        for(unsigned int i = 0, count = complete->count(); i < count; i++) {
            RIL_Call *call = (RIL_Call *) malloc(sizeof(RIL_Call));
            const Messages::CallGetCallListReply::CallListItem &item = complete->callList()[i];

            char *number;
            if(item.numberLength() == 0)
                number = NULL;
            else {
                number = (char *) malloc(item.numberLength() + 1);
                number[item.numberLength()] = 0;
                memcpy(number, &item.number()[0], item.numberLength());
            }

            call->state = (RIL_CallState) std::max(0, item.callState() - 1);
            call->index = item.callId();
            m_callIds.push_back(call->index);

            if(number && number[0] == '+')
                call->toa = 145;
            else
                call->toa = 129;

            call->isMpty = item.isMultiParty();
            call->isMT = item.direction() == Messages::CallGetCallListReply::CallListItem::MT;
            call->als = 0; // not used by Android, so why bother?
            call->isVoice = item.callType() < 0x102 || item.callType() == 0x700;
            call->isVoicePrivacy = 0; // for Samsung this is:
                                        // call->isVideo = callType == 0x300
            call->number = number;
            call->numberPresentation = 0;
            call->name = NULL;
            call->namePresentation = 2;
            call->uusInfo = NULL;

            call_pointers[i] = call;
        }

        request->complete(RIL_E_SUCCESS, call_pointers, call_pointers_size);

        for(unsigned int i = 0, count = complete->count(); i < count; i++) {
            free(call_pointers[i]->number);
            free(call_pointers[i]);
        }

        free(call_pointers);
    }

    delete reply;
}

void RequestHandler::handleLastCallFailCause(Request *request) {
    request->complete(RIL_E_SUCCESS, &m_lastCallFailCause, sizeof(RIL_LastCallFailCause));
}

void RequestHandler::handleDial(Request *request) {
    RIL_Dial *dial = (RIL_Dial *) request->data();

    Log::debug("Dialing %s, clir %d", dial->address, dial->clir);

    char *dup = strdup(dial->address);
    char *extension = strchr(dup, '/');
    if(extension)
        *extension++ = 0;

    Messages::CallOriginate *originate = new Messages::CallOriginate;

    originate->setType(Messages::CallOriginate::Normal);
    originate->setAllowCLI((Messages::CallOriginate::AllowCLI) dial->clir);

    std::vector<unsigned char> number;
    number.resize(82);

    originate->setNumberLength(std::min(number.size(), strlen(dup)));

    if(dup[0] == '+')
        originate->setNumberType(Messages::CallOriginate::International);
    else
        originate->setNumberType(Messages::CallOriginate::Unknown);

    memcpy(&number[0], dup, originate->numberLength());
    originate->setNumber(number);

    originate->setReserved(0);
    if(extension) {
        originate->setExtension(atoi(extension));
    } else {
        originate->setExtension(0);
    }
    free(dup);

    Message *reply = m_ril->execute(originate);
    completeGenCommand(reply, "CallOriginate", request);
}

void RequestHandler::handle(SamsungIPC::Messages::CallIncoming *message) {
    Log::debug("Incoming call: type 0x%04hX, reserved 0x%02hX, lineId 0x%02hX",
               message->callType(), message->reserved(), message->lineId());

    m_ril->unsolicited(RIL_UNSOL_CALL_RING);
    m_ril->unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED);
}

static RIL_LastCallFailCause mapCallFail(unsigned char cause1, unsigned char cause2) {
    RIL_LastCallFailCause cause;

    switch(cause1) {
        case 2:
        case 4:
        case 5:
        case 8:
            cause = CALL_FAIL_ERROR_UNSPECIFIED;

            break;

        case 3:
            cause = CALL_FAIL_CONGESTION;

            break;

        case 6:
            cause = CALL_FAIL_ACM_LIMIT_EXCEEDED;

            break;

        case 10:
            cause = CALL_FAIL_CALL_BARRED;

            break;

        default:
            switch(cause2) {
                case 1:
                    cause = CALL_FAIL_UNOBTAINABLE_NUMBER;

                    break;

                case 4:
                    cause = CALL_FAIL_CALL_BARRED;

                    break;

                case 6:
                    cause = CALL_FAIL_BUSY;

                    break;

                case 7:
                    cause = (RIL_LastCallFailCause) 18;

                    break;

                case 8:
                    cause = (RIL_LastCallFailCause) 19;

                    break;

                case 9:
                case 16:
                    cause = CALL_FAIL_ERROR_UNSPECIFIED;

                    break;

                case 20:
                    cause = CALL_FAIL_CONGESTION;

                    break;

                case 27:
                    cause = (RIL_LastCallFailCause) 57;

                    break;

                case 31:
                    cause = CALL_FAIL_ACM_LIMIT_EXCEEDED;

                    break;

                case 32:
                    cause = (RIL_LastCallFailCause) 69;

                    break;

                case 37:
                    cause = (RIL_LastCallFailCause) 88;

                    break;

                default:
                    cause = CALL_FAIL_NORMAL;

                    break;

            }

            break;
    }

    return cause;
}

void RequestHandler::handle(SamsungIPC::Messages::CallStateChanged *message) {


    Log::debug("Call state changed: type 0x%04hX, call id 0x%02hX, event 0x%02hX, release cause: 0x%02hhX, 0x%02hhX",
               message->callType(), message->callId(), message->event(), message->releaseCause1(), message->releaseCause2());

    m_lastCallFailCause = mapCallFail(message->releaseCause1(), message->releaseCause2());

    m_ril->unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED);
}

void RequestHandler::handleHangup(Request *request) {
    genericHangup(request, Messages::SsManageCall::HangupSpecificCall, true);
}

void RequestHandler::handleAnswer(Request *request) {
    if(!m_callIds.empty()) {
        Log::debug("Answering call");

        Message *reply = m_ril->execute(new Messages::CallAnswer);
        completeGenCommand(reply, "CallAnswer", request);
    } else {
        Log::debug("No call to answer");

        request->complete(RIL_E_GENERIC_FAILURE);
    }
}

void RequestHandler::handleHangupWaitingOrBackground(Request *request) {
    genericHangup(request, Messages::SsManageCall::HangupWaitingOrBackground, false);
}

void RequestHandler::handleHangupForegroundResumeBackground(Request *request) {
    genericHangup(request, Messages::SsManageCall::HangupForegroundResumeBackground, false);
}

void RequestHandler::handleSwitchWaitingOrHoldingAndActive(Request *request) {
    Messages::SsManageCall *message = new Messages::SsManageCall;
    message->setCallCommand(Messages::SsManageCall::SwitchWaitingOrHoldingAndActive);
    message->setCallId(0);
    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "SsManageCall", request);
}

void RequestHandler::handleConference(Request *request) {
    Log::debug("Initiating conference");

    Messages::SsManageCall *message = new Messages::SsManageCall;
    message->setCallCommand(Messages::SsManageCall::Conference);
    message->setCallId(0);
    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "SsManageCall", request);
}

void RequestHandler::handleUDUB(Request *request) {
    Log::debug("Sending UDUB");

    Messages::SsManageCall *message = new Messages::SsManageCall;
    message->setCallCommand(Messages::SsManageCall::HangupWaitingOrBackground);
    message->setCallId(0);
    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "SsManageCall", request);
}

void RequestHandler::handleSeparateConnection(Request *request) {
    int lineIndex = ((int *) request->data())[0] - 1;

    if(lineIndex >= (int) m_callIds.size()) {
        Log::debug("No call on line %d", lineIndex + 1);

        request->complete(RIL_E_GENERIC_FAILURE);

        return;
    }

    int callId = m_callIds[lineIndex];

    Log::debug("Separating connection %d", callId);
    Messages::SsManageCall *message = new Messages::SsManageCall;
    message->setCallCommand(Messages::SsManageCall::SeparateConnection);
    message->setCallId(callId);
    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "SsManageCall", request);
}

void RequestHandler::handleExplicitCallTransfer(Request *request) {
    Log::debug("Explicit call transfer");

    Messages::SsManageCall *message = new Messages::SsManageCall;
    message->setCallCommand(Messages::SsManageCall::ExplicitCallTransfer);
    message->setCallId(0);
    Message *reply = m_ril->execute(message);
    completeGenCommand(reply, "SsManageCall", request);
}

void RequestHandler::genericHangup(Request *request, int manageCommand, bool useValidCallId) {
    int calls = m_callIds.size();

    if(calls == 0) {
        Log::debug("No call to hangup");

        request->complete(RIL_E_GENERIC_FAILURE);
    } else if(calls == 1) {
        Log::debug("Releasing call");

        Message *reply = m_ril->execute(new Messages::CallRelease);
        completeGenCommand(reply, "CallRelease", request);

    } else {
        int callId;

        if(useValidCallId) {
            int lineIndex = ((int *) request->data())[0] - 1;

            if(lineIndex >= calls) {
                Log::debug("No call on line %d", lineIndex + 1);

                request->complete(RIL_E_GENERIC_FAILURE);

                return;
            }

            callId = m_callIds[lineIndex];
        } else {
            callId = 0;
        }

        Log::debug("Hangup through SsManageCall: command %d, call %d", manageCommand, callId);

        Messages::SsManageCall *message = new Messages::SsManageCall;
        message->setCallCommand((Messages::SsManageCall::CallCommand) manageCommand);
        message->setCallId(callId);
        Message *reply = m_ril->execute(message);
        completeGenCommand(reply, "SsManageCall", request);
    }
}

