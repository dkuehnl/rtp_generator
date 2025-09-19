#include "flowchart.h"

#include <QObject>
#include <QGraphicsTextItem>

const int left_x = 50;
const int right_x = 350;

FlowChart::FlowChart(QWidget* parent)
    : QGraphicsView(parent), m_scene(new QGraphicsScene(this)) {

    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);

    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

SipEvent FlowChart::parse_message(const QString& log_message) {
    SipEvent event;

    if (log_message.contains("Request msg")) {
        event.is_request = true;
        auto parts = log_message.split("Request msg ");
        event.message_header = parts[1].split(" ")[0];
    } else if (log_message.contains("Response msg")) {
        event.is_request = false;
        auto parts = log_message.split("Response msg ");
        event.message_header = parts[1].split(" ")[0];
    }

    if (log_message.contains("to TCP")) {
        event.destination_ip = log_message.section("to TCP ", 1).section(":", 0, 0);
        event.source_ip = log_message.section("SIP/2.0/TCP ", 1).section(":", 0, 0);
    }
    if (log_message.contains("from TCP")) {
        event.source_ip = log_message.section("from TCP ", 1).section(":", 0, 0);
        event.destination_ip = log_message.section("SIP/2.0/TCP ", 1).section(":", 0, 0);
    }

    int pos = log_message.indexOf(":\n");
    if (pos != -1) {
        event.sip_message = log_message.mid(pos + 2);
    }

    return event;
}

void FlowChart::add_message(const QString& log_message) {
    SipEvent message = FlowChart::parse_message(log_message);
    m_messages.push_back(message);
    FlowChart::draw_event(message);
}

void FlowChart::draw_event(const SipEvent& message) {
    int y = m_current_y + 20;
    m_current_y += 40;

    m_scene->addLine(left_x, 0, left_x, m_current_y, QPen(Qt::black));
    m_scene->addLine(right_x, 0, right_x, m_current_y, QPen(Qt::black));

    if (m_messages.size() == 1) {
        m_scene->addText(message.source_ip)->setPos(left_x - 30, -20);
        m_scene->addText(message.destination_ip)->setPos(right_x -30, -20);
    }

    int src_x = message.is_request ? left_x : right_x;
    int dst_x = message.is_request ? right_x : left_x;

    QGraphicsLineItem* arrow = m_scene->addLine(src_x, y, dst_x, y, QPen(Qt::blue, 2));
    QPolygonF arrow_head;
    if (src_x < dst_x) {
        arrow_head << QPointF(dst_x, y)
                   << QPointF(dst_x - 10, y - 5)
                   << QPointF(dst_x - 10, y + 5);
    } else {
        arrow_head << QPointF(dst_x, y)
                   << QPointF(dst_x + 10, y - 5)
                   << QPointF(dst_x + 10, y + 5);
    }

    m_scene->addPolygon(arrow_head, QPen(Qt::blue), QBrush(Qt::blue));

    QGraphicsTextItem* label = m_scene->addText(message.message_header);
    label->setPos((src_x + dst_x) / 2 - 60, y - 20);
}
