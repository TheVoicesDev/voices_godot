#include "screen_manager.h"
#include "core/config/project_settings.h"
#include "core/io/resource.h"
#include "core/io/resource_loader.h"
#include "scene/main/node.h"
#include "scene/main/window.h"
#include "scene/resources/packed_scene.h"
#include "resource_queue_loader.h"

ScreenManager *ScreenManager::singleton = nullptr;

ScreenManager *ScreenManager::get_singleton() {
	return singleton;
}

void ScreenManager::set_current_screen(Screen *p_value) {
    current_screen = p_value;
}

Screen* ScreenManager::get_current_screen() const {
    return current_screen;
}

void ScreenManager::set_loading_screen(LoadingScreen *p_value) {
    loading_screen = p_value;
}

LoadingScreen* ScreenManager::get_loading_screen() const {
    return loading_screen;
}

int ScreenManager::get_progress() const {
    return progress;
}

void ScreenManager::switch_screen(const String &p_path, LoadingScreen* p_loading_screen) {
    ERR_FAIL_COND_MSG(!ResourceLoader::exists(p_path), "Resource not found at path " + p_path);
    
    _reset();
    _screen_path = p_path;

    if (p_loading_screen == nullptr) {
        LoadingScreen* fallback = _instantiate_default_loading_screen();
        if (fallback != nullptr) {
            switch_screen(p_path, fallback);
            return;
        }
        
        load_screen();
        return;
    }

    Window* root = SceneTree::get_singleton()->get_root();
    root->add_child(p_loading_screen);
}

void ScreenManager::switch_screen_direct(Screen* p_screen, LoadingScreen* p_loading_screen) {
    ERR_FAIL_COND_MSG(p_screen == nullptr, "Attempted to load a null screen directly.");
    
    _reset();
    current_screen = p_screen;

    if (p_loading_screen == nullptr) {
        LoadingScreen* fallback = _instantiate_default_loading_screen();
        if (fallback != nullptr) {
            switch_screen_direct(p_screen, fallback);
            return;
        }
        
        load_screen();
        return;
    }

    Window* root = SceneTree::get_singleton()->get_root();
    root->add_child(p_loading_screen);
}

void ScreenManager::reload_screen(LoadingScreen* p_loading_screen) {
    ERR_FAIL_COND_MSG(current_screen == nullptr, "No current screen set to reload.");

    Screen* new_screen = static_cast<Screen*>(current_screen->duplicate());
    new_screen->set_name(current_screen->get_name());
    switch_screen_direct(new_screen, p_loading_screen);
}

void ScreenManager::load_screen() {
    emit_signal(SNAME("started"));
    
    Node* current_scene = SceneTree::get_singleton()->get_current_scene();
    if (current_scene != nullptr)
        current_scene->queue_free();

    if (_screen_path.is_empty() && current_screen != nullptr) {
        _screen_path = current_screen->get_scene_file_path();
        _on_resource_loaded(_screen_path);
        return;
    }
    
    if (current_screen != nullptr)
        current_screen->queue_free();

    ResourceQueueLoader::get_singleton()->queue(_screen_path);
}

void ScreenManager::queue_extra_path(const String &p_path) {
    _preload_list.push_back(p_path);
    ResourceQueueLoader::get_singleton()->queue(p_path);
}

void ScreenManager::_reset() {
    progress = 0;
    _preload_count = 0;
    _preload_list.clear();
    _screen_path.clear();
}

LoadingScreen* ScreenManager::_instantiate_default_loading_screen() {
    String default_path = static_cast<String>(GLOBAL_GET("voices/project/default_loading_screen"));
    if (default_path.is_empty())
        return nullptr;
    
    ERR_FAIL_COND_V_MSG(!ResourceLoader::exists(default_path), nullptr, "Loading screen at voices/project/default_loading_screen does not exist.");
    Ref<Resource> loading_res = ResourceLoader::load(default_path);
    
    ERR_FAIL_COND_V_MSG(!loading_res->is_class("PackedScene"), nullptr, "Loading screen at voices/project/default_loading_screen must be a PackedScene!");
    Ref<PackedScene> loading_pck = static_cast<Ref<PackedScene>>(loading_res);

    ERR_FAIL_COND_V_MSG(!loading_pck->can_instantiate(), nullptr, "Could not instantiate scene at voices/project/default_loading_screen");
    Node* loading_node = loading_pck->instantiate();

    ERR_FAIL_COND_V_MSG(!loading_node->is_class("LoadingScreen"), nullptr, "Loading screen at path voices/project/default_loading_screen is not of type LoadingScreen!");
    return static_cast<LoadingScreen*>(loading_node);
}

