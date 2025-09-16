#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_sip = new SipMachine(this);

    connect(ui->btnRegister, &QPushButton::clicked, this, &MainWindow::on_btnRegister_clicked);
    connect(m_sip, &SipMachine::registration_state_changed, this, &MainWindow::on_registration_state_changed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnRegister_clicked() {
    QString user = ui->leUser->text();
    QString domain = ui->leDomain->text();
    QString password = ui->lePassword->text();

    m_sip->create_account(user, domain, password, true);
}

void MainWindow::on_registration_state_changed(int sip_code, const QString& text) {
    if (sip_code == 200) {
        ui->lblRegState->setText(QString("Reg Success (SIP%1 %2").arg(sip_code).arg(text));
        ui->lblRegState->setStyleSheet("QLabel { color: green; }");
    } else {
        ui->lblRegState->setText(QString("Reg Failed: SIP%1 %2").arg(sip_code).arg(text));
        ui->lblRegState->setStyleSheet("QLabel { color: red; }");
    }
}
