#ifndef FLOWCHART_H
#define FLOWCHART_H

#include <QString>
#include <QGraphicsView>

struct SipEvent {
    QString source_ip;
    QString destination_ip;
    QString message_header;
    QString sip_message;
    bool is_request;
};

class FlowChart : public QGraphicsView {
    Q_OBJECT
public:
    FlowChart(QWidget* parent = nullptr);

public slots:
    void add_message(const QString& log_message);

private:
    QGraphicsScene* m_scene;
    QVector<SipEvent> m_messages;
    int m_current_y = 0;

    SipEvent parse_message(const QString& log_message);
    void draw_event(const SipEvent& event);
};

#endif // FLOWCHART_H
