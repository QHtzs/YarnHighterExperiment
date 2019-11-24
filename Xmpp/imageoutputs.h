#ifndef IMAGEOUTPUT_H
#define IMAGEOUTPUT_H

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QTimer>
#include <QPainter>
#include <QQueue>

//RCU

class ImageOutPuts : public QWidget
{
    Q_OBJECT
public:
    explicit ImageOutPuts(QWidget *parent = nullptr);

    ~ImageOutPuts();

    void SetUpdateRate(int freq);

    void StartPlay();

    void PausePlay();

public slots:
    void UpdatePicturesAndAutoDelete(QPixmap *pixmap);

private:
    QTimer* m_timer;
    QQueue<QPixmap*> m_rcu_queue;

protected:
    void paintEvent(QPaintEvent* e);
};

#endif // IMAGEOUTPUT_H
