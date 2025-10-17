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
 *Purpose of the file flowchart.h/cpp:
 *The FlowChart-Class is responsible for receiving the sip-logmessages from
 *from the custom logwriter-class declared in SipLogwriter.cpp.
 *It stores the sip-messages and draw it directly into mainwindow.ui.
 *This is possible because when instantiating this object from mainwindow.cpp
 *the ui has to be handed over to this class for referencing to the ui.
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
