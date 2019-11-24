#ifndef STACKEDWIDGET_H
#define STACKEDWIDGET_H

#include "loggerview.h"
#include "QXmppClient.h"
#include <QStackedWidget>
#include <QResizeEvent>
#include <QFileDialog>
#include <QButtonGroup>
#include <QCompleter>

namespace Ui {
class StackedWidget;
}

class StackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit StackedWidget(QWidget *parent = nullptr);
    ~StackedWidget();

public slots:
    void re_init_window(const QString& bid);

    void recieve_messages(const QString& bid, const QString &source, const QString& filetype, const QString &text, const QString& time);

    void call_comming(const QString &bid);

    void presenceChanged(const QString &bareJid, const QString &resource, int type);

    void file_send_progress(qint64, qint64);

    void init_all_rosters(QStringList bids);

    void send_data_failed(const QString &bid, const QString &time);

private slots:
    void on_BtnBack1_clicked();

    void on_RosterNames_clicked(const QModelIndex &index);

    void on_Search_textChanged(const QString &arg1);

    void on_BtnLoadPic_clicked();

    void update_Log_view();

    void on_BtnSend_clicked();


    void on_BtnToList_clicked();

    void on_listViewImage_doubleClicked(const QModelIndex &index);

    void on_ImageDir_editingFinished();

    void on_listViewImage_clicked(const QModelIndex &index);

signals:
    void CallCloseOrRefuse();

    void CallAccept();

    void send_message_to(QString bareid, QString source, QString filetype, const QString &text, const QString &time);

    void send_call_to(QString bareid, QString source);

private:
    Ui::StackedWidget *ui;

protected:
    void resizeEvent(QResizeEvent* e);

private:
    void page1_resize(QResizeEvent *e);
    void page2_resize(QResizeEvent *e);
    void page3_resize(QResizeEvent *e);
};

#endif // STACKEDWIDGET_H
