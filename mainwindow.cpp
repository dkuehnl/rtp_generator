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


#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QButtonGroup>
#include <QDebug>
#include <QtEndian>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_sip = new SipMachine(this);
    m_chart_widget = ui->gvFlowChart;
    m_udp_socket = new QUdpSocket(this);

    connect(m_sip, &SipMachine::registration_state_changed, this, &MainWindow::on_registration_state_changed);
    connect(m_sip, &SipMachine::new_sip_message, this, &MainWindow::display_sip_message, Qt::QueuedConnection);
    connect(ui->rbAdvCallflow, &QRadioButton::toggled, this, &MainWindow::activate_advanced_call_setup);
    connect(ui->rbAdvRtpFlow, &QRadioButton::toggled, this, &MainWindow::activate_advanced_rtp_setup);
    connect(ui->rbCustCodecs, &QRadioButton::toggled, this, &MainWindow::activate_cust_codecs);
    connect(ui->rbInvokeGatekeeper, &QRadioButton::toggled, this, &MainWindow::activate_gatekeeper);
    connect(ui->rbAdvRegister, &QRadioButton::toggled, this, &MainWindow::activate_advanced_register_setup);

    //Disable Advanced-Options:
    MainWindow::activate_advanced_settings(false);

    QButtonGroup* group_call_setup = new QButtonGroup(this);
    group_call_setup->addButton(ui->rbCallflow);
    group_call_setup->addButton(ui->rbAdvCallflow);
    ui->rbCallflow->setChecked(true);

    QButtonGroup* group_rtp_setup = new QButtonGroup(this);
    group_rtp_setup->addButton(ui->rbRtpFlow);
    group_rtp_setup->addButton(ui->rbAdvRtpFlow);

    QButtonGroup* group_rtp_stop = new QButtonGroup(this);
    group_rtp_stop->addButton(ui->rbStopRTPfor);
    group_rtp_stop->addButton(ui->rbStopRTPcomplete);

    QButtonGroup* group_register_setup = new QButtonGroup(this);
    group_register_setup->addButton(ui->rbRegister);
    group_register_setup->addButton(ui->rbAdvRegister);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::activate_advanced_settings(bool active) {
    MainWindow::activate_advanced_call_setup(active);
    MainWindow::activate_cust_codecs(active);
    MainWindow::activate_gatekeeper(active);
    MainWindow::activate_advanced_rtp_setup(active);
    MainWindow::activate_advanced_register_setup(active);
}


void MainWindow::activate_ui(bool active) {
    ui->rbRegister->setEnabled(active);
    ui->rbAdvRegister->setEnabled(active);
    ui->rbRtpFlow->setEnabled(active);
    ui->rbAdvRtpFlow->setEnabled(active);
    ui->rbCallflow->setEnabled(active);
    ui->rbAdvCallflow->setEnabled(active);
    MainWindow::activate_advanced_settings(active);
}

void MainWindow::on_btnRegister_clicked() {
    QString user = ui->leUser->text();
    QString proxy_ip = ui->leProxyIp->text();
    QString password = ui->lePassword->text();

    CallSetup call_setup;
    if (ui->rbAdvCallflow) {
        call_setup = MainWindow::collect_ui_call_information();
    }

    MainWindow::activate_ui(false);
    qDebug() << "Supp Timer: " << call_setup.supp_timer << "\n"
             << "Req Timer: " << call_setup.req_timer << "\n"
             << "Refresher: " << call_setup.refresher << "\n"
             << "Supp 100rel: " << call_setup.supp_rel << "\n"
             << "Req 100rel: " << call_setup.req_rel << "\n"
             << "Disable Update: " << call_setup.disable_update << "\n";

    m_sip->create_account(user, proxy_ip, password, call_setup);
}

void MainWindow::on_btnDeRegister_clicked() {
    MainWindow::activate_ui(true);
    if (!ui->rbAdvCallflow) {
        MainWindow::activate_advanced_settings(false);
    }
    m_sip->dereg_account();
}

void MainWindow::on_registration_state_changed(int sip_code, const QString& text) {
    if (sip_code == 200) {
        ui->lblRegState->setText(QString("Reg Success (SIP%1 %2)").arg(sip_code).arg(text));
        ui->lblRegState->setStyleSheet("QLabel { color: green; }");
    } else {
        ui->lblRegState->setText(QString("Reg Failed: SIP%1 %2)").arg(sip_code).arg(text));
        ui->lblRegState->setStyleSheet("QLabel { color: red; }");
    }
}

void MainWindow::display_sip_message(const QString& message) {
    m_chart_widget->add_message(message);
}

void MainWindow::on_btnCall_clicked() {
    QString destination = ui->leDestination->text();
    destination += "@tel.t-online.de";

    if (!m_sip->make_call(destination)) {
        qDebug() << "Failed to call";
    }
}

void MainWindow::on_btnEndCall_clicked() {
    m_sip->hangup_call();
}


