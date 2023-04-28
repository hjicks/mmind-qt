#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    // help menu
    void action_aboutqt();
    void action_about();
    void action_guide();

    // cheats menu
    void action_showgoal();
    void action_inflives();

    // file menu
    bool action_newgame();

    // main window
    void action_guess();
    void action_objstate(bool);
    void addguess(std::string);
    bool chkguess(std::string, std::string, int);
    void cheatgoal();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
