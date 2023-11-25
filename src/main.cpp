#include "qtarch.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        qDebug() << "Usage qtarch: <core> <game>";
        return 0;
    }
    QApplication a(argc, argv);
    qtarch qtarch(argv[1], argv[2]);
    qtarch.show();
    return a.exec();
}
