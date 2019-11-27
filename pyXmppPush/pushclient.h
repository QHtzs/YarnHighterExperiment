#ifndef PUSHCLIENT_H
#define PUSHCLIENT_H

/*
 * Copyright (C) 2008-2019 The QXmpp developers
 *
 * Author:
 *  Manjeet Dahiya
 *
 * Source:
 *  https://github.com/qxmpp-project/qxmpp
 *
 * This file is a part of QXmpp library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

/*
wrappered xmpp client based on QXmpp
*/

#include <QObject>
#include <QXmppClient.h>
#include <QXmppTransferManager.h>
#include <QXmppMessage.h>
#include <QDateTime>
#include <QMap>
#include <QPair>
#include <QMutex>
#include <QTimer>
#include <QEventLoop>
#include <QRandomGenerator>


class PushClient : public QXmppClient
{
    Q_OBJECT
public:
    explicit PushClient(QObject *parent = nullptr);
    PushClient(const PushClient &) = delete;
    PushClient& operator=(PushClient const &) = delete;
    ~PushClient();

signals:


public slots:
    void send_image_file(const QString &target_jid, const QString &filename);
    void connect_to_host(const QString &jid, const QString &pw);
    void reconnect_again();
    void update_configure(const QXmppConfiguration & config);
    void set_ignore_sslerror();
    void set_ca_certifys(QByteArrayList pems);
    void add_ca_certificate(QByteArray pem);
    void send_message(const QString &bid, const QString& msg);

private slots:
    void send_failed_data_again(QXmppTransferJob::Error e);

private:
    void msleep_unblock_widget(int m_sec);
    static int random_int(int low, int end);
    QXmppTransferManager *m_trans_manager;
};

#endif // PUSHCLIENT_H
