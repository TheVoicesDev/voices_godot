#ifndef VOICES_CONSTANTS_H
#define VOICES_CONSTANTS_H

#include "core/string/string_name.h"

class VoicesConstants {
    inline static VoicesConstants *singleton = nullptr;

public:
    static void create() { singleton = memnew(VoicesConstants); }
    static void free() {
        memdelete(singleton);
        singleton = nullptr;
    }

    _FORCE_INLINE_ static VoicesConstants *get_singleton() { return singleton; }
    
    const String screen_debug_empty = "No debug info to display!";
};

#define VoicesConstant(m_name) VoicesConstants::get_singleton()->m_name

#endif // VOICES_CONSTANTS_H