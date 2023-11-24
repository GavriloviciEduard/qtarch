#include "qtarch.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qtarch w;
    w.show();
    return a.exec();
}
