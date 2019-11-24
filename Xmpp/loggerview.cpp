#include "loggerview.h"

static QMap<QString, QImage> m_cache_image;


LogDelegate::LogDelegate(QStandardItemModel* model, QObject *parent)
    :QItemDelegate(parent)
{
    m_model = model;
    error_img = QImage(":/icons/pic.png");
}


void LogDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0){
        QStandardItem *itm = m_model->item(index.row(), 0);
        QVariant v = itm->data(Qt::UserRole);
        if(v.isValid()){
            QString filetype = v.toString();
            QRect rect = option.rect;
            if(filetype=="image"){
                QImage im;
                bool nctn = false;
                if(m_cache_image.contains(itm->text())){
                    im = m_cache_image.value(itm->text());
                }else{
                    im = QImage(itm->text());
                    nctn = true;
                }
                if(!im.isNull()){
                    int width = std::min(im.width(), rect.width());
                    int height = im.height() * width/im.width() ;
                    im = im.scaled(width, height);
                    itm->setSizeHint(QSize(width, height));
                    painter->drawImage(rect.x(), rect.y(), im);
                    if(nctn) m_cache_image[itm->text()] = im;
                    return;
                }else{
                    int x = rect.x()+rect.width()-error_img.width()-4;
                    x = x>0 ? x:0;
                    painter->drawImage(x, rect.y(), error_img);
                    return; //not draw again
                }
            }
        }
    }
    QItemDelegate::paint(painter, option, index);
}

QWidget *LogDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    Q_UNUSED(parent);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return nullptr;
}


LoggerView::LoggerView(QWidget *parent) : QListView(parent)
{
    setSelectionMode(QAbstractItemView::NoSelection);
    m_model = new QStandardItemModel(this);
    m_delegate = new LogDelegate(m_model, this);
    this->setModel(m_model);
    this->setItemDelegate(m_delegate);
}

LoggerView::~LoggerView()
{
    m_delegate->deleteLater();
    m_model->deleteLater();
}

const QString& LoggerView::GetBid() const
{
    return m_bid;
}

void LoggerView::SetBid(const QString &bid){
    m_model->clear();
    this->m_bid = bid;
}

void LoggerView::toBottom()
{
    this->scrollToBottom();
}

void LoggerView::AddLog(QString SOURCE, QString FILETYPE, QString RECORDTEXT, QString RECORDTIME, QString STATUS)
{
    QString title = SOURCE + "  " + RECORDTIME;
    QStandardItem *it;
    if(STATUS=="0"){
        it = new QStandardItem(title);
        it->setTextAlignment(Qt::AlignRight);
    }else if(STATUS=="2"){
        it = new QStandardItem(title + " [x]");
        it->setTextAlignment(Qt::AlignRight);
    }else{
        it = new QStandardItem(title);
    }
    it->setForeground(QBrush(Qt::blue));
    m_model->appendRow(it);
    QStandardItem* item = new QStandardItem(RECORDTEXT);
    item->setData(QVariant(FILETYPE), Qt::UserRole);
    if(STATUS != "1")item->setTextAlignment(Qt::AlignRight);
    m_model->appendRow(item);
}
