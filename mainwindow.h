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
    void addguess(std::string);
    bool chkguess(std::string, std::string, int);

private slots:
    // file menu
    bool action_newgame();

    // main window
    void btnGuess_click();
    void objects_State(bool);
    void lineGuess_txtchange();

    // help menu
    void action_aboutqt();
    void action_about();
    void action_guide();

    // cheats menu
    void action_showgoal();
    void action_inflives();
    void btnLives_click();
};

#endif // MAINWINDOW_H
