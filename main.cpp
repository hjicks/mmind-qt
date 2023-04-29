#include "mainwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>

using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
