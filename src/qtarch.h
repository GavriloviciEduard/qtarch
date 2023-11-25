#ifndef QTARCH_H
#define QTARCH_H

#include <QOpenGLWidget>
#include "retrowrapper.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class qtarch;
}
QT_END_NAMESPACE


class qtarch : public QOpenGLWidget
{
    Q_OBJECT

public:
    qtarch(const char* lib_path, const char* rom_path, QWidget *parent = nullptr);
    ~qtarch();

protected:
    void initializeGL();
    void paintGL();

private:
    Ui::qtarch *ui;
    const char* m_lib_path = {};
    const char* m_rom_path = {};
    QOpenGLFunctions_1_4* gl_funcs = {};
    QOpenGLWidget *m_openglWidget = {};
    std::unique_ptr<RetroWrapper> m_retro = {};
};
#endif // QTARCH_H
