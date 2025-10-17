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
