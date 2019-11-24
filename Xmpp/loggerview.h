#ifndef LOGGERVIEW_H
#define LOGGERVIEW_H

#include <QListView>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QItemDelegate>
#include <QPainter>
#include <QMap>

class LogDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    LogDelegate(QStandardItemModel* model, QObject *parent=nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

private:
    QStandardItemModel* m_model;
    QImage error_img;
};


class LoggerView : public QListView
{
    Q_OBJECT

public:
    explicit LoggerView(QWidget *parent = nullptr);

    ~LoggerView();

    void AddLog(QString SOURCE, QString FILETYPE, QString RECORDTEXT, QString RECORDTIME, QString STATUS);

    const QString& GetBid() const;

    void SetBid(const QString &bid);

    void toBottom();

private:
    QString m_bid;
    QStandardItemModel* m_model;
    LogDelegate* m_delegate;
};

#endif // LOGGERVIEW_H
