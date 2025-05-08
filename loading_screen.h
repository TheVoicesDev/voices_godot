#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include "core/object/class_db.h"
#include "core/string/string_name.h"
#include "scene/main/canvas_layer.h"
#include "scene/animation/animation_player.h"

class LoadingScreen : public CanvasLayer {
    GDCLASS(LoadingScreen, CanvasLayer);

public:
    StringName opening_animation;
    StringName closing_animation;

    AnimationPlayer* reference_animation_player;

    void set_opening_animation(const StringName &p_value);
    StringName get_opening_animation() const;

    void set_closing_animation(const StringName &p_value);
    StringName get_closing_animation() const;

    void set_reference_animation_player(AnimationPlayer *p_value);
    AnimationPlayer* get_reference_animation_player() const;

protected:
    void _notification(int p_notification);

    static void _bind_methods();

private:
    void _animation_finished(const StringName &p_anim);
    void _load_completed();
};

#endif // LOADING_SCREEN_H