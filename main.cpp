#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (argc >2) {
        return 2;
    }
    const char* filename = ":/imgs/bonsai.json";
    if (argc == 2) {
        filename = argv[1];
    }
    MainWindow w(filename);
    w.resize(1000, 800);
    w.show();
    return a.exec();
}
