#include "uirosterview.h"

UiRosterView::UiRosterView(QWidget *parent) : QListView(parent)
{
    m_lmodel = new MListItemModel(this);
    m_delegate = new RosterDelegate(m_lmodel, this);
    this->setModel(m_lmodel);
    this->setItemDelegate(m_delegate);
    m_sqlite = new HistoryRecords(this);
}

UiRosterView::~UiRosterView()
{
    m_lmodel->deleteLater();
    m_delegate->deleteLater();
    m_sqlite->deleteLater();
}

void UiRosterView::InitRecordDB(const QString &bid)
{
      QString dbname(bid);
      dbname = dbname.replace('@', '_');
      dbname = dbname.replace('.', '_');
      dbname = dbname.replace(':', '_');
      dbname = "db" + dbname + ".db";
      m_sqlite->ConnectToDb(dbname);
}

void UiRosterView::SetNewMessageZero(const QString &bid)
{
    m_lmodel->ReleaseItemRefer(bid);
}

QString UiRosterView::GetBareId(const QModelIndex &ind)
{
    QStandardItem* itm = m_lmodel->item(ind.row(), 0);
    QVariant v = itm->data(Qt::UserRole);
    if(v.isNull()) return "";
    QVariantMap mp = v.toMap();
    if(mp.empty()) return "";
    return mp.begin().key();
}

QList<QStringList> UiRosterView::GetRecordData(const QString &bid, const QString &source)
{
    return m_sqlite->GetRecordData(bid, source);
}

bool UiRosterView::UpdateRecords(const QString &bid, const QString& time)
{
   return m_sqlite->update_data(bid, time);
}

void UiRosterView::InitRosters(const QStringList &rosters_bid)
{
    m_lmodel->clear();
    for(QString roster: rosters_bid) m_lmodel->ReleaseItemRefer(roster);
}

void UiRosterView::AddRosters(const QString& roster_bid)
{
     m_lmodel->ReleaseItemRefer(roster_bid);
}

void UiRosterView::RemoveRosters(const QString& roster_bid)
{
    m_lmodel->RemoveItem(roster_bid);
}

void UiRosterView::MsgWriteToLog(const QString& jid, const QString &filetype, const QString& text, const QString &time, const QString &msg_status, bool notify)
{
    QString bid = QXmppUtils::jidToBareJid(jid);
    QString source = QXmppUtils::jidToResource(jid);
    if(notify)m_lmodel->AddItemRefer(bid);
    m_sqlite->InsertData(bid, source, filetype, text, time, msg_status);
}

void UiRosterView::SetFilterWord(const QString& s)
{
    m_delegate->SetFilterWord(s);
}

QStringList UiRosterView::GetRostersBareid(){
    QStringList ret;
    for(int i=0; i<m_lmodel->rowCount(); i++){
        QStandardItem* itm = m_lmodel->item(i, 0);
        QVariant v = itm->data(Qt::UserRole);
        if(v.isNull())continue;
        QVariantMap mp = v.toMap();
        if(mp.empty()) continue;
        ret << mp.begin().key();
    }
    return ret;
}
