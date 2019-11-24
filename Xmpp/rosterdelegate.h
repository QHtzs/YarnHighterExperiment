#ifndef ROSTERDELEGATE_H
#define ROSTERDELEGATE_H

#include <QObject>
#include <QPainter>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QListView>

class MListItemModel: public QStandardItemModel
{
    Q_OBJECT
public:
    explicit MListItemModel(QObject *parent);

    void AddItemRefer(const QString &bid);
    void ReleaseItemRefer(const QString &bid);
    void RemoveItem(const QString &bid);

    QStringList GetText(int ind);

    void sort_items(const QString &s);

private:
    void update_date(const QString &bid, int num);


};

class RosterDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit RosterDelegate(MListItemModel* model,  QObject *parent = nullptr);

    void SetFilterWord(const QString &s);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

private:
    MListItemModel* m_model;
    QString m_keyword;
};

#endif // ROSTERDELEGATE_H
