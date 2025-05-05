#include "register_types.h"

#include "core/config/engine.h"
#include "core/object/class_db.h"
#include "resource_queue_loader.h"

static ResourceQueueLoader* rql_pointer;

void initialize_voices_godot_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;

	// Register classes
    GDREGISTER_CLASS(ResourceQueueLoader);

    rql_pointer = memnew(ResourceQueueLoader);
    Engine::get_singleton()->add_singleton(Engine::Singleton("ResourceQueueLoader", ResourceQueueLoader::get_singleton()));
}

void uninitialize_voices_godot_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
    
    Engine::get_singleton()->remove_singleton("ResourceQueueLoader");
    memdelete(rql_pointer);
}