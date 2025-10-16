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
