#include "ebxmppclient.h"
#include <QTimer>
#include <QFile>
#include <QCoreApplication>
#include <QBuffer>
#include <QDir>

EBXmppClient::EBXmppClient(QObject *parent)
    : QXmppClient(parent),
      m_roster_manager(findExtension<QXmppRosterManager>()),
      m_pic_transfer_manager(new QXmppTransferManager),
      m_call_manager(new QXmppCallManager),
      m_default_config()
{
    addExtension(m_pic_transfer_manager);
    addExtension(m_call_manager);

    logger()->setLoggingType(QXmppLogger::NoLogging);

#ifdef ANDRIOD
   m_store_dir = "/storage/emulated/0/QxmppData";
#else
    m_store_dir = QDir::currentPath() + "/QxmppData";
#endif
   QDir fold;
   if(!fold.exists(m_store_dir)) fold.mkdir(m_store_dir);

    connect(this, SIGNAL(disconnected()), this, SIGNAL(logout_mannual()));
    connect(this, SIGNAL(error(QXmppClient::Error)), this, SLOT(client_error(QXmppClient::Error)));

    connect(m_pic_transfer_manager, SIGNAL(fileReceived(QXmppTransferJob*)),
                        this, SLOT(file_receieved(QXmppTransferJob*)));
    connect(this, SIGNAL(messageReceived(QXmppMessage)), this , SLOT(message_handle(QXmppMessage)));
}

EBXmppClient::~EBXmppClient(){
    m_call->deleteLater();
    m_call_manager->deleteLater();
    m_pic_transfer_manager->deleteLater();
    m_roster_manager->deleteLater();
}

QString EBXmppClient::cur_user() const
{
    return m_cur_user;
}

QXmppConfiguration& EBXmppClient::GetDefaultGlobalConfig()
{
    return m_default_config;
}

void EBXmppClient::SetCaCertificates(const QList<QSslCertificate> &cas){
    m_default_config.setCaCertificates(cas);
}

void EBXmppClient::AddClientCertificate(const QSslCertificate &crt)
{
    QSslConfiguration ssl_conf = configuration().sslconfig();
    ssl_conf.setLocalCertificate(crt);
    m_default_config.setSslConfiguration(ssl_conf);
}

void EBXmppClient::AddPrivateKey(const QSslKey &key){
    QSslConfiguration ssl_conf = configuration().sslconfig();
    ssl_conf.setPrivateKey(key);
    m_default_config.setSslConfiguration(ssl_conf);
}

void EBXmppClient::connectToServerHost(const QString un, const QString pw)
{

    m_default_config.setJid(un);
    m_default_config.setPassword(pw);
    connectToServer(m_default_config);
    m_cur_user = un;
    connect(this, &QXmppClient::connected, this, [&, un, pw](){
        emit connected_status(un, pw, true);
    });
    QTimer::singleShot(10000, this, [&, un, pw](){
        emit connected_status(un, pw, false);
    });
}

void EBXmppClient::sendMessage(const QString& bareJid, const QString& message)
{
    QXmppRosterManager *rosterManager = findExtension<QXmppRosterManager>();

    const QStringList resources = rosterManager
            ? rosterManager->getResources(bareJid)
            : QStringList();

    if (!resources.isEmpty()) {
        for (const auto &resource : resources) {
            QXmppMessage msg({}, bareJid + QStringLiteral("/") + resource, message);
            msg.setStamp(QDateTime::currentDateTime());
            sendPacket(msg);
        }
    } else {
        QXmppMessage msg({}, bareJid, message);
        msg.setStamp(QDateTime::currentDateTime());
        sendPacket(msg);
    }
}

void EBXmppClient::callConnect()
{

}

void EBXmppClient::callFinished()
{
    emit image_to_show(QImage(":/icons/end.jpg"));
    emit call_view_roster_view();
}

void EBXmppClient::callReceived(QXmppCall *call)
{
    Q_UNUSED(call);
}

void EBXmppClient::callStarted(QXmppCall *call)
{
    Q_UNUSED(call);
}

void EBXmppClient::callStateChanged(QXmppCall::State state)
{
    Q_UNUSED(state);
}

void EBXmppClient::videoModeChanged(QIODevice::OpenMode mode)
{
    Q_UNUSED(mode);
}

void EBXmppClient::get_rosters_bids()
{
    connect(m_roster_manager, &QXmppRosterManager::rosterReceived, this, [&](){
        QStringList ret = m_roster_manager->getRosterBareJids();
        emit send_rosters_bids(ret);
    });
}

void EBXmppClient::client_error(QXmppClient::Error e){
    if(e==QXmppClient::SocketError || e==QXmppClient::KeepAliveError) emit logout_mannual();
}

void EBXmppClient::transfer_error_info(QXmppTransferJob::Error){
    emit progress_bar_value(-1, -1);
    emit self_failed();
}

void EBXmppClient::send_message_to(QString bareid, QString source, QString filetype, const QString &text, const QString &time)
{
    if(filetype=="text"){
        sendMessage(bareid, text);
    }
    else{
        if(source.isEmpty()) source = "QXmpp";
        QString jid = bareid + "/" + source;
        QXmppTransferJob *job = m_pic_transfer_manager->sendFile(jid, text, filetype);
        if(job == nullptr){
            emit send_data_failed(bareid, time);
            return;
        }
        connect(job, SIGNAL(progress(qint64, qint64)),
                            this, SIGNAL(progress_bar_value(qint64,qint64)));
        connect(job, &QXmppTransferJob::finished, this, [=](){emit progress_bar_value(1, 1);});

        connect(job, SIGNAL(error(QXmppTransferJob::Error)), this,
                SLOT(transfer_error_info(QXmppTransferJob::Error)));

        connect(this, &EBXmppClient::self_failed, this, [&](){emit send_data_failed(bareid, time);});
    }
}

void EBXmppClient::message_handle(const QXmppMessage &message)
{
    QString jid = message.from();
    QString text = message.body();
    if(text=="__RECIEVED__") return;
    sendPacket(QXmppMessage(message.to(), jid, "__RECIEVED__"));
    QString time = message.stamp().toString("yyyy-MM-dd hh:mm:ss");
    if(message.stamp().isNull()) time = "00-00-00 00:00:00";
    emit message_come(QXmppUtils::jidToBareJid(jid), QXmppUtils::jidToResource(jid), "text", text, time);
}

void EBXmppClient::file_receieved(QXmppTransferJob* job)
{
     QString file_name = QString("IM%1%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(job->fileName().replace("^.*\\.", "."));
     file_name = m_store_dir + "/" + file_name;
     QFile *file = new QFile(file_name, job); // deleted while job(parent) deleting;
     if(!file->open(QIODevice::WriteOnly)) return;
     connect(job, &QXmppTransferJob::finished, this, [=](){
        file->flush(); //flush data
        file->close();
        emit message_come(QXmppUtils::jidToBareJid(job->jid()), QXmppUtils::jidToResource(job->jid()), "image", file_name, job->fileDate().toString("yyyy-MM-dd hh:mm:ss"));
     });
    job->accept(file); //file will close without flush
}
