#ifndef SIPLOGWRITER_H
#define SIPLOGWRITER_H

#include <pjsua2.hpp>
#include <QObject>

class SipLogWriter : public QObject,  public pj::LogWriter {
    Q_OBJECT

public:
    SipLogWriter(QObject* parent = nullptr) : QObject(parent) {}
    void write(const pj::LogEntry& entry) override;

signals:
    void new_sip_message(const QString& message);

};

#endif // SIPLOGWRITER_H
