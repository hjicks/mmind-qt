#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QTreeWidgetItem>
#include <iostream>>
int len, goal, guess, lives;

/* generates a goal, n digits long */
ulong gengoal(ulong n)
{
    ulong num = 0;
    /* it's technically legal, but
    we get a int that's shorter than n */
    do
        num = rand() % 10;
    while (num == 0);

    /* we have already generated one digit */
    while(n > 1)
    {
        num *= 10;
        num += rand() % 10;
        n--;
    }
    return num;
}

uint length(ulong n)
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
int digit(long number, uint index)
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
void anotherround()
{
    srand(time(nullptr));
    switch(rand % 2)
    {

    }
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /* dear Qt, with all due respect, why you on earth
     * you try to make my life so misearable? */
    bool res = 0;
    do
    {
        len = QInputDialog::getInt(0, "MasterMind", "Enter Length of number:", 2, 2, 9, 1, &res);
    }while(!res);
    ui->setupUi(this);
    lives = len;
    goal = gengoal(len);
    ui->lcdLives->display(lives);
    ui->spinGuess->setMaximum(pow(10, len) - 1);
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
	QMessageBox::about(this, "about", "MasterMind-qt r1");
}

void MainWindow::action_guide(){}

// cheats menu
void MainWindow::action_showgoal()
{
    QString g = QString::fromStdString("It's " + std::to_string(goal) + " you cheater");
    QMessageBox::information(this, "Cheat", g, "k");
}

void MainWindow::action_hesoyam(){
    lives = INT_MAX;
}

// file menu
void MainWindow::action_newgame(){}
void MainWindow::action_giveup(){}

// main window
void MainWindow::action_guess()
{
    guess = ui->spinGuess->text().toInt();
    if(guess == goal)

       return;
    else if(lives == 1)
    {
        lives--;
        /* :( */
        QMessageBox::critical(this, "MasterMind", "I'm afraid you have failed to achive what"
                                                  " you have tried to do, and now we are all in"
                                                  " brink of impading doom...", "k", "Oh no, "
                                                  "how shall i endure such shame...");
    }
    else
    {
        int i = length(goal) - 1;
        QString s;
        while(i+1 > 0)
        {
            if(digit(goal, i) == digit(guess, i))
                s += '#';
            else
                s += 'X';
            i--;
        }
        MainWindow::ui->textChecked->setText(s);
        ui->lcdLives->display(--lives);
        QTreeWidgetItem *n = new QTreeWidgetItem();
        n->setText(0, QString::fromStdString(std::to_string(len - lives)));
        n->setText(1, QString::fromStdString(std::to_string(guess)));
        n->setText(2, s);
        ui->treeHistory->addTopLevelItem(n);
    }
}
