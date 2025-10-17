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
 *Purpose of the file mainwindow.h/cpp:
 *In this file is the main work done with the ui and application-setup.
 *Some mandatory objects like SipMachine, FlowChart and UdpSocket are created
 *here and added as member-objects to be available over the whole program.
 *It also connects the ui-buttons to the business-logic and is responsible
 *for activating/deactivating the config-options in the ui depending on
 *the selected options.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sipmachine.h"
#include "flowchart.h"

#include <QMainWindow>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnRegister_clicked();
    void on_registration_state_changed(int sip_code, const QString& text);
    void display_sip_message(const QString& message);
    void on_btnCall_clicked();
    void on_btnEndCall_clicked();
    void on_btnDeRegister_clicked();
    void on_btnRtpPaket_clicked();


private:
    void activate_ui(bool active);
    void activate_advanced_settings(bool active);
    void activate_advanced_call_setup(bool active);
    void activate_advanced_rtp_setup(bool active);
    void activate_advanced_register_setup(bool active);
    void activate_cust_codecs(bool active);
    void activate_gatekeeper(bool active);

    QByteArray create_rtp_paket(const QString& payload_type, const QString& ssrc, uint16_t sequence);
    CallSetup collect_ui_call_information() const;

    Ui::MainWindow* ui;
    SipMachine* m_sip;
    FlowChart* m_chart_widget;
    QUdpSocket* m_udp_socket;

};
#endif // MAINWINDOW_H
