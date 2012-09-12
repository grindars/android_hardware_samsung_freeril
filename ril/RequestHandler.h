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

#ifndef __REQUEST_HANDLER__H__
#define __REQUEST_HANDLER__H__

#include <telephony/ril.h>

#include <IUnsolicitedReceiver.h>

namespace SamsungIPC {
    class Message;
}

class RIL;
class Request;
class OemRequestHandler;
class OemUnsolicitedBuilder;

class RequestHandler: public SamsungIPC::IUnsolicitedReceiver {
public:
    RequestHandler(RIL *ril);
    virtual ~RequestHandler();

    virtual void handle(Request *request);
    virtual bool supports(int request);

    virtual void handle(SamsungIPC::Messages::PwrPhoneBootComplete *message);
    virtual void handle(SamsungIPC::Messages::PwrPhonePoweredOff *message);
    virtual void handle(SamsungIPC::Messages::PwrPhoneReset *message);
    virtual void handle(SamsungIPC::Messages::PwrPhoneModeChanged *message);
    virtual void handle(SamsungIPC::Messages::MiscGetMobileEquipImsiReply *message);
    virtual void handle(SamsungIPC::Messages::SecGetPinStatusReply *message);
    virtual void handle(SamsungIPC::Messages::SecSimCardType *message);
    virtual void handle(SamsungIPC::Messages::NetGetCurrentPlmnReply *message);
    virtual void handle(SamsungIPC::Messages::NetGetNetworkRegistrationReply *message);
    virtual void handle(SamsungIPC::Messages::DispGetIconInformationReply *message);
    virtual void handle(SamsungIPC::Messages::DispRssiInformation *message);
    virtual void handle(SamsungIPC::Messages::CallIncoming *message);
    virtual void handle(SamsungIPC::Messages::CallStateChanged *message);
    virtual void handle(SamsungIPC::Messages::SvcDisplayScreen *message);

    static bool completeGenCommand(SamsungIPC::Message *reply, const char *name, Request *request);
    static void unexpected(const std::string &message, SamsungIPC::Message *reply);

private:

    void handleRadioPower(Request *request);

    void handleBasebandVersion(Request *request);
    void handleIMEI(Request *request);
    void handleIMEISV(Request *request);
    void handleIMSI(Request *request);

    void handleSIMStatus(Request *request);
    void handleEnterSIMPin(Request *request);
    void handleEnterSIMPuk(Request *request);
    void handleEnterSIMPin2(Request *request);
    void handleEnterSIMPuk2(Request *request);
    void handleChangeSIMPin(Request *request);
    void handleChangeSIMPin2(Request *request);
    static void handlePinStatusRefreshComplete(SamsungIPC::Message *reply, void *arg);
    void setPinStatus(Request *request, const char *pin, const char *puk, int op);
    void changeLockPwd(Request *request, const char *currentPwd, const char *newPwd, int op);

    void handleQueryNetworkSelectionMode(Request *request);
    void handleSetNetworkSelectionAutomatic(Request *request);
    void handleSetNetworkSelectionManual(Request *request);
    void handleQueryAvailableNetworks(Request *request);
    void handleOperator(Request *request);
    void handleSetBandMode(Request *request);
    void handleQueryAvailableBandMode(Request *request);
    void handleSetPreferredNetworkType(Request *request);
    void handleGetPreferredNetworkType(Request *request);
    void handleVoiceRegistrationState(Request *request);
    void handleDataRegistrationState(Request *request);
    void setPLMNSelection(Request *request, const char *plmn);
    void getNetworkRegistration(Request *request, int op);
    int getNetworkMode(Request *request);

    void handleCurrentCalls(Request *request);
    void handleDial(Request *request);
    void handleHangup(Request *request);
    void handleHangupWaitingOrBackground(Request *request);
    void handleHangupForegroundResumeBackground(Request *request);
    void handleSwitchWaitingOrHoldingAndActive(Request *request);
    void handleConference(Request *request);
    void handleUDUB(Request *Request);
    void handleLastCallFailCause(Request *request);
    void handleAnswer(Request *request);
    void handleSeparateConnection(Request *request);
    void handleExplicitCallTransfer(Request *request);
    void handleDTMF(Request *request);
    void handleDTMFStart(Request *request);
    void handleDTMFStop(Request *request);
    void genericHangup(Request *request, int manageCommand, bool useValidCallId);
    void continuousDTMF(Request *request, char tone);

    void handleScreenState(Request *request);
    void handleSignalStrength(Request *request);

    void handleSetMute(Request *request);
    void handleGetMute(Request *request);

    void handleOemHookRaw(Request *request);

    enum {
        FirstRequest = RIL_REQUEST_GET_SIM_STATUS,
        LastRequest = RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS
    };

    RIL *m_ril;

    static void (RequestHandler::*m_requestHandlers[LastRequest - FirstRequest + 1])(Request *request);
    int m_coarseRSSI;
    RIL_LastCallFailCause m_lastCallFailCause;
    std::vector<unsigned char> m_callIds;
    OemRequestHandler *m_oemHandler;
    OemUnsolicitedBuilder *m_oemBuilder;
};

#endif
