#include "rosterdelegate.h"

#include <QDebug>


MListItemModel::MListItemModel(QObject *parent)
    :QStandardItemModel(parent)
{
}

void MListItemModel::update_date(const QString &bid, int num)
{
    for(int i=0; i<rowCount(); i++){
        QStandardItem *itm = item(i);
        QVariant v = itm->data(Qt::UserRole);
        if(v.isValid()){
            QVariantMap mp = v.toMap();
            if(mp.contains(bid)){
                if(num==-1){
                    removeRow(i);
                    return;
                }else if(num==0){
                    mp[bid] = 0;
                }else{
                    mp[bid] = 1 + mp[bid].toInt();
                }
               itm->setData(mp, Qt::UserRole);
                return;
            }
        }
    }
    if(num > -1){
        QStandardItem *itm = new QStandardItem("");
        insertRow(0, itm);
        QVariantMap mp;
        mp[bid] = num;
        itm->setData(mp, Qt::UserRole);
    }
}

void MListItemModel::sort_items(const QString &s){
    QList<QStandardItem*> items;
    for(int i=0; i<rowCount(); i++){
        QStandardItem* itm = item(i, 0);
        QStandardItem* vitm = new QStandardItem("");
        items << vitm;
        vitm->setData(itm->data(Qt::UserRole), Qt::UserRole);
    }
    std::sort(items.begin(), items.end(), [&s](QStandardItem* const &t0, QStandardItem* const &t1){
        QVariant v0 = t0->data(Qt::UserRole);
        QVariant v1 = t1->data(Qt::UserRole);
        if(v0.isNull()) return false;
        if(v1.isNull()) return true;
        QVariantMap mp0 = v0.toMap();
        QVariantMap mp1 = v1.toMap();
        if(mp0.empty()) return  false;
        if(mp1.empty()) return true;
        QString bid0 = mp0.keys()[0];
        QString bid1 = mp1.keys()[0];
        if(bid0.contains(s) && bid1.contains(s)) return bid0 < bid1;
        return bid0.contains(s);
    });

    clear();
    for(QStandardItem* itm: items) appendRow(itm);

}

void MListItemModel::AddItemRefer(const QString &bid)
{
    update_date(bid, 1);
}

void MListItemModel::ReleaseItemRefer(const QString &bid)
{
    update_date(bid, 0);
}

void MListItemModel::RemoveItem(const QString &bid)
{
    update_date(bid, -1);
}

QStringList MListItemModel::GetText(int ind)
{
     QStringList ret;
     QStandardItem *itm = item(ind);
     QVariant v = itm->data(Qt::UserRole);
     if(v.isValid()){
        QVariantMap mp = v.toMap();
        if(!mp.empty()){
            ret << mp.begin().key() << QString::number(mp.begin().value().toInt());
        }
     }
     return ret;
}

RosterDelegate::RosterDelegate(MListItemModel* model,  QObject *parent) : QItemDelegate(parent)
{
    m_model = model;

}

void RosterDelegate::SetFilterWord(const QString &s)
{
    m_keyword = s;
    m_model->sort_items(s);
}


QWidget *RosterDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    Q_UNUSED(parent);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return nullptr; //un editor able
}


void RosterDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0){
        QStringList kv = m_model->GetText(index.row());
        if(!kv.empty()){
            QRect rect = option.rect;
            int width = rect.width();
            int height = rect.height();
            if(kv[0].contains(m_keyword)) painter->setPen(Qt::blue);
            QString name = kv[0].split('@')[0];
            painter->drawText(rect.x(),
                              rect.y(),
                              width*2/3,
                              height,
                              Qt::AlignLeft, name);
            if(kv[1].toInt()>0){
                painter->setPen(Qt::red);
                painter->drawText(rect.x()+width*2/3,
                                  rect.y(),
                                  width/3,
                                  height,
                                  Qt::AlignCenter, "( "+kv[1] + " )");
            }
            painter->setPen(Qt::black);
        }
    }
}

