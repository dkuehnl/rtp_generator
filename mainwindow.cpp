#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_sip = new SipMachine(this);
    m_chart_widget = ui->gvFlowChart;

    connect(ui->btnRegister, &QPushButton::clicked, this, &MainWindow::on_btnRegister_clicked);
    connect(m_sip, &SipMachine::registration_state_changed, this, &MainWindow::on_registration_state_changed);
    connect(m_sip, &SipMachine::new_sip_message, this, &MainWindow::display_sip_message, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
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
