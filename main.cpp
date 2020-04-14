#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (argc >2) {
        QString msg;
        msg = msg.asprintf("Usage: %s [filename]\n Default file is a Bonsai CT image.", argv[0]);
        QMessageBox *mbox = new QMessageBox(QMessageBox::Critical,
                                           "HighVis",
                                           msg);
        mbox->exec();
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
