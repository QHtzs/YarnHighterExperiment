#ifndef UIROSTERVIEW_H
#define UIROSTERVIEW_H

#include "rosterdelegate.h"
#include "historyrecords.h"
#include "QXmppUtils.h"
#include <QListView>
#include <QStringList>
#include <QString>


class UiRosterView : public QListView
{
    Q_OBJECT
public:
    explicit UiRosterView(QWidget *parent = nullptr);

    ~UiRosterView();
    void InitRecordDB(const QString &bid);

    QString GetBareId(const QModelIndex &index);

    void SetNewMessageZero(const QString &bid);

    QList<QStringList> GetRecordData(const QString &bid, const QString &source);

    bool UpdateRecords(const QString &bid, const QString& time);

    QStringList GetRostersBareid();

public slots:
    void InitRosters(const QStringList &rosters_bid);

    void AddRosters(const QString& roster_bid);

    void RemoveRosters(const QString& roster_bid);

    void MsgWriteToLog(const QString& jid, const QString &filetype, const QString& text, const QString &time, const QString &msg_status="1", bool notify=true);

    void SetFilterWord(const QString& s);

private:
    MListItemModel *m_lmodel;
    RosterDelegate *m_delegate;
    HistoryRecords *m_sqlite;

};

#endif // UIROSTERVIEW_H
