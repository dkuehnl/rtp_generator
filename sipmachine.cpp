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



#include "sipmachine.h"

#include <QString>
#include <QMetaObject>
#include <QDebug>
#include <QCoreApplication>

// Forward: Callback-Signatur für das Modul (korrekt: ein Parameter)
extern "C" pj_status_t on_tx_request_cb(pjsip_tx_data *tdata);

// Modul-Definition (static in .cpp!)
static pjsip_module mod_tx_hook = {
    NULL, NULL,
    { (char*)"mod-tx-hook", 11 },
    -1,
    PJSIP_MOD_PRIORITY_TRANSPORT_LAYER - 1,
    NULL, NULL, NULL, NULL,
    NULL, NULL,
    &on_tx_request_cb,  // on_tx_request  <- erwartet (pjsip_tx_data*)
    &on_tx_request_cb,  // on_tx_response <- auch (pjsip_tx_data*)
    NULL
};

// Implementation des Callbacks (ein Parameter!)
extern "C" pj_status_t on_tx_request_cb(pjsip_tx_data *tdata) {
    qDebug() << "in Callback";
    if (!tdata || !tdata->msg) return PJ_SUCCESS;

    // Holen des per-Call-Pointers, den wir vor dem makeCall in prm.txOption.mod_data gesetzt haben.
    SipCall* sip_call = nullptr;
    if (mod_tx_hook.id >= 0) {
        sip_call = reinterpret_cast<SipCall*>(tdata->mod_data[mod_tx_hook.id]);
    }

    // Falls kein SipCall zugeordnet ist, einfach nichts tun
    if (!sip_call) {
        return PJ_SUCCESS;
    }
    qDebug() << "SIP-Call gefunden";
    // Jetzt kannst du wie gewünscht Header anpassen basierend auf Flags in sip_call
    if (!sip_call->m_rel_not_supported && !sip_call->m_timer_not_supported) {
        qDebug() << "No Supported";
        // keine Supported-Header
        while (pjsip_hdr *hdr = (pjsip_hdr*) pjsip_msg_find_hdr(tdata->msg, PJSIP_H_SUPPORTED, NULL)) {
            pj_list_erase(hdr);
        }
        return PJ_SUCCESS;
    }

    if (sip_call->m_rel_not_supported) {
        qDebug() << "only timer";
        while (pjsip_hdr *hdr = (pjsip_hdr*) pjsip_msg_find_hdr(tdata->msg, PJSIP_H_SUPPORTED, NULL)) {
            pj_list_erase(hdr);
        }
        pj_str_t hname = pj_str((char*)"Supported");
        pj_str_t hval  = pj_str((char*)"timer");
        pjsip_generic_string_hdr *new_hdr = pjsip_generic_string_hdr_create(tdata->pool, &hname, &hval);
        pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)new_hdr);
        return PJ_SUCCESS;
    }

    if (sip_call->m_timer_not_supported) {
        qDebug() << "only rel";
        while (pjsip_hdr *hdr = (pjsip_hdr*) pjsip_msg_find_hdr(tdata->msg, PJSIP_H_SUPPORTED, NULL)) {
            pj_list_erase(hdr);
        }
        pj_str_t hname = pj_str((char*)"Supported");
        pj_str_t hval  = pj_str((char*)"100rel");
        pjsip_generic_string_hdr *new_hdr = pjsip_generic_string_hdr_create(tdata->pool, &hname, &hval);
        pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)new_hdr);
        return PJ_SUCCESS;
    }

    return PJ_SUCCESS;
}

class SipMachine::MyAccount : public pj::Account {
public:
    MyAccount(SipMachine* sip_machine) : m_machine(sip_machine) {}
    ~MyAccount() {}

    virtual void onRegState(pj::OnRegStateParam& reg_state_param) override {
        pj::AccountInfo account_info = getInfo();
        int code = account_info.regStatus;
        std::string text = account_info.regStatusText;

        if (m_machine) {
            QMetaObject::invokeMethod(
                m_machine,
                "on_account_reg_state",
                Qt::QueuedConnection,
                Q_ARG(int, code),
                Q_ARG(QString, QString::fromStdString(text)));
        }
    }

private:
    SipMachine* m_machine;
};

SipMachine::SipMachine(QObject* parent) : QObject(parent) {}

SipMachine::~SipMachine() {
    try {
        SipMachine::dereg_account();

        pjsip_endpt_unregister_module(pjsua_get_pjsip_endpt(), &mod_tx_hook);
        if (m_endpoint_inited) {
            m_endpoint.libDestroy();
            m_endpoint_inited = false;
        }
    } catch (pj::Error& err) {
        qWarning() << "PJSIP cleanup error:" << err.info().c_str();
    }

    if (m_logwriter) {
        delete m_logwriter;
        m_logwriter = nullptr;
    }
}

