#include "historyrecords.h"
#include <QDir>

HistoryRecords::HistoryRecords(QObject *parent)
    :QObject(parent)
{
   db = QSqlDatabase::addDatabase("QSQLITE");
}

HistoryRecords::~HistoryRecords()
{
    db.close();
}

bool HistoryRecords::ConnectToDb(const QString &dbname)
{
   if(db.isOpen()) db.close();
   m_tables.clear();
   QDir fold;
   QString dir;
#ifdef ANDRIOD
   dir = "/storage/emulated/0/QxmppData";
#else
   dir = QDir::currentPath() + "/QxmppData";
#endif
   if(!fold.exists(dir)){
       fold.mkdir(dir);
   }
   dir = dir + "/" + dbname;
   db.setDatabaseName(dir);
   bool ret = db.open();
   return ret;
}

bool HistoryRecords::InsertData(const QString& bid, const QString& source,
                                const QString &filetype, const QString& text, const QString &time, const QString& ok)
{
    QString table_name = std_bid(bid);
    if(!m_tables.contains(table_name))
    {
        if(!is_table_exist(bid)) create_table(bid);
        m_tables.push_back(table_name);
    }
    QString sql = QString("insert into %1(SOURCE, FILETYPE, RECORDTEXT, RECORDTIME, STATUS) values('%2', '%3', '%4', '%5', '%6')").arg(
                table_name, source, filetype,  text,
                time,
                ok
                );
    QSqlQuery query(db);
    query.exec(sql);
    return query.lastError().type() == QSqlError::NoError;
}

bool HistoryRecords::update_data(const QString &bid, const QString& time){
    QString table_name = std_bid(bid);
    //status: [1:recieved from other],  send[0:just send, 2:send failed]
    QString sql = QString("update %1 set STATUS='2' where STATUS='0' and RECORDTIME = '%2' ").arg(bid, time);
    QSqlQuery query(db);
    query.exec(sql);
    //qDebug() << "update" << sql << query.lastError();
    return query.lastError().type() == QSqlError::NoError;
}

QList<QStringList> HistoryRecords::GetRecordData(const QString &bid, const QString &source)
{
    QString table_name = std_bid(bid);
    QString sql;
    if(!source.isEmpty()){
         sql = QString("select SOURCE, FILETYPE, RECORDTEXT, RECORDTIME, STATUS from %1 "
                       "where SOURCE='%2' order by ID limit 100").arg(table_name, source);
    }else {
         sql = QString("select SOURCE, FILETYPE, RECORDTEXT, RECORDTIME, STATUS from %1 "
                       "order by ID limit 100").arg(table_name);
    }
    QSqlQuery query(db);
    query.exec(sql);
    QList<QStringList> ret;
    while (query.next()) {
        QStringList tmp;
        tmp << query.value(0).toString()
            << query.value(1).toString()
            << query.value(2).toString()
            << query.value(3).toString()
            <<query.value(4).toString();

        ret << tmp;
    }
    return ret;
}

bool HistoryRecords::is_table_exist(const QString &bid)
{
    QString sql = QString("select * from sqlite_master where type = 'table' and name = '%1'").arg(std_bid(bid));
    QSqlQuery query(db);
    query.exec(sql);
    bool ret = query.next();
    return ret;
}

void HistoryRecords::create_table(const QString &bid)
{
    QString tabe_name = std_bid(bid);
    QString sql = QString("create table %1( "
                  "[ID] integer PRIMARY KEY autoincrement, "
                  "[SOURCE] CHAR(50), "
                  "[RECORDTEXT] TEXT, "
                  "[FILETYPE] CHAR(2),"
                  "[RECORDTIME] CHAR(30),"
                  "[STATUS] CHAR(1)"
                  ")").arg(tabe_name);
    QSqlQuery query(db);
    query.exec(sql);
}

QString HistoryRecords::std_bid(const QString &bid)
{
    QString ret = bid;
    ret = ret.replace('@', "_");
    ret = ret.replace('/', "_");
    ret = ret.replace(':', "_");
    ret = ret.replace('.', "_");
    ret = "a"+ret;
    return ret;
}
