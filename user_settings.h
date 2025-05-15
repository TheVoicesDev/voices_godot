#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/io/config_file.h"

class UserSettings : public ConfigFile {
    GDCLASS(UserSettings, ConfigFile);

public:
    UserSettings();
    ~UserSettings();

    static UserSettings* get_singleton();

    void set_value(const String &p_section, const String &p_key, const Variant &p_value);
};

#endif