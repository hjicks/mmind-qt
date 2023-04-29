#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox> /* errors, infos */
#include <QInputDialog> /* getting len */
#include <QApplication> /* seems to be needed in all qt programs */
#include <QTreeWidgetItem> /* guessTree */
#include <QClipboard>
#include <QPalette> /* lcdLabel color */

using namespace std;

string guess, goal;
unsigned int len = 0, chance = 1;
int lives = 0; /* qlcd doesn't accept uint */
bool win = false, anotherchance = true;
double pbar = 0;
QPalette lcolor;
QFont f("Lucida Console");

string gengoal(int len)
{
    string digits;
    for (int i = 0; i < len; i++) {
        digits += to_string(rand() % 10);
    }
    return digits;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    if(!action_newgame()) std::exit(EXIT_SUCCESS);
    setWindowFlags( Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::action_lineGuess_txtchange()
{
    guess = ui->lineGuess->text().toStdString();
    if (guess.empty()) {
        f.setPointSize(12);
        ui->lineGuess->setFont(f);
    }
    else if (guess.length() == len) {
        f.setPointSize(28);
        ui->lineGuess->setFont(f);
        ui->buttonGuess->setEnabled(true);
    }
    else {
        f.setPointSize(28);
        ui->lineGuess->setFont(f);
        ui->buttonGuess->setEnabled(false);
	}
}

void MainWindow::action_objstate(bool state)
{
    ui->buttonGuess->setEnabled(state);
    ui->lineGuess->setEnabled(state);
    ui->textChecked->setEnabled(state);
    ui->treeHistory->setEnabled(state);
}

void MainWindow::addguess(string guess)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(to_string(chance)));
    item->setText(1, QString::fromStdString(guess));
    item->setText(2, ui->textChecked->toPlainText());
    ui->treeHistory->addTopLevelItem(item);
}

bool MainWindow::chkguess(string guess, string goal, int len) {
    QString result;
    pbar = 0;
    for (int i = 0; i < len; i++)
    {
        if (guess[i] == goal[i])
        {
            result += "<font color=darkgreen>#</font>";
            pbar++;
        }
        else result += "<font color=darkred>X</font>";
    }
    ui->textChecked->setHtml(result);
    if (guess == goal)  return true;
    else return false;
}

/* help menu */
void MainWindow::action_aboutqt()
{
    QApplication::aboutQt();
}

void MainWindow::action_about()
{
    QMessageBox m;
    m.setTextFormat(Qt::RichText);
    m.setWindowTitle("About");
    m.setText("MasterMind-Qt v2.4<br>"
              "Developed by:<br>"
              "mkf and<br>"
              "MH<br>"
              "Github repository:<br>"
              "<a href='https://github.com/hjdicks/mmind-qt'>Github repo</a>");
    m.exec();
}

void MainWindow::action_guide()
{
    QMessageBox m;
    m.setWindowTitle("Help");
    m.setText("Our MasterMind is a simple game. When the game starts,\n"
              "enter length of the number you gonna guess. Then,\n"
              "You can enter your guesses on the box signed with 0.\n"
              "To submit your guess, click the guess button or hit\n"
              "enter when focus is on your guess box.\n"
              "Result of your guess will be shown below box\n"
              "of your guess. Also, you can check guess history in\n"
              "guesses tree.\n"
              "Do not forget to check the Cheats menu!\n"
              "Maybe you can unlock our hidden cheat...");
    m.setIcon(QMessageBox::Question);
    m.addButton("Ok", QMessageBox::AcceptRole);
    m.exec();
}

/* cheats menu, action_livesButton secrectly enables it */
void MainWindow::action_buttonLives()
{
    if(ui->menuCheats->isEnabled())
    {
        QMessageBox m;
        m.setWindowTitle("Secret menu");
        m.setText("Take a look at cheats menu instead.");
        m.setIcon(QMessageBox::Information);
        m.addButton("ooooo!", QMessageBox::AcceptRole);
        m.exec();
        return;
    }
    ui->menuCheats->setEnabled(true);
}

