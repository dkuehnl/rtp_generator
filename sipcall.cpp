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
 *Purpose of the file sipcall.h/cpp:
 *The SipCall-Class is a derived class from PJSIP-Call-class and is
 *responsible for the custom call-setup maintained and configured over
 *the ui in mainwindow.
 *This class is triggered and maintained by SipMachine-Class.
 *Currently it maintaines if 100rel/timer is acitvated or not. If this
 *will remain here depened on how I get this config-options get work with
 *PJSIP (because the default behaivor is to set both options in supported-
 *header and only get a easy way to elevate them in require-header. But
 *there is no option to fully delete both of them out of the INVITE-message)
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




#include "sipcall.h"

#include <QDebug>
#include <QString>

SipCall::SipCall(pj::Account& acc, int call_id) : pj::Call(acc, call_id) {}

void SipCall::onCallState(pj::OnCallStateParam& prm) {
    pj::CallInfo ci = getInfo();
    qDebug() << "Call state changed: " << QString::fromStdString(ci.stateText);
}

void SipCall::onCallMediaState(pj::OnCallMediaStateParam& prm) {
    pj::CallInfo ci = getInfo();
    for (auto& media : ci.media) {
        if (media.type == PJMEDIA_TYPE_AUDIO && getMedia(media.index)) {
            pj::AudioMedia* audio_media = (pj::AudioMedia*) getMedia(media.index);
            pj::AudDevManager& mgr = pj::Endpoint::instance().audDevManager();
            mgr.getCaptureDevMedia().startTransmit(*audio_media);
            audio_media->startTransmit(mgr.getPlaybackDevMedia());
        }
    }
}
