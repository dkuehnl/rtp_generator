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
        const CallSetup& setup);

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
