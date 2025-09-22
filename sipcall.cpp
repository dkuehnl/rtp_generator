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