void MainWindow::action_showgoal()
{
    QMessageBox m;
    QAbstractButton *copy;
    m.setWindowTitle("Cheat");
    m.setText(QString::fromStdString("It's " + goal + " you cheater!"));
    m.setIcon(QMessageBox::Information);
    copy = m.addButton("Copy to clipboard (that lazy?!)", QMessageBox::YesRole);
    m.addButton("Thanks", QMessageBox::AcceptRole);
    m.exec();
    if(m.clickedButton() == copy)
    {
        QClipboard *cb = QGuiApplication::clipboard();
        cb->setText(QString::fromStdString(goal));
    }
    ui->actionShow_Goal->setEnabled(true);
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
    lcolor.setColor(QPalette::WindowText, Qt::blue);
    ui->lcdLives->setPalette(lcolor);
    ui->lcdLives->display(9);
    /* if the game was over */
    action_objstate(true);
}

/* file menu */
bool MainWindow::action_newgame()
{
    bool res;
    len = QInputDialog::getInt(this, "MasterMind", "Enter Length of number:", 2, 2, 10, 1, &res);
    if(res)
    {
        if (len < 10) lives = len;
        else lives = 9;
        chance = 1;
        goal = gengoal(len);
        win = false;
        ui->setupUi(this);
        ui->lcdLives->display(lives);
        ui->lineGuess->setMaxLength(len);
        ui->lineGuess->setFocus();
        anotherchance = true;
        QString fill = "?";
        ui->textChecked->setText(fill.repeated(len));
        ui->lineGuess->setPlaceholderText("Enter your guess here");
    }
    ui->lineGuess->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$"),this));

    return res;
}

/* main window */
void MainWindow::action_guess()
{

    for(int h = 0; h < ui->treeHistory->topLevelItemCount(); h++)
    {
        QTreeWidgetItem *item = ui->treeHistory->topLevelItem(h);
        item->setSelected(false);
        if (guess == item->text(1).toStdString()) {
            item->setSelected(true);
            QMessageBox m;
            m.setWindowTitle("Guess Repeated");
            m.setText("Your guess is repeated.");
            m.setIcon(QMessageBox::Warning);
            m.addButton("Ok", QMessageBox::AcceptRole);
            m.exec();
            return;
        }
    }

    bool res = chkguess(guess, goal, len);
    ui->lcdLives->display(--lives);
    pbar = ((pbar/len) * 100);
    ui->progressBar->setValue(pbar);

    if(res)
    {
       win = true;
       addguess(guess);
       action_objstate(false);
       lcolor.setColor(QPalette::WindowText, Qt::black);
       ui->lcdLives->setPalette(lcolor);
       QMessageBox m;
       m.setWindowTitle("YOU WIN!");
       m.setText("I am pleased to assure you, that; you have managed to win!");
       m.setIcon(QMessageBox::Information);
       m.addButton("Cool!", QMessageBox::AcceptRole);
       m.exec();
    }
    else if (!res && lives == 0)
    {

        addguess(guess);
        action_objstate(false);

        QMessageBox m;
        QAbstractButton *retry, *newgame, *leave;
        m.setWindowTitle("YOU LOST!");
        m.setIcon(QMessageBox::Critical);
        if (anotherchance) {
            m.setText("I am afraid, you lost.\n"
                      "Game over buddy.");
            retry = m.addButton("Another chance?", QMessageBox::AcceptRole);
        }
        else m.setText("I am afraid, you REALLY lost.\n"
                      "Goal was: " + QString::fromStdString(goal));
        newgame = m.addButton("New game and i shall win!", QMessageBox::DestructiveRole);
        leave = m.addButton("Ok", QMessageBox::RejectRole);
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

    if (lives == 1 && !win) {
        lcolor.setColor(QPalette::WindowText, Qt::darkRed);
        ui->lcdLives->setPalette(lcolor);
    }
}
