/**
 * @file main.cpp
 * @author Thiago Wesley Pasquim de Melo
 * @author Vinicius Gabriel Giordani Barbosa
 */


#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
