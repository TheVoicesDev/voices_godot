#include "screen_manager.h"
#include "screen.h"
#include "voices_constants.h"

void Screen::set_resources(const TypedArray<Resource> &p_resources) {
    resources = p_resources;
}

TypedArray<Resource> Screen::get_resources() const {
    return resources;
}

void Screen::set_needs_preloading(const bool p_value) {
    needs_preloading = p_value;
}

bool Screen::get_needs_preloading() const {
    return needs_preloading;
}

void Screen::loaded() {
    _preloaded = true;
    GDVIRTUAL_CALL(_loaded);
}

String Screen::get_debug_info() const {
    String ret = VoicesConstant(screen_debug_empty);
    GDVIRTUAL_CALL(_get_debug_info, ret);
    return ret;
}

bool Screen::is_loaded() const {
    return !needs_preloading || (needs_preloading && _preloaded);
}

void Screen::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_READY: {
            if (Engine::get_singleton()->is_editor_hint() || is_loaded())
                return;
            
            Screen* new_screen = static_cast<Screen*>(duplicate());
            new_screen->set_name(get_name());
            ScreenManager::get_singleton()->call_deferred(SNAME("switch_screen_direct"), new_screen, static_cast<LoadingScreen*>(nullptr));
        }   break;
    }
}

void Screen::_bind_methods() {
    // Getters and Setters
    ClassDB::bind_method(D_METHOD("set_resources", "resources"), &Screen::set_resources);
    ClassDB::bind_method("get_resources", &Screen::get_resources);
    ClassDB::bind_method(D_METHOD("set_needs_preloading", "value"), &Screen::set_needs_preloading);
    ClassDB::bind_method("get_needs_preloading", &Screen::get_needs_preloading);

    // Properties
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "resources", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("Resource")), "set_resources", "get_resources");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "needs_preloading"), "set_needs_preloading", "get_needs_preloading");
    
    ClassDB::bind_method("is_loaded", &Screen::is_loaded);
}