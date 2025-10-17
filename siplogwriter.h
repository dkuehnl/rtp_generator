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
 *Purpose of the file siplogwriter.h/cpp:
 *This Class is derived from the original PJSIP logwriter and has the
 *responsibility to catch the PJSIP-logmessages, classify them and hand
 *them over to the FlowChart-Object.
 *Handover is done by a signal and the characteristics for deciding whether
 *or not handover it is done by the Payload - only SIP-Messages are forwarded.
 *All other PJSIP-message are dropped/ignored.
 *
 *
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
