#include "siplogwriter.h"

void SipLogWriter::write(const pj::LogEntry& entry) {
    std::string message = entry.msg;

    if (
        message.find("SIP/2.0") != std::string::npos ||
        message.find("REGISTER") != std::string::npos ||
        message.find("INVITE") != std::string::npos ||
        message.find("BYE") != std::string::npos ||
        message.find("CANCEL") != std::string::npos ||
        message.find("ACK") != std::string::npos ||
        message.find("OPTION") != std::string::npos
        ) {
        //qDebug().noquote() << "Custom Logwriter:\n" <<  QString::fromStdString(message);
        emit new_sip_message(QString::fromStdString(message));
    }


}
