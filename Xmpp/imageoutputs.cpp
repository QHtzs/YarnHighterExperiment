#include "imageoutputs.h"

ImageOutPuts::ImageOutPuts(QWidget *parent) : QWidget(parent)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);
    m_timer->setInterval(50);
    // max  20frame/seconds
    connect(m_timer, &QTimer::timeout, this, [&](){update();});
    m_rcu_queue.enqueue(nullptr);
}

ImageOutPuts::~ImageOutPuts()
{
    //m_timer->deleteLater();
    while (!m_rcu_queue.empty()) {
        QPixmap *ptr = m_rcu_queue.dequeue();
        if(ptr)delete ptr;
    }
}

void ImageOutPuts::StartPlay()
{
    m_timer->start();
}

void ImageOutPuts::PausePlay()
{
    m_timer->stop();
}

void ImageOutPuts::SetUpdateRate(int freq){
    freq = freq > 0 ? freq : 1;
    int msec = 1000/freq;
    if(msec < 50){
        msec = 50;
    }
    m_timer->setInterval(msec);
}

void ImageOutPuts::UpdatePicturesAndAutoDelete(QPixmap *pixmap)
{
    if(m_rcu_queue.size()>20){
        QPixmap *ptr = m_rcu_queue.dequeue();
        if(ptr) delete  ptr;
    }
    m_rcu_queue.enqueue(pixmap);
}

void ImageOutPuts::paintEvent(QPaintEvent* e){
    QPainter painter(this);
    if(m_rcu_queue.last())painter.drawPixmap(0, 0, *m_rcu_queue.last()); //error will occour if it renders too slower;
    e->accept();
}
