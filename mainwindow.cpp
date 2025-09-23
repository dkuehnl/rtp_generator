#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QButtonGroup>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_sip = new SipMachine(this);
    m_chart_widget = ui->gvFlowChart;

    connect(m_sip, &SipMachine::registration_state_changed, this, &MainWindow::on_registration_state_changed);
    connect(m_sip, &SipMachine::new_sip_message, this, &MainWindow::display_sip_message, Qt::QueuedConnection);
    connect(ui->rbAdvCallflow, &QRadioButton::toggled, this, &MainWindow::activate_advanced_call_setup);
    connect(ui->rbAdvRtpFlow, &QRadioButton::toggled, this, &MainWindow::activate_advanced_rtp_setup);
    connect(ui->rbCustCodecs, &QRadioButton::toggled, this, &MainWindow::activate_cust_codecs);
    connect(ui->rbInvokeGatekeeper, &QRadioButton::toggled, this, &MainWindow::activate_gatekeeper);

    //Disable Advanced-Options:
    MainWindow::activate_advanced_call_setup(false);
    MainWindow::activate_cust_codecs(false);
    MainWindow::activate_gatekeeper(false);
    MainWindow::activate_advanced_rtp_setup(false);


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


}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_btnRegister_clicked() {
    QString user = ui->leUser->text();
    QString proxy_ip = ui->leProxyIp->text();
    QString password = ui->lePassword->text();

    m_sip->create_account(user, proxy_ip, password);
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
    ui->cbSuppTimer->setEnabled(active);
}
