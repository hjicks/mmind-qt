#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QTreeWidgetItem>
#include <QDialogButtonBox>

int len = 0, goal = 0, guess = 0, lives = 0;
bool win;


/* generates a goal, n digits long */
ulong gengoal(ulong n)
{
    ulong num = 0;
    /* it's technically legal, but
    we get a int that's shorter than n */
    do
        num = arc4random() % 10;
    while (num == 0);

    /* we have already generated one digit */
    while(n > 1)
    {
        num *= 10;
        num += arc4random() % 10;
        n--;
    }
    return num;
}

int length(ulong n)
{
    ulong t = 0;
    while(n > 0)
    {
        n /= 10;
        t++;
    }
    return t;
}

/* we could also use arrays for this,
    alas, that would a can of worms
    digit(number, index) = number[index] */
int digit(long number, int index)
{
    if((index > length(number)) || (index < 0))
        return -1;

    int i = 0, j;
    while(i <= index)
    {
        j = number % 10;
        number /= 10;
        i++;
    }
    return j;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /* dear Qt, with all due respect, why you on earth
     * you try to make my life so misearable? */
    action_newgame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// file menu
void MainWindow::action_aboutqt()
{
    QApplication::aboutQt();
}

void MainWindow::action_about()
{
    QMessageBox::about(this, "about", "MasterMind-qt r2");
}

void MainWindow::action_guide(){}

// cheats menu
void MainWindow::action_showgoal()
{
    QMessageBox m;
    m.setWindowTitle("Cheat");
    m.setText(QString::fromStdString("It's " + std::to_string(goal) + " you cheater"));
    m.setIcon(QMessageBox::Information);
    m.addButton("k", QMessageBox::AcceptRole);
    m.exec();
}

void MainWindow::action_inflives(){
    if(lives > 9)
    {
        QMessageBox m;
        m.setWindowTitle("Cheat");
        m.setText("Oi mate, it is already enabled!");
        m.setIcon(QMessageBox::Information);
        m.addButton("oh huh", QMessageBox::AcceptRole);
        m.exec();
        return;
    }
    else if(win)
        return;
    lives = 999;
    ui->lcdLives->display(9);
    /* if the game is over */
    ui->buttonGuess->setEnabled(true);
    ui->spinGuess->setEnabled(true);
    ui->textChecked->setEnabled(true);
}

// file menu

void MainWindow::action_newgame()
{
    bool res = 0;
    do
    {
        len = QInputDialog::getInt(0, "MasterMind", "Enter Length of number:", 2, 2, 9, 1, &res);
    } while(!res);
    ui->setupUi(this);
    lives = len;
    goal = gengoal(len);
    win = false;
    ui->lcdLives->display(lives);
    ui->spinGuess->setMaximum(pow(10, len) - 1);
    ui->spinGuess->setMinimum(pow(10, len - 1));

}
void MainWindow::action_giveup(){
    QMessageBox m;
    m.setText("But-But... are you really totally 100% completely sure about it?");
    m.setIcon(QMessageBox::Question);
    m.addButton(QString::fromStdString("Yea"), QMessageBox::AcceptRole);
    m.addButton(QString::fromStdString("Nay!"), QMessageBox::RejectRole);
    switch(m.exec())
    {
        case QMessageBox::AcceptRole:
            action_newgame();
    }
}

// main window
void MainWindow::action_guess()
{
    QString s;
    guess = ui->spinGuess->text().toInt();
    if(guess == goal)
    {
       win = true;
       s = '#';
       ui->textChecked->setText(s.repeated(len));
       ui->buttonGuess->setEnabled(false);
       ui->spinGuess->setEnabled(false);

       QMessageBox m;
       m.setWindowTitle("YOU WIN!");
       m.setText("I am, as your servent pleased "
                 "to assure you, that; you have managed to win.");
       m.setIcon(QMessageBox::Information);
       m.addButton(QString::fromStdString("umm now what..."), QMessageBox::AcceptRole);
       m.exec();
       // XXX: high scores?
    }
    else if(lives == 1)
    {
        /* :( */
        ui->lcdLives->display(--lives);
        ui->buttonGuess->setEnabled(false);
        ui->spinGuess->setEnabled(false);

        QMessageBox m;
        m.setWindowTitle("INSERT COIN");
        m.setText("I'm afraid you have failed to achive what"
                  " you have tried to do, and now we are all in"
                  " brink of impading doom...");
        m.setIcon(QMessageBox::Critical);
        m.addButton(QString::fromStdString("another chance?"), QMessageBox::AcceptRole);
        m.addButton(QString::fromStdString("dices were packed! new game and i shall win!"), QMessageBox::RejectRole);
        m.addButton(QString::fromStdString("God has forsaken me, *leaving in shame*"), QMessageBox::DestructiveRole);
        switch(m.exec())
        {
            case QMessageBox::AcceptRole:
                // XXX live++
                break;
            case QMessageBox::RejectRole:
                action_newgame();
                break;
            case QMessageBox::DestructiveRole:
                close();
                break;
        }
    }
    else
    {
        int i = length(goal) - 1;
        while(i + 1 > 0)
        {
            if(digit(goal, i) == digit(guess, i))
                s += '#';
            else
                s += "<font color=red>X</font>";
            i--;
        }
        ui->textChecked->setHtml(s);
        ui->lcdLives->display(--lives);
        QTreeWidgetItem *n = new QTreeWidgetItem();
        n->setText(0, QString::fromStdString(std::to_string(len - lives)));
        n->setText(1, QString::fromStdString(std::to_string(guess)));
        n->setText(2, ui->textChecked->toPlainText());
        ui->treeHistory->addTopLevelItem(n);
    }
}
