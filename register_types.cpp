#include "register_types.h"

#include "core/config/engine.h"
#include "core/object/class_db.h"
#include "resource_queue_loader.h"
#include "voices_constants.h"
#include "screen_manager.h"
#include "screen.h"
#include "loading_screen.h"

static ResourceQueueLoader* rql_pointer;
static ScreenManager* scrn_manager;

void initialize_voices_godot_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
    
    VoicesConstants::create();

	// Register classes
    GDREGISTER_CLASS(ResourceQueueLoader);
    rql_pointer = memnew(ResourceQueueLoader);
    Engine::get_singleton()->add_singleton(Engine::Singleton("ResourceQueueLoader", ResourceQueueLoader::get_singleton()));

    GDREGISTER_CLASS(ScreenManager);
    scrn_manager = memnew(ScreenManager);
    Engine::get_singleton()->add_singleton(Engine::Singleton("ScreenManager", ScreenManager::get_singleton()));

    GDREGISTER_CLASS(Screen);
    GDREGISTER_CLASS(LoadingScreen);
}

void uninitialize_voices_godot_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
    
    Engine::get_singleton()->remove_singleton("ScreenManager");
    memdelete(scrn_manager);

    Engine::get_singleton()->remove_singleton("ResourceQueueLoader");
    memdelete(rql_pointer);

    VoicesConstants::free();
}