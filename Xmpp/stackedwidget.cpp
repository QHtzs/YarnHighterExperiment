#include "stackedwidget.h"
#include "ui_stackedwidget.h"

#include <QCompleter>
#include <QDir>
#include <QDebug>


StackedWidget::StackedWidget(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::StackedWidget)
{
    ui->setupUi(this);
    setCurrentIndex(0);
    setAutoFillBackground(true);
    ui->RosterNames->setSelectionMode(QAbstractItemView::NoSelection);
    //call , feature not add now
    ui->BtnCall->setEnabled(false);
    ui->BtnCall->setVisible(false);
#ifndef ANDRIOD
    ui->ImageDir->setText(QDir::currentPath());
#else
    ui->ImageDir->setText("/storage/emulated/0/DCIM");
#endif
    QString cur_dir = ui->ImageDir->text();
    QDir dir(cur_dir);

#ifndef ANDRIOD
    if(dir.exists()){
        QStringList filters;
        QStringList subdirs = dir.entryList(filters, QDir::Dirs, QDir::Name);
        for(int i=0; i<subdirs.size(); i++) subdirs[i] = cur_dir + "/" + subdirs[i];
        ui->ImageDir->setCompleter(new QCompleter(subdirs));
    }
#else
    QStringList subdirs;
    subdirs << "/storage/emulated/0/DCIM/Camera";
    subdirs << "/storage/emulated/0/QxmppData";
    ui->ImageDir->setCompleter(new QCompleter(subdirs));
#endif

}

StackedWidget::~StackedWidget()
{
    delete ui;
}

void StackedWidget::re_init_window(const QString& bid)
{
    setWindowTitle(bid);
    ui->RosterNames->InitRecordDB(bid);
}

void StackedWidget::resizeEvent(QResizeEvent* e){
    page1_resize(e);
    page2_resize(e);
    page3_resize(e);
    QPalette plt = palette();
    plt.setBrush(QPalette::Background, QColor(210, 210, 210));
    setPalette(plt);
    e->accept();
}

void StackedWidget::page1_resize(QResizeEvent *e)
{
    QSize sz = e->size();
    ui->Search->setGeometry(10, 5, sz.width()-20, 30);
    ui->RosterNames->setGeometry(10, 35, sz.width()-20, sz.height()-20);
}

void StackedWidget::page2_resize(QResizeEvent *e)
{
    QSize sz = e->size();
    int width =sz.width() - 10;
    ui->Chat_f1->setGeometry(10, 0, width, 40);
    ui->Chat_Name->setGeometry(10, 40, width, 25);
    ui->Chat_log->setGeometry(10, 65, width, sz.height()-170);
    ui->Chat_Edit->setGeometry(10, sz.height()-110, width, 60);
    ui->Chat_f2->setGeometry(10, sz.height()-50, width, 40);
    ui->progressBar->setValue(0);
}

void StackedWidget::page3_resize(QResizeEvent *e)
{
    QSize sz = e->size();
   int width =sz.width() - 10;
   ui->Frame3->setGeometry(5, 0, width, 40);
   ui->listViewImage->setGeometry(5, 40, width, sz.height()-45);
}

void StackedWidget::on_BtnBack1_clicked()
{
    setCurrentIndex(0);
}

void StackedWidget::update_Log_view()
{
    QString bid = ui->Chat_log->GetBid();
    if(bid.isEmpty()) return;
    QList<QStringList> ret = ui->RosterNames->GetRecordData(bid, "");
    QString SOURCE, FILETYPE, RECORDTEXT, RECORDTIME, STATUS;
    for(int i=0; i<ret.size(); i++){
       SOURCE = ret[i][0];
       FILETYPE = ret[i][1];
       RECORDTEXT = ret[i][2];
       RECORDTIME = ret[i][3];
       STATUS = ret[i][4];
       ui->Chat_log->AddLog(SOURCE, FILETYPE, RECORDTEXT, RECORDTIME, STATUS);
    }
    ui->Chat_log->toBottom();
}

