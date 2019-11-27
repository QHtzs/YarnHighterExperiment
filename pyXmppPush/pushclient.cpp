#include "pushclient.h"

class M_Data: public QObjectUserData
{
public:
    explicit M_Data(const QString &jid, const QString &name){
       this->jid = jid;
        filename = name;
    }
    QString jid;
    QString filename;
};

PushClient::PushClient(QObject *parent) : QXmppClient(parent)
{
    m_trans_manager = new QXmppTransferManager();
    this->addExtension(m_trans_manager);
    connect(this, SIGNAL(disconnected()), this, SLOT(reconnect_again()));
}

PushClient::~PushClient(){
    m_trans_manager->deleteLater();
}

void PushClient::msleep_unblock_widget(int m_sec)
{
    QEventLoop loop;
    QTimer::singleShot(m_sec,  &loop, [&loop](){loop.quit();});
    loop.exec();
}

int PushClient::random_int(int low, int end){
    return QRandomGenerator::global()->bounded(low, end); // [low, end)
}

void PushClient::send_image_file(const QString &target_jid, const QString &filename)
{
    QXmppTransferJob* job = m_trans_manager->sendFile(target_jid, filename, "image");
    job->setUserData(0, new M_Data(target_jid, filename));
    connect(job, SIGNAL(error(QXmppTransferJob::Error)),
            this, SLOT(send_failed_data_again(QXmppTransferJob::Error)));
    connect(job, &QXmppTransferJob::finished, this, [job](){delete job->userData(0);});
}

void PushClient::send_failed_data_again(QXmppTransferJob::Error){
    msleep_unblock_widget(random_int(30000, 60000));
    QXmppTransferJob *job = reinterpret_cast<QXmppTransferJob*>(sender());
    if(job->userData(0) == nullptr) return;
    M_Data *v = reinterpret_cast<M_Data*>(job->userData(0));
    if(v){
        send_image_file(v->jid, v->filename);
        delete v;
    }
}

void PushClient::reconnect_again()
{
    msleep_unblock_widget(30000);
    connectToServer(this->configuration());
}

void PushClient::connect_to_host(const QString &jid, const QString &pw)
{
    if(isConnected()) this->disconnectFromServer();
    QXmppConfiguration conf = this->configuration();
   if(conf.jid() != jid) conf.setJid(jid);
   if(conf.password() != pw) conf.setPassword(pw);
   connectToServer(conf);
}

void PushClient::update_configure(const QXmppConfiguration & config)
{
    this->configuration() = config;
}

void PushClient::send_message(const QString &bid, const QString& msg)
{
    QXmppMessage message(configuration().jid(), bid, msg);
    message.setStamp(QDateTime::currentDateTimeUtc());
    sendPacket(message);
}

void PushClient::set_ignore_sslerror()
{
    configuration().setIgnoreSslErrors(true);
}

void PushClient::set_ca_certifys(QByteArrayList pems){
    QList<QSslCertificate> cas;
    for(QByteArray bytes: pems){
        cas << QSslCertificate(bytes, QSsl::Pem);
    }
    configuration().setCaCertificates(cas);
}

void PushClient::add_ca_certificate(QByteArray pem){
    auto cas = configuration().caCertificates();
    cas << QSslCertificate(pem, QSsl::Pem);
    configuration().setCaCertificates(cas);
}
