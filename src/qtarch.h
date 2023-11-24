#ifndef QTARCH_H
#define QTARCH_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class qtarch;
}
QT_END_NAMESPACE

class qtarch : public QWidget
{
    Q_OBJECT

public:
    qtarch(QWidget *parent = nullptr);
    ~qtarch();

private:
    Ui::qtarch *ui;
};
#endif // QTARCH_H
