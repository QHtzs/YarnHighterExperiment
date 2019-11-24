#include "loginform.h"
#include "ui_loginform.h"

#include <QDir>

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);

 #ifdef ANDRIOD
    m_store_dir = "/storage/emulated/0/QxmppData";
 #else
    m_store_dir = QDir::currentPath() + "/QxmppData";
 #endif
    QDir fold;
    if(!fold.exists(m_store_dir)) fold.mkdir(m_store_dir);

    QSettings setting(m_store_dir + "/users.cfg");
    m_un_pw = setting.value("user").toMap();

    QString un = setting.value("last_un").toString();
    QString pw = setting.value("last_pw").toString();
    ui->UserName->setText(un);
    ui->PassWord->setText(pw);


    QCompleter* comple = new QCompleter(m_un_pw.keys(), ui->UserName);
    ui->UserName->setCompleter(comple);

    setAutoFillBackground(true);
    set_back_groud_icon(":/icons/screen.jpg");

    m_movie = new QMovie(":/icons/run.gif", "", this);
    ui->GraphView->setMovie(m_movie);
    m_movie->start();
    ui->GraphView->setVisible(false);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::set_back_groud_icon(const QString& icon_path)
{
    QPalette pale = this->palette();
    QImage im(icon_path);
    im = im.scaled(this->size());
    pale.setBrush(QPalette::Background, QBrush(im));
    setPalette(pale);  
}

void LoginForm::resizeEvent(QResizeEvent *e)
{
    QSize sz = e->size();
    int w = sz.width(), h = sz.height();
    w = (w - 380)/2;
    h = (h - 420)/2;
    w = w > 0 ? w : 0;
    h = h > 0 ? h : 0;
    ui->frame->setGeometry(w, h, 380, 420);
    set_back_groud_icon(":/icons/screen.jpg");
    e->accept();

}

void LoginForm::empty_tips()
{
    ui->Tips->setText("");
}

void LoginForm::start_show_graph()
{
    ui->GraphView->setVisible(true);
}

void LoginForm::stop_show_graph()
{
    ui->GraphView->setVisible(false);
}

void LoginForm::slot_login_result(const QString &un, const QString &pw, bool ret)
{

    if(!ret){
        ui->Tips->setStyleSheet("color:red");
        ui->Tips->setText("failed: time out");
    }else{
        ui->Tips->setStyleSheet("color:green");
        ui->Tips->setText("success");
        if(ui->Remember->isChecked()){
            QSettings setting(m_store_dir + "/users.cfg");
            m_un_pw[un] = pw;
            setting.setValue("user", m_un_pw);
            setting.setValue("last_un", un);
            setting.setValue("last_pw", pw);
        }
    }
    ui->pushButton->setEnabled(true);
    ui->pushButton->setVisible(true);
    stop_show_graph();
}

void LoginForm::on_pushButton_clicked()
{
    ui->Tips->setText("");
    QString un = ui->UserName->text();
    QString pw = ui->PassWord->text();
    if(un.isEmpty() || pw.isEmpty()){
        ui->Tips->setStyleSheet("color:red");
        ui->Tips->setText("username or password is empty");
        return;
    }else{
        emit signal_to_login(un, pw);
        start_show_graph();
        ui->pushButton->setEnabled(false);
        ui->pushButton->setVisible(false);
    }
}
