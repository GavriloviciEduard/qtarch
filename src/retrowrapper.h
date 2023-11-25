#ifndef RETROWRAPPER_H
#define RETROWRAPPER_H

#include <QLibrary>
#include "video.h"

static unsigned g_joy[RETRO_DEVICE_ID_JOYPAD_R3+1] = { 0 };

class RetroWrapper
{
public:
    RetroWrapper(QOpenGLFunctions_1_4* gl_functions) { m_video = std::make_unique<Video>(gl_functions); }
    ~RetroWrapper() = default;

    bool load_library(const char* lib_path);
    void retro_init();
    void retro_deinit();
    void retro_get_system_info(struct retro_system_info *info);
    void retro_get_system_av_info(struct retro_system_av_info *info);
    void retro_reset();
    void retro_run();
    void core_load_game(const char* rom_path);
    void retro_unload_game();

    const struct retro_system_av_info* get_retro_system_av_info() { return &m_av_info; }
    inline static std::unique_ptr<Video> m_video;

private:
    std::unique_ptr<QLibrary> m_library_loader = {};
    struct retro_system_av_info m_av_info = {0};

    void set_callbacks();
    static void core_log(enum retro_log_level level, const char *fmt, ...);
    bool retro_load_game(const struct retro_game_info *game);
    static bool core_environment(unsigned cmd, void *data);
    static void core_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch);
    static void core_input_poll(void);
    static void core_audio_sample(int16_t left, int16_t right);
    static size_t core_audio_sample_batch(const int16_t *data, size_t frames);
    static int16_t core_input_state(unsigned port, unsigned device, unsigned index, unsigned id);
};

#endif // RETROWRAPPER_H
