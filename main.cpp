#include "mainwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>

using namespace std;
// I dare not to take a look at what i've done
// todo: code now looks really really... ugly
// move aux funcations into a .h file,
// try to refactor it
// debug it, if you can
// remove resize grip
// XXXs in code

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
