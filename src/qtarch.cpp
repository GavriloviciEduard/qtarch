#include "qtarch.h"
#include "./ui_qtarch.h"
#include <QDebug>
#include <QTimer>
#include <QOpenGLFunctions_1_4>
#include <QOpenGLVersionFunctionsFactory>
#include <QOpenGLPaintDevice>
#include <QPainter>

qtarch::qtarch(const char* lib_path, const char* rom_path, QWidget *parent):
    QOpenGLWidget(parent), ui(new Ui::qtarch)
{
    m_lib_path = lib_path;
    m_rom_path = rom_path;
    ui->setupUi(this);
}

qtarch::~qtarch()
{
    delete ui;
}

void qtarch::initializeGL()
{
    gl_funcs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_1_4>(context());
    m_retro = std::make_unique<RetroWrapper>(gl_funcs);
    if(!m_retro->load_library(m_lib_path))
    {
        qDebug() << "Failed to load core.";
        exit(1);
    }
    m_retro->retro_init();
    m_retro->core_load_game(m_rom_path);
    m_retro->m_video->video_configure(&m_retro->get_retro_system_av_info()->geometry);
}

void qtarch::paintGL()
{
    //TODO: Move rendering in different thread and account for fps.
    m_retro->retro_run();
    gl_funcs->glClear(GL_COLOR_BUFFER_BIT);
    m_retro->m_video->video_render();
    update();
}
