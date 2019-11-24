#ifndef LOGINFORM_H
#define LOGINFORM_H


#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMovie>
#include <QSettings>
#include <QCompleter>
#include <QVariantMap>
#include <QResizeEvent>
#include <QBrush>
#include <QPalette>



namespace Ui {
class LoginForm;
}

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

protected:
    void resizeEvent(QResizeEvent *e);

public slots:
   void empty_tips();

   void set_back_groud_icon(const QString& icon_path);

   void slot_login_result(const QString &un, const QString &pw, bool ret);

signals:
    void signal_to_login(const QString &un, const QString &pw);


private slots:
    void on_pushButton_clicked();

private:
    Ui::LoginForm *ui;
    QVariantMap m_un_pw;
    QMovie* m_movie;
    QString m_store_dir;

    void start_show_graph();
    void stop_show_graph();
};

#endif // LOGINFORM_H
