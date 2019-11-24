#ifndef EBXMPPCLIENT_H
#define EBXMPPCLIENT_H

#include <QXmppClient.h>
#include <QXmppUtils.h>
#include <QAudioOutput>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>
#include <QXmppRosterManager.h>
#include <QXmppRtpChannel.h>
#include <QXmppMessage.h>
#include <QXmppTransferManager.h>
#include <QAudioInput>
#include <QAudioOutput>
#include <QXmppCallManager.h>
#include <QCamera>
#include <QDateTime>
#include <QCameraImageCapture>


class EBXmppClient : public QXmppClient
{
    Q_OBJECT
public:
    explicit EBXmppClient(QObject *parent = nullptr);
    ~EBXmppClient();
    QString cur_user() const;
    //添加信任证书
    void SetCaCertificates(const QList<QSslCertificate> &cas);

    //存在双向验证需求时用,一般无效
    void AddClientCertificate(const QSslCertificate &crt);
    void AddPrivateKey(const QSslKey &key);

    //获取全局配置的引用，便于修改
    QXmppConfiguration& GetDefaultGlobalConfig();

signals:

    void self_failed();

    void logout_mannual();

    void image_to_show(const QImage &im);

    void presenceChanged(const QString &bareJid, const QString &resource, int type);

    void message_come(const QString &bareid, const QString &source, const QString &filetype, const QString &text, const QString &time);

    void progress_bar_value(qint64 c_byte, qint64 total_byte);

    void call_view_roster_view();

    void send_rosters_bids(QStringList bids);

    void connected_status(QString un, QString pw, bool ret);

    void send_data_failed(const QString &bid, const QString &time);


public slots:
    //封装connectToServer,把m_default_config持久配置传入客户端socket
    void connectToServerHost(const QString un, const QString pw);
    //覆盖sendMessage, 添加发送时间
    void sendMessage(const QString& bareJid, const QString& message);
    void callConnect();
    void callFinished();
    void callReceived(QXmppCall *call);
    void callStarted(QXmppCall *call);
    void callStateChanged(QXmppCall::State state);
    void videoModeChanged(QIODevice::OpenMode mode);
    void get_rosters_bids();
    void send_message_to(QString bareid, QString source, QString filetype, const QString &text, const QString &time);

private slots:
    void transfer_error_info(QXmppTransferJob::Error);
    void client_error(QXmppClient::Error);
    void message_handle(const QXmppMessage &message);
    void file_receieved(QXmppTransferJob*job);

private:
    QXmppRosterManager* m_roster_manager;
    QXmppTransferManager* m_pic_transfer_manager;
    QXmppCall* m_call;
    QXmppCallManager *m_call_manager;
    QString m_cur_user;
    QXmppConfiguration m_default_config;
    QString m_store_dir;
};

#endif // EBXMPPCLIENT_H
