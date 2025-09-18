#include "sipmachine.h"
#include "mainwindow.h"

#include <QString>
#include <QMetaObject>
#include <QDebug>

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
        if (m_account) {
            pj::AccountInfo account_info = m_account->getInfo();
            if (account_info.regIsActive) {
                m_account->setRegistration(false);
            }
            m_account->shutdown();
            delete m_account;
            m_account = nullptr;
        }

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

        m_logwriter = new SipLogWriter(this);
        endpoint_config.logConfig.writer = m_logwriter;

        connect(m_logwriter, &SipLogWriter::new_sip_message, this, &SipMachine::new_sip_message, Qt::QueuedConnection);


        m_endpoint.libInit(endpoint_config);

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

bool SipMachine::create_account(const QString& username, const QString& proxy_ip, const QString& password, QLabel* label) {
    if (!m_endpoint_inited && !init()) {
        return false;
    }

    try {
        pj::AccountConfig acc_config;
        std::string user = username.toStdString();
        std::string proxy = proxy_ip.toStdString();
        m_label = label;

        acc_config.idUri = "sip:" + user + "@tel.t-online.de";
        acc_config.regConfig.registrarUri = "sip:" + proxy + ":5060;transport=tcp";
        acc_config.sipConfig.proxies.clear();
        acc_config.sipConfig.proxies.push_back("sip:" + proxy + ":5060;transport=tcp");

        acc_config.natConfig.sipOutboundUse = 0;
        acc_config.natConfig.contactRewriteUse = 0;
        acc_config.natConfig.contactRewriteMethod = 0;

        acc_config.regConfig.registerOnAdd = true;

        pj::AuthCredInfo credentials("digest", "*", user, 0, password.toStdString());
        acc_config.sipConfig.authCreds.push_back(credentials);
        acc_config.regConfig.timeoutSec = 550;

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

void SipMachine::on_account_reg_state(int sip_code, const QString& text) {
    emit registration_state_changed(sip_code, text);
}

void SipMachine::on_new_sip_message(const QString& message) {
    emit new_sip_message(message);
}
