#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QGridLayout>
#include "loginform.h"
#include "stackedwidget.h"
#include "ebxmppclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LoginForm* ui_login;
    StackedWidget* ui_stack;
    EBXmppClient* m_client;
    QGridLayout* m_lay;

    void update_layout(QWidget *widget);

};

#endif // MAINWINDOW_H