void ScreenManager::_progress_updated(const String &p_path, const Array &p_progress) {
    if (p_path == _screen_path) {
        progress = (int)floorf((float)p_progress[0] * 50);
        return;
    }

    float segment = 1.0f / _preload_list.size();
    progress = 50 + (int)floorf(((float)_preload_count / _preload_list.size()) + (segment * (float)p_progress[0] * 50));
    emit_signal(SNAME("progress_updated"), progress);
}

void ScreenManager::_on_resource_loaded(const String &p_path) {
    if (p_path == _screen_path) {
        progress = 50;

        if (current_screen == nullptr) {
            Ref<Resource> res = ResourceLoader::load(p_path);
            ERR_FAIL_COND_MSG(!res->is_class(SNAME("PackedScene")), "The requested resource must be a packed scene!");

            Ref<PackedScene> pck = static_cast<Ref<PackedScene>>(res);
            ERR_FAIL_COND_MSG(!pck->can_instantiate(), vformat("Packed scene (%s) resource could not be instantiated!", p_path));
            Node* node = pck->instantiate();

            ERR_FAIL_COND_MSG(node->is_class("Screen"), "Instantiated node is not of type Screen.");
            current_screen = static_cast<Screen*>(node);
        }

        current_screen->loaded();
        
        if (_preload_list.size() == 0)
            _completed();
        
        return;
    }

    if (!_preload_list.has(p_path))
        return;

    _preload_count++;
    progress = 50 + (int)floorf(((float)_preload_count / _preload_list.size()) * 50);
    emit_signal(SNAME("progress_updated"), progress);

    if (_preload_count < _preload_list.size())
        return;
    
    _completed();
}

void ScreenManager::_completed() {
    progress = 100;
    
    if (SceneTree::get_singleton()->get_current_scene() == current_screen)
        SceneTree::get_singleton()->get_root()->remove_child(current_screen);
    
    SceneTree::get_singleton()->add_current_scene(current_screen);
    current_screen->propagate_notification(Node::NOTIFICATION_READY);

    emit_signal(SNAME("progress_updated"), progress);
    emit_signal(SNAME("completed"));

    _reset();
}

void ScreenManager::_bind_methods() {
    // Getters and Setters
    ClassDB::bind_method(D_METHOD("set_current_screen", "value"), &ScreenManager::set_current_screen);
    ClassDB::bind_method("get_current_screen", &ScreenManager::get_current_screen);
    ClassDB::bind_method(D_METHOD("set_loading_screen", "value"), &ScreenManager::set_loading_screen);
    ClassDB::bind_method("get_loading_screen", &ScreenManager::get_loading_screen);
    
    // Properties
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_screen", PROPERTY_HINT_NODE_TYPE, "Screen"), "set_current_screen", "get_current_screen");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "loading_screen", PROPERTY_HINT_NODE_TYPE, "LoadingScreen"), "set_loading_screen", "get_loading_screen");

    // Methods
    ClassDB::bind_method("get_progress", &ScreenManager::get_progress);
    
    ClassDB::bind_method(D_METHOD("switch_screen", "path", "loading_screen"), &ScreenManager::switch_screen);
    ClassDB::bind_method(D_METHOD("switch_screen_direct", "screen", "loading_screen"), &ScreenManager::switch_screen_direct);
    ClassDB::bind_method(D_METHOD("reload_screen", "loading_screen"), &ScreenManager::reload_screen);

    ClassDB::bind_method("load_screen", &ScreenManager::load_screen);
    ClassDB::bind_method(D_METHOD("queue_extra_path", "path"), &ScreenManager::queue_extra_path);

    ADD_SIGNAL(MethodInfo("started"));
    ADD_SIGNAL(MethodInfo("progress_updated", PropertyInfo(Variant::INT, "progress")));
    ADD_SIGNAL(MethodInfo("completed"));

    GLOBAL_DEF_BASIC(PropertyInfo(Variant::STRING, "voices/project/default_loading_screen", PROPERTY_HINT_FILE, "*.tscn,*.scn"), "");
}

ScreenManager::ScreenManager() {
	singleton = this;

    ResourceQueueLoader* queue_loader = ResourceQueueLoader::get_singleton();
    queue_loader->connect(SNAME("preload_progressed"), callable_mp(this, &ScreenManager::_progress_updated));
    queue_loader->connect(SNAME("preload_completed"), callable_mp(this, &ScreenManager::_on_resource_loaded));
}

ScreenManager::~ScreenManager() {
	if (singleton != this)
        return;
		
	singleton = nullptr;

    ResourceQueueLoader* queue_loader = ResourceQueueLoader::get_singleton();
    queue_loader->disconnect(SNAME("preload_progressed"), callable_mp(this, &ScreenManager::_progress_updated));
    queue_loader->disconnect(SNAME("preload_completed"), callable_mp(this, &ScreenManager::_on_resource_loaded));
}