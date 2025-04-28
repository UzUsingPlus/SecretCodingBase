#ifndef GAMEUI_H
#define GAMEUI_H

#include <QWidget>
#include "ui_gameui.h"
#include "entity.h"
class Hall ;
namespace Ui {
    class GameUi;
}
class Hall ;
class GameUi : public QWidget
{
    Q_OBJECT
public:
    explicit GameUi(QWidget *parent = nullptr,Hall* hall = nullptr);
private:
    Hall* hall ;
    Ui::GameUi *ui ;
    void on_beginGameButton_clicked();
    void on_leaveGameButton_clicked();
public :
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* obj,QEvent* event) override ;
public slots:
    void show_leave_game_res(int code);
    void show_begin_game_res(int code);
    void show_game_state(int code);
    void end_the_game(int code);
};

#endif // GAMEUI_H

