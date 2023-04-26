#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox> /* errors, infos */
#include <QInputDialog> /* getting len */
#include <QApplication> /* seems to be needed in all qt programs */
#include <QTreeWidgetItem> /* guessTree */
#include <QClipboard>

int len = 0, goal = 0, guess = 0, lives = 0;
bool win;


/* generates a goal, n digits long */
ulong gengoal(ulong n)
{
    ulong num = 0;
    /* it's technically legal, but
    we get a int that's shorter than n */
    do
        num = random() % 10;
    while (num == 0);

    /* we have already generated one digit */
    while(n > 1)
    {
        num *= 10;
        num += random() % 10;
        n--;
    }
    return num;
}

/* counts from 1 */
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
    digit(number, index) = number[index]
    counts from 0 btw */
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
    if(!action_newgame())
    {
        /* one might wonder, why we didn't used QApplication::exit(),
         * or MainWindow's close(), destory() etc.
         * because, we don't have acess to original QApplicaiton
         * nor MainWindow has fully created to be able to exit it.
         * and since it's not an error (valid choice it is)
         *  we consider it a great SUCCESS, hence the following: */
        std::exit(EXIT_SUCCESS);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// help menu
void MainWindow::action_aboutqt()
{
    QApplication::aboutQt();
}

void MainWindow::action_about()
{
    QMessageBox::about(this, "about", "MasterMind-qt r2");
}

// XXX
void MainWindow::action_guide()
{
    QMessageBox m;
    m.setWindowTitle("Help? there is no help");
    m.setText("you fell for this this trap? lol. "
              "seriously, remind me to write one");
    m.setIcon(QMessageBox::Information);
    m.addButton(":(", QMessageBox::AcceptRole);
    m.exec();
}

// cheats menu
void MainWindow::action_showgoal()
{
    QMessageBox m;
    QAbstractButton *copy;
    m.setWindowTitle("Cheat");
    m.setText(QString::fromStdString("It's " + std::to_string(goal) + " you cheater"));
    m.setIcon(QMessageBox::Information);
    copy = m.addButton("i'm too lazy to remember that, please copy that to clipboard", QMessageBox::YesRole);
    m.addButton("k", QMessageBox::AcceptRole);
    m.exec();
    if(m.clickedButton() == copy)
    {
        QClipboard *cb = QGuiApplication::clipboard();
        cb->setText(QString::fromStdString(std::to_string(goal)));
    }
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
    /* if the game was over */
    ui->buttonGuess->setEnabled(true);
    ui->spinGuess->setEnabled(true);
    ui->textChecked->setEnabled(true);
}

// file menu

bool MainWindow::action_newgame()
{
    bool res;
    len = QInputDialog::getInt(this, "MasterMind", "Enter Length of number:", 2, 2, 9, 1, &res);
    if(res)
    {
        lives = len;
        goal = gengoal(len);
        win = false;
        ui->setupUi(this);
        ui->lcdLives->display(lives);
        ui->spinGuess->setMaximum(pow(10, len) - 1);
        ui->spinGuess->setMinimum(pow(10, len - 1));
    }
    /* you pressed cancel "by mistake" ? too bad! your fault. */
    return res;
}
void MainWindow::action_giveup()
{
    QMessageBox m;
    QAbstractButton *y;
    m.setText("B-but... are you really totally completely 100% sure about it?");
    m.setIcon(QMessageBox::Question);
    y = m.addButton("Yea", QMessageBox::AcceptRole);
    m.addButton("Nay!", QMessageBox::RejectRole);
    m.exec();
    if(m.clickedButton() == y)
    {
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
       ui->textChecked->setEnabled(false);
       ui->treeHistory->setEnabled(false);

       QMessageBox m;
       m.setWindowTitle("YOU WIN!");
       m.setText("I am, as your servent pleased "
                 "to assure you, that; you have managed to win.");
       m.setIcon(QMessageBox::Information);
       m.addButton("oh yeah", QMessageBox::AcceptRole);
       m.exec();
    }
    else if(lives == 1)
    {
        /* :( */
        ui->lcdLives->display(--lives);
        ui->buttonGuess->setEnabled(false);
        ui->spinGuess->setEnabled(false);
        ui->textChecked->setEnabled(false);
        ui->treeHistory->setEnabled(false);

        QMessageBox m;
        QAbstractButton *retry, *newgame, *leave;
        m.setWindowTitle("INSERT COIN");
        m.setText("I'm afraid you have failed to acheive what"
                  " you had to do, and now we are all in"
                  " brink of impading doom...");
        m.setIcon(QMessageBox::Critical);
        retry = m.addButton("another chance?", QMessageBox::AcceptRole);
        newgame = m.addButton("dices were packed! new game and i shall win!", QMessageBox::RejectRole);
        leave = m.addButton("God has forsaken me, *leaving in shame*", QMessageBox::DestructiveRole);
        m.exec();

        /* fun(?) fact: switch doesn't work with non numerical types! */
        if(m.clickedButton() == retry)
        {
                /* maybe we should move these into a funcation?
                 * or maybe we should not... carry on */
                ui->lcdLives->display(++lives);
                ui->buttonGuess->setEnabled(true);
                ui->spinGuess->setEnabled(true);
                ui->textChecked->setEnabled(true);
                ui->treeHistory->setEnabled(true);
        }
        else if(m.clickedButton() == newgame)
                action_newgame();
        else if(m.clickedButton() == leave)
                close();
    }
    else
    {
        /* len counts from 1, digit counts from 0 */
        int i = len - 1;
        while(i >= 0)
        {
            if(digit(goal, i) == digit(guess, i))
                s += "<font color=darkgreen>#</font>";
            else
                s += "<font color=darkred>X</font>";
           // s += digit(goal, i);
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
