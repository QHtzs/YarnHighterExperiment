#ifndef HISTORYRECORDS_H
#define HISTORYRECORDS_H

/*
*/

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QList>
#include <QStringList>
#include <QDateTime>


class HistoryRecords: public QObject
{
    Q_OBJECT
public:
    HistoryRecords(QObject *parent=nullptr);
    ~HistoryRecords();
    bool ConnectToDb(const QString &dbname);

    bool InsertData(const QString& bid, const QString& source, const QString &filetype, const QString& text, const QString &time,  const QString& ok);

    bool update_data(const QString &bid, const QString& time);

    QList<QStringList> GetRecordData(const QString &bid, const QString &source);

private:
    QSqlDatabase db;
    bool is_table_exist(const QString &bid);
    void create_table(const QString &bid);
    QString std_bid(const QString &bid);
    QStringList m_tables;
};

#endif // HISTORYRECORDS_H
