#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "loginui.h"
#include "ui_loginui.h"
#include "HallInterface.h"
#include <QMainWindow>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(QWidget* parent = nullptr,Hall* hall = nullptr);
    ~MainWindow();
    void on_logout_button_clicked();
    void on_enter_game_button_clicked();
    void on_leave_words_button_clicked();
    void show_rank();
    void show_words();
    void show_playerList();
public slots:
    void show_logout_res(int code);
    void show_enter_game_res(int code);
    void show_leave_words_res(int code);
    void show_hall_state(int code);
private:
    Hall *hall;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
