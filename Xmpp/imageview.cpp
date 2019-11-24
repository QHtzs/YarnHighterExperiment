#include "imageview.h"

static QMap<QString, QImage> m_tmp_image;

ImageDelegate::ImageDelegate(QStandardItemModel* model, QObject *parent)
    :QItemDelegate(parent)
{
    m_model = model;
    cur_index = 0;
}

void ImageDelegate::cleartmp()
{
    m_model->clear();
    //m_tmp_image.clear();
    cur_index = 0;
}

void ImageDelegate::setCurIndex(const QModelIndex &index, const QModelIndex & _1){
    Q_UNUSED(_1);
    cur_index = index.row();
}

void ImageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0){
        QStandardItem *itm = m_model->item(index.row(), 0);
        QImage im;
        bool ctn = false;
        if(m_tmp_image.contains(itm->text())) {
            im = m_tmp_image.value(itm->text());
            ctn = true;
        }
        else{
           if(index.row() < cur_index+2 && index.row() >cur_index-2)im = QImage(itm->text());
        }
        if(!im.isNull()){
            im = im.scaled(100, 100);
            QRect rect = option.rect;
            int width = std::min(im.width(), rect.width());
            itm->setSizeHint(QSize(width, im.height()));
            painter->drawImage(rect.x(), rect.y(), im);
            if(!ctn) m_tmp_image[itm->text()] = im;
            return;
        }
    }
    QItemDelegate::paint(painter, option, index);
}

QWidget *ImageDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    Q_UNUSED(parent);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return nullptr;
}


ImageView::ImageView(QWidget *parent):QListView(parent)
{
    m_model = new QStandardItemModel(this);
    m_delegate = new ImageDelegate(m_model, this);
    this->setModel(m_model);
    this->setItemDelegate(m_delegate);
//    connect(this, SIGNAL(currentChanged(QModelIndex, QModelIndex)),
//            m_delegate, SLOT(setCurIndex(QModelIndex, QModelIndex)));
}

ImageView::~ImageView()
{
    m_model->deleteLater();
    m_delegate->deleteLater();
}

void ImageView::UpdateImage(QStringList filenames)
{
    m_delegate->cleartmp();
    for(QString name: filenames) m_model->appendRow(new QStandardItem(name));
}

QString ImageView::GetText(const QModelIndex &index){
    return m_model->item(index.row(), 0)->text();
}

void ImageView::setCurIndex(const QModelIndex &index)
{
    m_delegate->setCurIndex(index, index);
}
