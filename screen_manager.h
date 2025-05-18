#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/variant/array.h"
#include "loading_screen.h"
#include "screen.h"

class ScreenManager : public Object {
    GDCLASS(ScreenManager, Object);

public:
    ScreenManager();
    ~ScreenManager();

    static ScreenManager* get_singleton();

    Screen* current_screen = nullptr;
    LoadingScreen* loading_screen = nullptr;
    int progress = 0;

    void set_current_screen(Screen *p_value);
    Screen* get_current_screen() const;

    void set_loading_screen(LoadingScreen *p_value);
    LoadingScreen* get_loading_screen() const;

    int get_progress() const;

    void switch_screen(const String &p_path, LoadingScreen* p_loading_screen = nullptr);
    void switch_screen_direct(Screen* p_screen, LoadingScreen* p_loading_screen = nullptr);
    void reload_screen(LoadingScreen* p_loading_screen = nullptr);

    void load_screen();
    void queue_extra_path(const String &p_path);

protected:
    static ScreenManager* singleton;

    static void _bind_methods();

private:
    String _screen_path;
    PackedStringArray _preload_list;
    int _preload_count = 0;

    void _reset();
    LoadingScreen* _instantiate_default_loading_screen();

    void _progress_updated(const String &p_path, const Array &p_progress);
    void _on_resource_loaded(const String &p_path);
    void _completed();
};

#endif