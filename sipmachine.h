#ifndef SIPMACHINE_H
#define SIPMACHINE_H

#include <QObject>
#include <QString>
#include <QMetaObject>
#include <pjsua2.hpp>

class SipMachine : public QObject {
    Q_OBJECT

public:
    explicit SipMachine(QObject* parent = nullptr);
    ~SipMachine();

    bool init();
    bool create_account(
        const QString& username,
        const QString& domain,
        const QString& password,
        bool register_on_add = true);

signals:
    void registration_state_changed(int sip_code, const QString& text);

public slots:
    void on_account_reg_state(int sip_code, const QString& sip_text);

private:
    pj::Endpoint m_endpoint;
    bool m_endpoint_inited = false;

    class MyAccount;
    MyAccount* m_account = nullptr;
};

#endif // SIPMACHINE_H
