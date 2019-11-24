#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QListView>


#include <QStandardItemModel>
#include <QStandardItem>
#include <QItemDelegate>
#include <QPainter>
#include <QMap>

class ImageDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    ImageDelegate(QStandardItemModel* model, QObject *parent=nullptr);

    void cleartmp();

public slots:
    void setCurIndex(const QModelIndex &index, const QModelIndex & _1);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

private:
    QStandardItemModel* m_model;
    int cur_index;
};


class ImageView: public QListView
{
    Q_OBJECT
public:
    explicit ImageView(QWidget *parent=nullptr);
    ~ImageView();
    QString GetText(const QModelIndex &index);

public slots:
    void UpdateImage(QStringList filenames);
    void setCurIndex(const QModelIndex &index);

private:
    QStandardItemModel *m_model;
    ImageDelegate* m_delegate;
};

#endif // IMAGEVIEW_H
