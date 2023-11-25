#include "retrowrapper.h"
#include "video.h"
#include <QDebug>

//TODO: Refactor this class!!!!

bool RetroWrapper::load_library(const char* lib_path)
{
    m_library_loader = std::make_unique<QLibrary>(lib_path);
    return m_library_loader->load();
}

void RetroWrapper::core_load_game(const char* rom_path)
{
    struct retro_system_info system = {0};
    struct retro_game_info info = {rom_path, 0 };
    FILE *file = fopen(rom_path, "rb");
    if(!file)
    {
        qDebug() << "Failed to load content";
        return;
    }
    fseek(file, 0, SEEK_END);
    info.size = ftell(file);
    rewind(file);
    retro_get_system_info(&system);
    if (!system.need_fullpath)
    {
        info.data = malloc(info.size);
        if (!info.data || !fread((void*)info.data, info.size, 1, file))
        {
            qDebug() << "Failed to load content";
            return;
        }
        if(!retro_load_game(&info))
        {
            qDebug() << "Failed to load content";
            return;
        }
        retro_get_system_av_info(&m_av_info);
    }
}

void RetroWrapper::core_log(enum retro_log_level level, const char *fmt, ...)
{
    char buffer[4096] = {0};
    static const char * levelstr[] = { "dbg", "inf", "wrn", "err" };
    va_list va;

    va_start(va, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, va);
    va_end(va);

    if (level == 0)
        return;

    fprintf(stderr, "[%s] %s", levelstr[level], buffer);
    fflush(stderr);

    if (level == RETRO_LOG_ERROR)
        exit(EXIT_FAILURE);
}

bool RetroWrapper::core_environment(unsigned cmd, void *data)
{
    bool *bval;

    switch (cmd) {
    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE: {
        struct retro_log_callback *cb = (struct retro_log_callback *)data;
        cb->log = core_log;
        break;
    }
    case RETRO_ENVIRONMENT_GET_CAN_DUPE:
        bval = (bool*)data;
        *bval = true;
        break;
    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: {
        const enum retro_pixel_format *fmt = (enum retro_pixel_format *)data;

        if (*fmt > RETRO_PIXEL_FORMAT_RGB565)
            return false;

        return m_video->video_set_pixel_format(*fmt);
    }
    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
    case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
        *(const char **)data = ".";
        return true;

    default:
        qDebug() << "Unhandled env " << cmd;
        return false;
    }

    return true;
}

void RetroWrapper::core_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
{
    qDebug() << "Called core_video_refresh.";
    if (data)
    {
        m_video->video_refresh(data, width, height, pitch);
    }
}

void RetroWrapper::core_input_poll(void)
{
    qDebug() << "Called core_input_poll.";
}

int16_t RetroWrapper::core_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
    qDebug() << "Called core_input_state.";
    return 0;
}

void RetroWrapper::core_audio_sample(int16_t left, int16_t right)
{
    qDebug() << "Called core_audio_sample.";
}

size_t RetroWrapper::core_audio_sample_batch(const int16_t *data, size_t frames)
{
    qDebug() << "Called core_audio_sample.";
    return 0;
}

void RetroWrapper::set_callbacks()
{
    /*
     * Libretro is callback based. The frontend will set all callbacks at this stage,
     * and the implementation must store these function pointers somewhere.
     * The frontend can, at a later stage, call these.
     */

    typedef void (*set_environment)(retro_environment_t);
    set_environment set_environment_fun = (set_environment) m_library_loader->resolve("retro_set_environment");
    if(set_environment_fun)
    {
        set_environment_fun(core_environment);
    }

    typedef void (*set_video_refresh)(retro_video_refresh_t);
    set_video_refresh set_video_refresh_fun = (set_video_refresh) m_library_loader->resolve("retro_set_video_refresh");
    if(set_video_refresh_fun)
    {
        set_video_refresh_fun(core_video_refresh);
    }

    typedef void (*set_input_poll)(retro_input_poll_t);
    set_input_poll set_input_poll_fun = (set_input_poll) m_library_loader->resolve("retro_set_input_poll");
    if(set_input_poll_fun)
    {
        set_input_poll_fun(core_input_poll);
    }

    typedef void (*set_input_state)(retro_input_state_t);
    set_input_state set_input_state_fun = (set_input_state) m_library_loader->resolve("retro_set_input_state");
    if(set_input_state_fun)
    {
        set_input_state_fun(core_input_state);
    }

    typedef void (*set_audio_sample)(retro_audio_sample_t);
    set_audio_sample set_audio_sample_fun = (set_audio_sample) m_library_loader->resolve("retro_set_audio_sample");
    if(set_audio_sample_fun)
    {
        set_audio_sample_fun(core_audio_sample);
    }

    typedef void (*set_audio_sample_batch)(retro_audio_sample_batch_t);
    set_audio_sample_batch set_audio_sample_batch_fun = (set_audio_sample_batch) m_library_loader->resolve("retro_set_audio_sample_batch");
    if(set_audio_sample_batch_fun)
    {
        set_audio_sample_batch_fun(core_audio_sample_batch);
    }
}

void RetroWrapper::retro_init()
{
    set_callbacks();
    typedef void (*retro_init)(void);
    retro_init fun = (retro_init) m_library_loader->resolve("retro_init");
    if (fun)
    {
        fun();
        qDebug() << "Core loaded.";
    }
}

void RetroWrapper::retro_deinit()
{
    typedef void (*retro_deinit)(void);
    retro_deinit fun = (retro_deinit) m_library_loader->resolve("retro_deinit");
    if (fun)
    {
        fun();
    }
}

void RetroWrapper::retro_get_system_info(struct retro_system_info* info)
{
    typedef void (*retro_get_system_info)(struct retro_system_info*);
    retro_get_system_info fun = (retro_get_system_info) m_library_loader->resolve("retro_get_system_info");
    if (fun)
    {
        fun(info);
    }
}

void RetroWrapper::retro_get_system_av_info(struct retro_system_av_info *info)
{
    typedef void (*retro_get_system_av_info)(struct retro_system_av_info*);
    retro_get_system_av_info fun = (retro_get_system_av_info) m_library_loader->resolve("retro_get_system_av_info");
    if (fun)
    {
        fun(info);
    }
}

void RetroWrapper::retro_reset()
{
    typedef void (*retro_reset)(void);
    retro_reset fun = (retro_reset) m_library_loader->resolve("retro_reset");
    if (fun)
    {
        fun();
    }
}

void RetroWrapper::retro_run()
{
    typedef void (*retro_run)(void);
    retro_run fun = (retro_run) m_library_loader->resolve("retro_run");
    if (fun)
    {
        qDebug() << "Called retro_run()";
        fun();
    }
}

bool RetroWrapper::retro_load_game(const struct retro_game_info *game)
{
    typedef bool (*retro_load_game)(const struct retro_game_info *game);
    retro_load_game fun = (retro_load_game) m_library_loader->resolve("retro_load_game");
    if (fun)
    {
        return fun(game);
    }
    return false;
}

void RetroWrapper::retro_unload_game()
{
    typedef void (*retro_unload_game)();
    retro_unload_game fun = (retro_unload_game) m_library_loader->resolve("retro_unload_game");
    if (fun)
    {
        fun();
    }
}
