#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sipmachine.h"
#include "flowchart.h"

#include <QMainWindow>

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
    void on_rbCallflow_toggled(bool checked);
    void on_registration_state_changed(int sip_code, const QString& text);
    void display_sip_message(const QString& message);
    void on_btnCall_clicked();


private:
    void activate_advanced_call_setup(bool active);
    void activate_advanced_rtp_setup(bool active);

    Ui::MainWindow* ui;
    SipMachine* m_sip;
    FlowChart* m_chart_widget;

};
#endif // MAINWINDOW_H
