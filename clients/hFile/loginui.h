#ifndef LOGINUI_H
#define LOGINUI_H
#include "entity.h"
#include <QWidget>
#include <QString>
class Hall ;
namespace Ui {
class LoginUi;
}
class Hall ;
class LoginUi : public QWidget
{
    Q_OBJECT

public:
    explicit LoginUi(QWidget *parent = nullptr);
    LoginUi(QWidget *parent = nullptr,Hall* hall = nullptr);
    ~LoginUi();
    void get_user_info();
    void on_registerButton_clicked();
    void on_loginButton_clicked();
    void on_exitButton_clicked();
    void toHall();
    int waitForRes(int waitTime);
public slots:
    void show_regis_res(int code);
    void show_login_res(int code);
signals:
    void exit();
private:
    Hall* hall ;
    Ui::LoginUi *ui;
};

#endif // LOGINUI_H
