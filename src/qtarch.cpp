#include "qtarch.h"
#include "./ui_qtarch.h"

qtarch::qtarch(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::qtarch)
{
    ui->setupUi(this);
}

qtarch::~qtarch()
{
    delete ui;
}