CallSetup MainWindow::collect_ui_call_information() const {
    CallSetup setup;
    if (ui->rbAdvCallflow->isChecked()) {
        setup.gatekeeper = ui->rbInvokeGatekeeper->isChecked();
        setup.codecs = ui->rbCustCodecs->isChecked();
        setup.disable_update = ui->cbAllUpdate->isChecked();
        setup.supp_rel = ui->cbSupp100rel->isChecked();
        setup.req_rel = ui->cbReq100rel->isChecked();
        setup.supp_timer = ui->cbSuppTimer->isChecked();
        setup.req_timer = ui->cbReqTimer->isChecked();
        setup.pcma = ui->cbPCMA->isChecked();
        setup.pcmu = ui->cbPCMU->isChecked();
        setup.g722 = ui->cbG722->isChecked();
        setup.televent = ui->cbTelDyn->isChecked();
        setup.cust_televent = ui->cbTelCust->isChecked();
    }

    if (setup.cust_televent) {
        setup.cust_tel_pt = ui->leTelDyn->text();
    }
    if (setup.req_timer) {
        setup.refresher = ui->combRefresher->currentText();
    }

    return setup;
}

void MainWindow::activate_advanced_rtp_setup(bool active) {
    ui->rbStopRTPcomplete->setEnabled(active);
    ui->rbStopRTPfor->setEnabled(active);

    ui->cbStopRTP->setEnabled(active);
    ui->cbStopRTPafter->setEnabled(active);
    ui->cbChangeSSRC->setEnabled(active);
    ui->cbChangeSSRCSequence->setEnabled(active);
    ui->cbChangeSequenceSSRC->setEnabled(active);
    ui->cbChangeSequence->setEnabled(active);
    ui->cbSetStartSSRC->setEnabled(active);
    ui->cbStartSequence->setEnabled(active);
    ui->cbSetSequenceGap->setEnabled(active);

    ui->leChangeSequence->setEnabled(active);
    ui->leChangeSSRC->setEnabled(active);
    ui->leSetSequenceGap->setEnabled(active);
    ui->leSetStartSSRC->setEnabled(active);
    ui->leStopRTPafter->setEnabled(active);
    ui->leStopRTPfor->setEnabled(active);
    ui->leStartSequence->setEnabled(active);

    ui->combStopRTPfor->setEnabled(active);
    ui->combStopRTPafter->setEnabled(active);
    ui->combChangeSSRC->setEnabled(active);
    ui->combChangeSequence->setEnabled(active);

    ui->lblSequenceGap->setEnabled(active);
}

void MainWindow::activate_advanced_call_setup(bool active) {
    ui->rbCustCodecs->setEnabled(active);
    ui->rbInvokeGatekeeper->setEnabled(active);
}

void MainWindow::activate_cust_codecs(bool active) {
    ui->cbG722->setEnabled(active);
    ui->cbPCMA->setEnabled(active);
    ui->cbPCMU->setEnabled(active);
    ui->cbTelCust->setEnabled(active);

    ui->cbTelDyn->setEnabled(active);
    ui->leTelDyn->setEnabled(active);
}

void MainWindow::activate_gatekeeper(bool active) {
    ui->cbAllUpdate->setEnabled(active);
    ui->cbSupp100rel->setEnabled(active);
    ui->cbReq100rel->setEnabled(active);
    ui->cbSuppTimer->setEnabled(active);
    ui->cbReqTimer->setEnabled(active);
    ui->combRefresher->setEnabled(active);
}

void MainWindow::activate_advanced_register_setup(bool active) {
    ui->rbSipInstance->setEnabled(active);
}

void MainWindow::on_btnRtpPaket_clicked() {
    int count = 0;
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=, &count]() mutable {
        if (count >= 10) {
            timer->stop();
            return;
        }

        QByteArray paket = MainWindow::create_rtp_paket("PCMA", "0x11111111", count);
        m_udp_socket->writeDatagram(paket, QHostAddress("127.0.0.1"), 4000);
        count++;
    });

    timer->start(20);
}

QByteArray MainWindow::create_rtp_paket(const QString& payload_type, const QString& input_ssrc, uint16_t sequence) {
    //Initial values
    uint8_t pt = 0;
    uint32_t timestamp_step = 0;
    int payload_size = 0;

    if (payload_type.toUpper() == "PCMU") {
        pt = 0;
        timestamp_step = 160;
        payload_size = 160;
    } else if (payload_type.toUpper() == "PCMA") {
        pt = 8;
        timestamp_step = 160;
        payload_size = 160;
    } else if (payload_type.toUpper() == "G722") {
        pt = 9;
        timestamp_step = 160;
        payload_size = 160;
    } else {
        qWarning() << "Unsupported payload type: " << payload_type;
        return QByteArray();
    }

#pragma pack(push, 1)
    struct RtpHeader {
        uint8_t v_p_x_cc;
        uint8_t m_pt;
        uint16_t seq;
        uint32_t timestamp;
        uint32_t ssrc;
    };
#pragma pack(pop)

    RtpHeader header{};
    header.v_p_x_cc = (2 << 6);
    header.m_pt = pt & 0x7F;
    header.seq = qToBigEndian(sequence);

    static uint32_t global_timestamp = 0;
    header.timestamp = qToBigEndian(global_timestamp);
    global_timestamp += timestamp_step;

    bool ok = false;
    uint32_t ssrc = input_ssrc.toUInt(&ok, 0);
    if (!ok) ssrc = 0x0000FFFF;
    header.ssrc = qToBigEndian(ssrc);

    QByteArray payload(payload_size, 0xFF);
    if (payload_type.toUpper() == "G722") {
        payload.fill(0x00);
    }


    QByteArray packet;
    packet.resize(sizeof(RtpHeader) + payload_size);
    memcpy(packet.data(), &header, sizeof(RtpHeader));
    memcpy(packet.data() + sizeof(RtpHeader), payload.constData(), payload_size);

    return packet;
}

