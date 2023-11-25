#ifndef VIDEO_H
#define VIDEO_H

#include <QOpenGLFunctions_1_4>
#include "libretro.h"

static GLfloat g_vertex[] = {
    -1.0f, -1.0f, // left-bottom
    -1.0f,  1.0f, // left-top
    1.0f, -1.0f, // right-bottom
    1.0f,  1.0f, // right-top
};

static GLfloat g_texcoords[] ={
    0.0f,  1.0f,
    0.0f,  0.0f,
    1.0f,  1.0f,
    1.0f,  0.0f,
};

static struct {
    GLuint tex_id;
    GLuint pitch;
    GLint tex_w, tex_h;
    GLuint clip_w, clip_h;

    GLuint pixfmt;
    GLuint pixtype;
    GLuint bpp;
} g_video  = {0};

class Video
{
public:
    Video(QOpenGLFunctions_1_4* gl_functions) {m_gl_functions = gl_functions; }
    ~Video() = default;

    void refresh_vertex_data();
    bool video_set_pixel_format(unsigned format);
    void video_configure(const struct retro_game_geometry *geom);
    void video_render();
    void video_refresh(const void *data, unsigned width, unsigned height, unsigned pitch);

private:
    QOpenGLFunctions_1_4* m_gl_functions = {};
};

#endif // VIDEO_H
