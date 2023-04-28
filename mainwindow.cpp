#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QMessageBox> /* errors, infos */
#include <QInputDialog> /* getting len */
#include <QApplication> /* seems to be needed in all qt programs */
#include <QTreeWidgetItem> /* guessTree */
#include <QClipboard>
#include <QPalette>

using namespace std;

string guess, goal;
unsigned int len = 0;
int lives = 0;
int chance = 1;
bool win;
bool anotherchance = true;
bool cheat = false;
bool mcheat = false;
QPalette lcolor;

string gengoal(int len) {
    srand(time(0));
    string digits;
    for (int i = 0; i < len; i++) {
        string rnd = to_string(rand() % 10);
        digits += rnd;
    }
    return digits;
}

void MainWindow::cheatgoal()    {
    QMessageBox m;
    QAbstractButton *copy;
    m.setWindowTitle("Cheat");
    m.setText(QString::fromStdString("It's " + goal + " you cheater!"));
    m.setIcon(QMessageBox::Information);
    copy = m.addButton("Copy to clipboard (that lazy?!)", QMessageBox::YesRole);
    m.addButton("Fine", QMessageBox::AcceptRole);
    m.exec();
    if(m.clickedButton() == copy)
    {
        QClipboard *cb = QGuiApplication::clipboard();
        cb->setText(QString::fromStdString(goal));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_S)
    {
        cheat = true;
    }
}
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_G && cheat)
    {
        cheatgoal();
        cheat  = false;
        mcheat = true;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    if(!action_newgame())
    {
        std::exit(EXIT_SUCCESS);
    }
    setWindowFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint );
    ui->buttonGuess->setAutoDefault(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::action_objstate(bool state) {
    ui->buttonGuess->setEnabled(state);
    ui->lineGuess->setEnabled(state);
    ui->textChecked->setEnabled(state);
    ui->treeHistory->setEnabled(state);
}

void MainWindow::addguess(string guess) {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(to_string(chance)));
    item->setText(1, QString::fromStdString(guess));
    item->setText(2, ui->textChecked->toPlainText());
    ui->treeHistory->addTopLevelItem(item);
}

bool MainWindow::chkguess(string guess, string goal, int len) {
    string result;
    for (int i = 0; i < len; i++) {
        if (guess[i] == goal[i]) {
            result += "<font color=darkgreen>#</font>";
        }
        else { result += "<font color=darkred>X</font>"; }
    }
    ui->textChecked->setHtml(QString::fromStdString(result));
    if (guess == goal)  return true;
    else {

        return false;
    }
}

// help menu
void MainWindow::action_aboutqt()
{
    QApplication::aboutQt();
}

void MainWindow::action_about()
{
    QMessageBox::about(this, "about", "MasterMind-qt V2.2");
}

// XXX
void MainWindow::action_guide()
{
    QMessageBox m;
    m.setWindowTitle("Help");
    m.setText("you fell for this this trap? lol. "
              "seriously, remind me to write one");
    m.setIcon(QMessageBox::Information);
    m.addButton(":(", QMessageBox::AcceptRole);
    m.exec();
}

// cheats menu
void MainWindow::action_showgoal()
{
    if (mcheat) {
        cheatgoal();
    }
    else {
        QMessageBox m;
        m.setWindowTitle("Cheat");
        m.setText("Developer only! (Maybe you can find the cheat code?)");
        m.setIcon(QMessageBox::Information);
        m.addButton("oh huh", QMessageBox::AcceptRole);
        m.exec();
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
    lcolor.setColor(QPalette::WindowText, Qt::black);
    ui->lcdLives->setPalette(lcolor);
    ui->lcdLives->display(9);
    /* if the game was over */
    chance++;
    action_objstate(true);
}

// file menu
bool MainWindow::action_newgame()
{
    bool res;
    len = QInputDialog::getInt(this, "MasterMind", "Enter Length of number:", 2, 2, 10, 1, &res);
    if(res)
    {
        if (len < 10) lives = len;
        else lives = 9;
        goal = gengoal(len);
        win = false;
        ui->setupUi(this);
        ui->lcdLives->display(lives);
        ui->lineGuess->setMaxLength(len);
        ui->lineGuess->setFocus();
        anotherchance = true;
    }
    ui->lineGuess->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$"),this));

    return res;
}

// main window
void MainWindow::action_guess()
{
    guess = ui->lineGuess->text().toStdString();
    if (guess.length() == len) {
        bool res = chkguess(guess, goal, len);
        ui->lcdLives->display(--lives);
        if(res)
        {
           win = true;
           addguess(guess);
           action_objstate(false);
           QMessageBox m;
           m.setWindowTitle("YOU WIN!");
           m.setText("I am pleased to assure you, that; you have managed to win!");
           m.setIcon(QMessageBox::Information);
           m.addButton("Indeed!", QMessageBox::AcceptRole);
           m.exec();
        }
        else if (!res && lives == 0)
        {

            addguess(guess);
            action_objstate(false);

            QMessageBox m;
            QAbstractButton *retry, *newgame, *leave;
            m.setWindowTitle("YOU LOST!");
            m.setText("I am ashamed to tell you, that; you lost.");
            m.setIcon(QMessageBox::Critical);
            if (anotherchance) retry = m.addButton("Another chance?", QMessageBox::AcceptRole);
            newgame = m.addButton("New game and i shall win!", QMessageBox::DestructiveRole);
            leave = m.addButton("Cancel", QMessageBox::RejectRole);
            m.exec();

            if(anotherchance && m.clickedButton() == retry)
            {       anotherchance = false;
                    ui->lcdLives->display(++lives);
                    action_objstate(true);
                    ui->lineGuess->setFocus();
            }
            else if(m.clickedButton() == newgame)
                    action_newgame();
            else if(m.clickedButton() == leave)
                    m.close();
        }
        else
        {
            addguess(guess);
            chance++;
        }

        if (lives == 1) {
            lcolor.setColor(QPalette::WindowText, Qt::red);
            ui->lcdLives->setPalette(lcolor);
        }
    }
    else {
        QMessageBox m;
        m.setWindowTitle("Invalid!");
        m.setText("Invalid number!");
        m.setIcon(QMessageBox::Information);
        m.addButton("Indeed!", QMessageBox::AcceptRole);
        m.exec();
    }
}
