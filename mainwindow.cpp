#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox> /* errors, infos */
#include <QInputDialog> /* getting len */
#include <QApplication> /* seems to be needed in all qt programs */
#include <QTreeWidgetItem> /* guessTree */
#include <QClipboard> /* copying in showgoal() */
#include <QPalette> /* lcdLabel color */

using namespace std;

string guess, goal;
unsigned int len = 0, chance = 1;
int lives = 0; /* qlcd doesn't accept uint */
bool win = false, anotherchance = true;
double pbar = 0;
QPalette lcdcolor;

string gengoal(int len)
{
    string digits;
    for (int i = 0; i < len; i++)
        digits += to_string(rand() % 10);
    return digits;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /* controls cancel on first recceive length dialog
       one might wonder, why we didn't use QApplication::exit(),
       or MainWindow's close(), destory(), etc.
       because, we don't have access to original QApplicaiton
       nor MainWindow has fully created to be able to exit it.
       and since it's not an error (valid choice it is)
       we consider it a SUCCESS, hence the following: */
    if(!action_newgame()) std::exit(EXIT_SUCCESS);
    /* disables maximize button and removes resize grip */
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint
                   | Qt::MSWindowsFixedSizeDialogHint);
}

MainWindow::~MainWindow()
{
    delete ui;
}
 /* enables button guess only when
    guess length is equal to given length */
void MainWindow::lineGuess_txtchange()
{
    guess = ui->lineGuess->text().toStdString();
    if (guess.length() == len)
        ui->buttonGuess->setEnabled(true);
    else
        ui->buttonGuess->setEnabled(false);
}

/* controls state of different objects */
void MainWindow::objects_State(bool state)
{
    ui->buttonGuess->setEnabled(state);
    ui->lineGuess->setEnabled(state);
    ui->textChecked->setEnabled(state);
    ui->treeHistory->setEnabled(state);
    ui->progressBar->setEnabled(state);
}

/* add guesses to tree history */
void MainWindow::addguess(string guess)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(to_string(chance)));
    item->setText(1, QString::fromStdString(guess));
    item->setText(2, ui->textChecked->toPlainText());
    ui->treeHistory->addTopLevelItem(item);
}

/* checks user guesses and generates #X
   also add number of correctly guessed
   digits to pbar */
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
        else
            result += "<font color=darkred>X</font>";
    }
    ui->textChecked->setHtml(result);
    if (guess == goal)
        return true;
    else
        return false;
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
    }
    /* validation of user guess using Regex
       ^ indicates the beginning of the string
       * means any number of the given chatacters
       $ indicates the end of the string
       [] is a list of valid characters,
       or a range of numbers or characters */
    ui->lineGuess->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$"),this));
    return res;
}

/* main window */
void MainWindow::btnGuess_click()
{
    /* this loop checks user guesses not to be repeated */
    for(int h = 0; h < ui->treeHistory->topLevelItemCount(); h++)
    {
        QTreeWidgetItem *item = ui->treeHistory->topLevelItem(h);
        /* deselect prevoius repeated guess */
        item->setSelected(false);
        if (guess == item->text(1).toStdString())
        {
            /* selects user repeated guess to highlight it */
            item->setSelected(true);
            /* guess repeated message box*/
            QMessageBox m;
            m.setWindowTitle("Guess Repeated");
            m.setText("Your guess is repeated.");
            m.setIcon(QMessageBox::Warning);
            m.addButton("Ok", QMessageBox::AcceptRole);
            m.exec();
            return;
        }
    }

    win = chkguess(guess, goal, len);
    ui->lcdLives->display(--lives);

    /* sets progress bar value */
    pbar = ((pbar/len) * 100);
    ui->progressBar->setValue(pbar);

    /* win state */
    if(win)
    {
       win = true;
       addguess(guess);
       /* disable certain objects after user won */
       objects_State(false);
       lcdcolor.setColor(QPalette::WindowText, Qt::black);
       ui->lcdLives->setPalette(lcdcolor);
       /* win message box */
       QMessageBox m;
       m.setWindowTitle("YOU WIN!");
       m.setText("I am pleased to assure you, that; you have managed to win!");
       m.setIcon(QMessageBox::Information);
       m.addButton("Cool!", QMessageBox::AcceptRole);
       m.exec();
    }
    /* lose state */
    else if (!win && lives == 0)
    {
        addguess(guess);
        objects_State(false);
        /* lose message box */
        QMessageBox m;
        QAbstractButton *retry, *newgame, *leave;
        m.setWindowTitle("YOU LOST!");
        m.setIcon(QMessageBox::Critical);
        if (anotherchance)
        {
            m.setText("I am afraid, you lost.\n"
                      "Game over buddy.");
            retry = m.addButton("Another chance?", QMessageBox::AcceptRole);
        }
        else m.setText("I am afraid, you have REALLY lost.\n"
                      "Goal was: " + QString::fromStdString(goal));
        newgame = m.addButton("New game and i shall win!", QMessageBox::DestructiveRole);
        leave = m.addButton("Ok", QMessageBox::RejectRole);
        m.exec();

        /* controls lose message box buttons */
        if(anotherchance && m.clickedButton() == retry)
        {       /* controls to give user only one another chance */
                anotherchance = false;
                ui->lcdLives->display(++lives);
                objects_State(true);
                ui->lineGuess->setFocus();
        }
        else if(m.clickedButton() == newgame)
                action_newgame();
        else if(m.clickedButton() == leave)
                m.close();

    }
    /* gets ready for next guess */
    else
    {
        addguess(guess);
        chance++;
    }
    /* makes lcd lives color red for
       2 last tries. doesn't apply if user has won */
    if (lives == 1 && !win) {
        lcdcolor.setColor(QPalette::WindowText, Qt::darkRed);
        ui->lcdLives->setPalette(lcdcolor);
    }
}

/* cheats menu, action_livesButton secretly enables it */
void MainWindow::btnLives_click()
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
    ui->itemShowGoal->setEnabled(true);
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
    {
        QMessageBox m;
        m.setWindowTitle("Cheat");
        m.setText("You have already won.");
        m.setIcon(QMessageBox::Information);
        m.addButton("Ok", QMessageBox::AcceptRole);
        m.exec();
        return;
    }
    lives = 999;
    lcdcolor.setColor(QPalette::WindowText, Qt::blue);
    ui->lcdLives->setPalette(lcdcolor);
    ui->lcdLives->display(9);
    /* if the game was over */
    objects_State(true);
    lineGuess_txtchange();
}

/* help menu */
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
              "Do not forget to check the Cheats menu;\n"
              "if you can unlock it...");
    m.setIcon(QMessageBox::Question);
    m.addButton("Ok", QMessageBox::AcceptRole);
    m.exec();
}

void MainWindow::action_about()
{
    QMessageBox m;
    m.setTextFormat(Qt::RichText);
    m.setWindowTitle("About");
    m.setText("MasterMind-Qt v3<br>"
              "Developed by:<br>"
              "mkf and<br>"
              "MH<br>"
              "Github repository:<br>"
              "<a href='https://github.com/hjdicks/mmind-qt'>Github repo</a>");
    m.exec();
}

void MainWindow::action_aboutqt()
{
    QApplication::aboutQt();
}
