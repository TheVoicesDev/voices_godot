#include "loading_screen.h"
#include "screen_manager.h"

void LoadingScreen::set_opening_animation(const StringName &p_value) {
    opening_animation = p_value;
}

StringName LoadingScreen::get_opening_animation() const {
    return opening_animation;
}

void LoadingScreen::set_closing_animation(const StringName &p_value) {
    closing_animation = p_value;
}

StringName LoadingScreen::get_closing_animation() const {
    return closing_animation;
}

void LoadingScreen::set_reference_animation_player(AnimationPlayer *p_value) {
    reference_animation_player = p_value;
}

AnimationPlayer* LoadingScreen::get_reference_animation_player() const {
	return reference_animation_player;
}

void LoadingScreen::_notification(const int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_READY: {
            if (Engine::get_singleton()->is_editor_hint())
                return;
            
            reference_animation_player->connect(SNAME("animation_finished"), callable_mp(this, &LoadingScreen::_animation_finished));
            reference_animation_player->play(opening_animation);
            reference_animation_player->seek(0.0, true);

            ScreenManager::get_singleton()->connect(SNAME("preload_completed"), callable_mp(this, &LoadingScreen::_load_completed));
        }   break;
    }
}

void LoadingScreen::_animation_finished(const StringName &p_anim) {
    if (p_anim == opening_animation)
        ScreenManager::get_singleton()->load_screen();
    
    if (p_anim == closing_animation)
        queue_free();
}

void LoadingScreen::_load_completed() {
    ScreenManager::get_singleton()->disconnect(SNAME("preload_completed"), callable_mp(this, &LoadingScreen::_load_completed));

    reference_animation_player->play(closing_animation);
    reference_animation_player->seek(0.0, true);
}

void LoadingScreen::_bind_methods() {
    // Getters and Setters
    ClassDB::bind_method(D_METHOD("set_opening_animation", "value"), &LoadingScreen::set_opening_animation);
    ClassDB::bind_method("get_opening_animation", &LoadingScreen::get_opening_animation);
    ClassDB::bind_method(D_METHOD("set_closing_animation", "value"), &LoadingScreen::set_closing_animation);
    ClassDB::bind_method("get_closing_animation", &LoadingScreen::get_closing_animation);
    ClassDB::bind_method(D_METHOD("set_reference_animation_player", "value"), &LoadingScreen::set_reference_animation_player);
    ClassDB::bind_method("get_reference_animation_player", &LoadingScreen::get_reference_animation_player);

    // Properties
    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "opening_animation"), "set_opening_animation", "get_opening_animation");
    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "closing_animation"), "set_closing_animation", "get_closing_animation");

    ADD_GROUP("References", "reference");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "reference_animation_player", PROPERTY_HINT_NODE_TYPE, "AnimationPlayer"), "set_reference_animation_player", "get_reference_animation_player");
}