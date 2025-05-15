#include "user_settings.h"

void UserSettings::set_value(const String &p_section, const String &p_key, const Variant &p_value) {
    ConfigFile::set_value(p_section, p_key, p_value);
}