/*
 * Copyright (C) 2025-2025 Dennis Kühnlein <d.kuehnlein@outlook.com>
 *
 * *********************RTP-Generator******************************
 *
 *The purpose of the RTP-Generator is to establish a working call
 *and send a customized RTP-stream. This can be used to test the
 *behavior of IMS network-elements in edge-cases and to force
 *the user-agent behavior out of the world.
 *Due to the dynamic routing inside an IMS there are also some config-
 *options for the call-setup like:
 *- activating/deactivating UPDATE
 *- activating/deactivating 100rel and timer
 *- configure available codecs (speach and dtmf out-of-band)
 *
 *For RTP are more options available such as:
 *- change SSRC and/or sequence-number after time or packets
 *- pause rtp-stream for time or complete
 *- etc. (see mainwindow-ui or README for complete config-option)
 *
 *
 *Purpose of the file sipmachine.h/cpp:
 *The SipMachine-object is responsible for the PJSIP-account-configuration
 *and call-setup. It is triggered through the mainwindow.cpp-buttons and
 *also gets information about the ui-triggered setups (CallSetup struct).
 *SipMachine triggeres a registration to enable outgoing calls. Incoming calls
 *are currently not supported and also not planned to implement.
 *
 *The custom-logwrite SipLogWriter is also instantiated here and maintained in
 *the matching member-variable.
 *
 * License:
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */



#ifndef SIPMACHINE_H
#define SIPMACHINE_H

#include <QObject>
#include <QString>
#include <QMetaObject>

#include <pjsua2.hpp>
#include <pjsip.h>
#include <pjsip.h>
#include <pjsip_ua.h>

#include "siplogwriter.h"
#include "sipcall.h"

struct CallSetup {
    bool gatekeeper = false;
    bool disable_update = false;
    bool supp_rel = true;
    bool req_rel = false;
    bool supp_timer = true;
    bool req_timer = false;
    bool codecs = false;
    bool g722 = false;
    bool pcma = false;
    bool pcmu = false;
    bool televent = false;
    bool cust_televent = false;
    QString cust_tel_pt = "100";
    QString refresher = "uac";
};

class SipMachine : public QObject {
    Q_OBJECT

public:
    explicit SipMachine(QObject* parent = nullptr);
    ~SipMachine();

    bool init();
    bool create_account(
        const QString& username,
        const QString& proxy_ip,
        const QString& password,
        const CallSetup& setup
    );

    bool make_call(const QString& destination);
    void hangup_call();
    void dereg_account();

signals:
    void registration_state_changed(int sip_code, const QString& text);
    void new_sip_message(const QString& message);

public slots:
    void on_account_reg_state(int sip_code, const QString& sip_text);

private slots:
    void on_new_sip_message(const QString& message);

private:    
    pj::Endpoint m_endpoint;
    bool m_endpoint_inited = false;
    SipLogWriter* m_logwriter = nullptr;

    class MyAccount;
    MyAccount* m_account = nullptr;

    SipCall* m_call = nullptr;
    CallSetup m_setup;
};

#endif // SIPMACHINE_H
