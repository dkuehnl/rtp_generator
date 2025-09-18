#ifndef SIPMACHINE_H
#define SIPMACHINE_H

#include <QObject>
#include <QString>
#include <QMetaObject>
#include <QLabel>
#include <pjsua2.hpp>

#include "siplogwriter.h"

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
        QLabel* label);

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
    QLabel* m_label;
    SipLogWriter* m_logwriter = nullptr;

    class MyAccount;
    MyAccount* m_account = nullptr;
};

#endif // SIPMACHINE_H
