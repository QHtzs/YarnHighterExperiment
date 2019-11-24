#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui_login = new LoginForm();
    ui_stack = new StackedWidget();
    m_client = new EBXmppClient();
    m_lay = new QGridLayout();

    //忽略ssl错误
    m_client->GetDefaultGlobalConfig().setIgnoreSslErrors(true);

    QWidget *tmp_w = new QWidget(this);
    tmp_w->setLayout(m_lay);
    setCentralWidget(tmp_w);

    update_layout(ui_login);

    QAction* action = new QAction(QIcon(":/icons/exit.png"), "logout", this);
    this->menuBar()->addAction(action);
    connect(action, &QAction::triggered, this, [=](){
        m_client->disconnectFromServer();
        ui_login->empty_tips();
        update_layout(ui_login);
    });

    connect(ui_login,  SIGNAL(signal_to_login(QString,QString)), m_client, SLOT(connectToServerHost(QString,QString)));

    connect(m_client, SIGNAL(connected_status(QString, QString, bool)), ui_login, SLOT(slot_login_result(QString, QString, bool)));

    connect(m_client, &EBXmppClient::connected, this, [&](){
           m_client->get_rosters_bids();
           this->setWindowTitle(m_client->cur_user());
           ui_stack->re_init_window(m_client->cur_user());
           update_layout(ui_stack);
    });

    connect(m_client, &EBXmppClient::logout_mannual,  this, [&](){
        ui_login->empty_tips();
        update_layout(ui_login);
    });

    connect(m_client, SIGNAL(send_data_failed(QString, QString)), ui_stack, SLOT(send_data_failed(QString, QString)));

    connect(m_client, SIGNAL(send_rosters_bids(QStringList)), ui_stack, SLOT(init_all_rosters(QStringList)));

    connect(m_client, SIGNAL(message_come(QString, QString,QString, QString, QString)), ui_stack, SLOT(recieve_messages(QString, QString, QString, QString, QString)));

    connect(ui_stack, SIGNAL(send_message_to(QString, QString, QString, QString, QString)), m_client, SLOT(send_message_to(QString, QString, QString, QString, QString)));
}

MainWindow::~MainWindow()
{
    ui_login->deleteLater();
    ui_stack->deleteLater();
    m_client->disconnectFromServer();
    m_client->deleteLater();
    m_lay->deleteLater();
    delete ui;
}

void MainWindow::update_layout(QWidget *widget){
    for(int i=m_lay->count()-1; i>=0; i--){
       QWidget *w = m_lay->itemAt(i)->widget();
       m_lay->removeWidget(w);
       w->setParent(nullptr);
    }
    m_lay->addWidget(widget);
}