bool SipMachine::init() {
    try {
        m_endpoint.libCreate();
        pj::EpConfig endpoint_config;
        endpoint_config.logConfig.level = 5;
        endpoint_config.logConfig.msgLogging = 1;
        endpoint_config.uaConfig.natTypeInSdp = 0;
        endpoint_config.uaConfig.userAgent = (QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion()).toStdString();

        m_logwriter = new SipLogWriter(this);
        endpoint_config.logConfig.writer = m_logwriter;

        connect(m_logwriter, &SipLogWriter::new_sip_message, this, &SipMachine::new_sip_message, Qt::QueuedConnection);


        m_endpoint.libInit(endpoint_config);
        pj_status_t st = pjsip_endpt_register_module(pjsua_get_pjsip_endpt(), &mod_tx_hook);
        if (st != PJ_SUCCESS) {
            qWarning() << "Failed to register TX module";
        } else {
            qDebug() << "TX module registered OK";
        }

        pj::TransportConfig transport_cfg;
        transport_cfg.port = 5060;
        m_endpoint.transportCreate(PJSIP_TRANSPORT_TCP, transport_cfg);



        m_endpoint.libStart();
        m_endpoint_inited = true;
        return true;
    } catch (pj::Error& err) {
        qWarning() << "PJSIP init error:" << err.info().c_str();
        if (m_logwriter) {
            delete m_logwriter;
            m_logwriter = nullptr;
        }
        return false;
    }
}

bool SipMachine::create_account(const QString& username, const QString& proxy_ip, const QString& password, const CallSetup& setup) {
    if (!m_endpoint_inited && !init()) {
        return false;
    }

    m_setup = setup;

    try {
        pj::AccountConfig acc_config;
        std::string user = username.toStdString();
        std::string proxy = proxy_ip.toStdString();

        acc_config.idUri = "sip:" + user + "@tel.t-online.de";
        acc_config.regConfig.registrarUri = "sip:" + proxy + ":5060;transport=tcp";
        acc_config.regConfig.registerOnAdd = true;
        acc_config.regConfig.timeoutSec = 550;

        acc_config.sipConfig.proxies.clear();
        acc_config.sipConfig.proxies.push_back("sip:" + proxy + ":5060;transport=tcp");
        pj::AuthCredInfo credentials("digest", "*", user, 0, password.toStdString());
        acc_config.sipConfig.authCreds.push_back(credentials);

        pj::AccountCallConfig acc_call_config;
        if (m_setup.req_rel) {
            acc_call_config.prackUse = PJSUA_100REL_MANDATORY;
        }
        if (m_setup.req_timer) {
            acc_call_config.timerUse = PJSUA_SIP_TIMER_REQUIRED;
        }

        acc_config.natConfig.sipOutboundUse = 0;
        acc_config.natConfig.contactRewriteUse = 0;
        acc_config.natConfig.contactRewriteMethod = 0;
        acc_config.natConfig.viaRewriteUse = 0;

        acc_config.callConfig = acc_call_config;
        if (m_account) {
            m_account->setRegistration(false);
            delete m_account;
            m_account = nullptr;
        }

        m_account = new MyAccount(this);
        m_account->create(acc_config);
        return true;
    } catch (pj::Error& err) {
        qWarning() << "Account creation error:" << err.info().c_str();
        return false;
    }
}

void SipMachine::dereg_account() {
    try {
        if (m_account) {
            pj::AccountInfo account_info = m_account->getInfo();
            if (account_info.regIsActive) {
                m_account->setRegistration(false);
            }
            m_account->shutdown();
            delete m_account;
            m_account = nullptr;
        }
    } catch (pj::Error& err) {
        qWarning() << "Deregistration failed:" << err.info().c_str();
    }

    if (m_call) {
        delete m_call;
        m_call = nullptr;
    }
}

bool SipMachine::make_call(const QString& destination) {
    if (!m_account) {
        qWarning() << "No account available";
        return false;
    }

    try {
        pj::CallOpParam prm(false);
        if (m_setup.disable_update) {
            pj::SipHeader allow_header;
            allow_header.hName = "Allow";
            allow_header.hValue = "PRACK, INVITE, ACK, BYE, CANCEL, INFO, SUBSCRIBE, NOTIFY, REFER, MESSAGE, OPTIONS";
            prm.txOption.headers.push_back(allow_header);
        } else {
            pj::SipHeader allow_header;
            allow_header.hName = "Allow";
            allow_header.hValue = "PRACK, INVITE, ACK, BYE, UPDATE, CANCEL, INFO, SUBSCRIBE, NOTIFY, REFER, MESSAGE, OPTIONS";
            prm.txOption.headers.push_back(allow_header);
        }



        std::string uri = "sip:" + destination.toStdString();
        if (m_call) {
            delete m_call;
            m_call = nullptr;
        }

        m_call = new SipCall(*m_account);
        m_call->m_rel_not_supported = m_setup.supp_rel;
        m_call->m_timer_not_supported = m_setup.supp_timer;

        m_call->makeCall(uri, prm);
        return true;
    } catch (pj::Error &err) {
        qWarning() << "Call creation error:" << err.info().c_str();
        return false;
    }
}

void SipMachine::hangup_call() {
    if (m_call) {
        try {
            pj::CallOpParam prm(true);
            prm.reason = "Normal call clearing";
            m_call->hangup(prm);
            delete m_call;
            m_call = nullptr;
        } catch (pj::Error& err) {
            qWarning() << "Hangup failed: " << err.info().c_str();
        }
    } else {
        qWarning() << "No active call to hang up";
    }
}

void SipMachine::on_account_reg_state(int sip_code, const QString& text) {
    emit registration_state_changed(sip_code, text);
}

void SipMachine::on_new_sip_message(const QString& message) {
    emit new_sip_message(message);
}
