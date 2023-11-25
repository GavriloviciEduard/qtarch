#include "video.h"

bool Video::video_set_pixel_format(unsigned format)
{
    if (g_video.tex_id)
    {
        qDebug() << "Tried to change pixel format after initialization.";
        exit(0);
    }

    switch (format) {
    case RETRO_PIXEL_FORMAT_0RGB1555:
        g_video.pixfmt = GL_UNSIGNED_SHORT_5_5_5_1;
        g_video.pixtype = GL_BGRA;
        g_video.bpp = sizeof(uint16_t);
        break;
    case RETRO_PIXEL_FORMAT_XRGB8888:
        g_video.pixfmt = GL_UNSIGNED_INT_8_8_8_8_REV;
        g_video.pixtype = GL_BGRA;
        g_video.bpp = sizeof(uint32_t);
        break;
    case RETRO_PIXEL_FORMAT_RGB565:
        g_video.pixfmt  = GL_UNSIGNED_SHORT_5_6_5;
        g_video.pixtype = GL_RGB;
        g_video.bpp = sizeof(uint16_t);
        break;
    default:
        qDebug() <<"Unknown pixel type";
        exit(0);
    }

    return true;
}

void Video::refresh_vertex_data()
{
    GLfloat *coords = g_texcoords;
    coords[1] = coords[5] = (float)g_video.clip_h / g_video.tex_h;
    coords[4] = coords[6] = (float)g_video.clip_w / g_video.tex_w;
}

void Video::video_configure(const struct retro_game_geometry *geom)
{
    m_gl_functions->glEnable(GL_TEXTURE_2D);
    if (g_video.tex_id)
    {
        m_gl_functions->glDeleteTextures(1, &g_video.tex_id);
    }
    g_video.tex_id = 0;
    if (!g_video.pixfmt)
    {
        g_video.pixfmt = GL_UNSIGNED_SHORT_5_5_5_1;
    }
    m_gl_functions->glGenTextures(1, &g_video.tex_id);
    if (!g_video.tex_id)
    {
        qDebug() << "Failed to create the video texture";
        exit(1);
    }

    g_video.pitch = geom->base_width * g_video.bpp;
    m_gl_functions->glBindTexture(GL_TEXTURE_2D, g_video.tex_id);
    m_gl_functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_gl_functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_gl_functions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, geom->max_width,
                               geom->max_height, 0, g_video.pixtype, g_video.pixfmt, NULL);
    m_gl_functions->glBindTexture(GL_TEXTURE_2D, 0);

    g_video.tex_w = geom->max_width;
    g_video.tex_h = geom->max_height;
    g_video.clip_w = geom->base_width;
    g_video.clip_h = geom->base_height;

    refresh_vertex_data();
}

void Video::video_refresh(const void *data, unsigned width, unsigned height, unsigned pitch)
{
    if (g_video.clip_w != width || g_video.clip_h != height)
    {
        g_video.clip_h = height;
        g_video.clip_w = width;

        refresh_vertex_data();
    }

    m_gl_functions->glBindTexture(GL_TEXTURE_2D, g_video.tex_id);

    if (pitch != g_video.pitch)
    {
        g_video.pitch = pitch;
        m_gl_functions->glPixelStorei(GL_UNPACK_ROW_LENGTH, g_video.pitch / g_video.bpp);
    }

    if (data)
    {
        m_gl_functions->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                        g_video.pixtype, g_video.pixfmt, data);
    }
}

void Video::video_render()
{
    m_gl_functions->glBindTexture(GL_TEXTURE_2D, g_video.tex_id);
    m_gl_functions->glEnableClientState(GL_VERTEX_ARRAY);
    m_gl_functions->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    m_gl_functions->glVertexPointer(2, GL_FLOAT, 0, g_vertex);
    m_gl_functions->glTexCoordPointer(2, GL_FLOAT, 0, g_texcoords);
    m_gl_functions->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
