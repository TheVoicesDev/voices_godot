#include "resource_queue_loader.h"
#include "core/io/resource.h"
#include "core/io/resource_loader.h"

ResourceQueueLoader *ResourceQueueLoader::singleton = nullptr;

ResourceQueueLoader *ResourceQueueLoader::get_singleton() {
	return singleton;
}

String ResourceQueueLoader::get_current_path() const {
    return current_path;
}

TypedArray<String> ResourceQueueLoader::get_path_queue() const {
    return path_queue.duplicate();
}

void ResourceQueueLoader::run_callbacks() {
    bool not_loading_anything = current_path.is_empty();
    if (path_queue.is_empty() && not_loading_anything)
        return;
    
    if (not_loading_anything) {
        current_path = path_queue.pop_front();
        while (!current_path.is_empty() && ResourceCache::has(current_path)) {
            _progress_array[0] = 1.0;
            emit_signal(SNAME("preload_started"), current_path);
            emit_signal(SNAME("preload_progressed"), current_path, _progress_array);
            emit_signal(SNAME("preload_completed"), current_path);

            current_path = path_queue.pop_front();
        }
        
        ResourceLoader::load_threaded_request(current_path);
        emit_signal(SNAME("preload_started"), current_path);
    }
    
    ResourceLoader::ThreadLoadStatus status = ResourceLoader::load_threaded_get_status(current_path, _progress);
    switch (status) {
        case ResourceLoader::ThreadLoadStatus::THREAD_LOAD_FAILED:
        case ResourceLoader::ThreadLoadStatus::THREAD_LOAD_INVALID_RESOURCE: {
            print_error("[ResourceQueueLoader] Failed to load resource " + current_path);
            emit_signal(SNAME("preload_failed"), current_path);
        }   break;

        case ResourceLoader::ThreadLoadStatus::THREAD_LOAD_IN_PROGRESS: {
            _progress_array[0] = _progress;
            emit_signal(SNAME("preload_progressed"), current_path, _progress_array);
        }   break;
        
        case ResourceLoader::ThreadLoadStatus::THREAD_LOAD_LOADED: {
            emit_signal(SNAME("preload_completed"), current_path);
            current_path.clear();
        }   break;
    }
}

void ResourceQueueLoader::queue(const String &p_path) {
    path_queue.push_back(p_path);
}

void ResourceQueueLoader::queue_multiple(const TypedArray<String> &p_paths) {
    path_queue.append_array(p_paths);
}

void ResourceQueueLoader::dequeue(const String &p_path) {
    int index = path_queue.find(p_path);
    ERR_FAIL_COND_MSG(index < 0, vformat("Tried to dequeue path not inside the queue. (%s)", p_path));

    path_queue.remove_at(index);
}

bool ResourceQueueLoader::has(const String &p_path) const {
    return path_queue.has(p_path);
}

int ResourceQueueLoader::size() const {
    return path_queue.size();
}

void ResourceQueueLoader::_bind_methods() {
    ClassDB::bind_method("get_current_path", &ResourceQueueLoader::get_current_path);
    ClassDB::bind_method("get_path_queue", &ResourceQueueLoader::get_path_queue);

    ClassDB::bind_method("run_callbacks", &ResourceQueueLoader::run_callbacks);

    ClassDB::bind_method(D_METHOD("queue", "path"), &ResourceQueueLoader::queue);
    ClassDB::bind_method(D_METHOD("queue_multiple", "paths"), &ResourceQueueLoader::queue_multiple);
    ClassDB::bind_method(D_METHOD("dequeue", "path"), &ResourceQueueLoader::dequeue);
    
    ClassDB::bind_method(D_METHOD("has", "path"), &ResourceQueueLoader::has);
    ClassDB::bind_method("size", &ResourceQueueLoader::size);

    ADD_SIGNAL(MethodInfo("preload_started", PropertyInfo(Variant::STRING, "current_path")));
    ADD_SIGNAL(MethodInfo("preload_progressed", PropertyInfo(Variant::STRING, "current_path"), PropertyInfo(Variant::ARRAY, "progress")));
    ADD_SIGNAL(MethodInfo("preload_completed", PropertyInfo(Variant::STRING, "current_path")));
    ADD_SIGNAL(MethodInfo("preload_failed", PropertyInfo(Variant::STRING, "current_path")));
}

ResourceQueueLoader::ResourceQueueLoader() {
	singleton = this;
}

ResourceQueueLoader::~ResourceQueueLoader() {
	if (singleton == this) {
		singleton = nullptr;
	}
}