void StackedWidget::on_BtnLoadPic_clicked()
{

    QString bid = ui->Chat_log->GetBid();
    if(bid.isEmpty()) return;
#ifndef ANDRIOD
    QString file = QFileDialog::getOpenFileName(ui->ImageDir, "select picture", "", "Image(*.png *.PNG *.jpg *.JPG *.ICON *.icon *.jpeg)");
    if(file.isEmpty()) return;
    emit send_message_to(bid, "QXmpp", "image", file, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    ui->progressBar->setRange(0, 1);
    ui->progressBar->setValue(0);
    ui->progressBar->setStyleSheet("color:white");
    ui->RosterNames->MsgWriteToLog(bid+"/self", "image", file, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "0", false);
    update_Log_view();
#else
    setCurrentIndex(2);
#endif
}

void StackedWidget::file_send_progress(qint64 c, qint64 t){
    if(t==-1) {
        ui->progressBar->setStyleSheet("color:red");
        return;
    }
    int tt = static_cast<int>(t/c);
    if(tt==1){
        return;
    }
    ui->progressBar->setRange(0, tt);
    ui->progressBar->setValue(1);
}

void StackedWidget::on_RosterNames_clicked(const QModelIndex &index)
{
    QString bareid = ui->RosterNames->GetBareId(index);
    if(bareid.isEmpty()) return;
    ui->Chat_Name->setText("chat with [" + bareid + "]");
    setCurrentIndex(1);
    ui->Chat_log->SetBid(bareid);
    update_Log_view();
    ui->RosterNames->SetNewMessageZero(bareid);
}

void StackedWidget::on_Search_textChanged(const QString &arg1)
{
    ui->RosterNames->SetFilterWord(arg1);
}

void StackedWidget::presenceChanged(const QString &bareJid, const QString &resource, int type){
    Q_UNUSED(bareJid);
    Q_UNUSED(resource);
    Q_UNUSED(type);
}

void StackedWidget::recieve_messages(const QString& bid, const QString &source, const QString& filetype, const QString &text, const QString &time){
    QString jid = source.isEmpty()? bid+"/QXmpp":  bid+"/"+source;
    if(!bid.isEmpty() && ui->Chat_log->GetBid()==bid){
        ui->RosterNames->MsgWriteToLog(jid, filetype, text, time, "1", false);
        update_Log_view();
    }else{
        ui->RosterNames->MsgWriteToLog(jid, filetype, text, time, "1", true);
    }
}

void StackedWidget::call_comming(const QString &bareid){
    if(bareid.isEmpty()) return;
    ui->Chat_Name->setText("chat with [" + bareid + "]");
    setCurrentIndex(1);
    ui->Chat_log->SetBid(bareid);
    update_Log_view();
}

void StackedWidget::on_BtnSend_clicked()
{
    QString barejid = ui->Chat_log->GetBid();
    QString text = ui->Chat_Edit->toPlainText();
    if(text.isEmpty()) return;
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    emit send_message_to(barejid, "", "text", text, time);
    ui->RosterNames->MsgWriteToLog(barejid+"/self", "text", text, time, "0", false);
    ui->Chat_Edit->setPlainText("");
    update_Log_view();
}

void StackedWidget::init_all_rosters(QStringList bids)
{
     ui->RosterNames->InitRosters(bids);
}

void StackedWidget::send_data_failed(const QString &bid, const QString &time)
{
    ui->RosterNames->UpdateRecords(bid, time);
}

void StackedWidget::on_BtnToList_clicked()
{
    setCurrentIndex(1);
}

void StackedWidget::on_listViewImage_doubleClicked(const QModelIndex &index)
{
    QString file = ui->listViewImage->GetText(index);
    QString bid = ui->Chat_log->GetBid();
    if(!bid.isEmpty()){
        emit send_message_to(bid, "QXmpp", "image", file, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->progressBar->setRange(0, 1);
        ui->progressBar->setValue(0);
        ui->progressBar->setStyleSheet("color:white");
        ui->RosterNames->MsgWriteToLog(bid+"/self", "image", file, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "0", false);
        update_Log_view();
    }
    setCurrentIndex(1);
}

void StackedWidget::on_ImageDir_editingFinished()
{
    QString cur_dir = ui->ImageDir->text();
    QDir dir(cur_dir);
    if(!dir.exists()) return;
    QStringList filters;
//  QStringList subdirs = dir.entryList(filters, QDir::Dirs, QDir::Name);
//  for(int i=0; i<subdirs.size(); i++) subdirs[i] = cur_dir + "/" + subdirs[i];
//  ui->ImageDir->setCompleter(new QCompleter(subdirs));
    filters  << "*.jpg" << "*.png" << "*.icon";
    QStringList pictures = dir.entryList(filters, QDir::Files, QDir::Name);
    for(int i=0; i<pictures.size(); i++) pictures[i] = cur_dir + "/" + pictures[i];
    ui->listViewImage->UpdateImage(pictures);
}

void StackedWidget::on_listViewImage_clicked(const QModelIndex &index)
{
    ui->listViewImage->setCurIndex(index);
}
