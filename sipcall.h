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




#ifndef SIPCALL_H
#define SIPCALL_H

#include <pjsua2.hpp>


class SipCall : public pj::Call {

public:
    SipCall(pj::Account& acc, int call_id = PJSUA_INVALID_ID);
    void onCallState(pj::OnCallStateParam& prm) override;
    void onCallMediaState(pj::OnCallMediaStateParam& prm) override;

    bool m_rel_not_supported = false;
    bool m_timer_not_supported = false;
};

#endif // SIPCALL_H
