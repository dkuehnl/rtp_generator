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
