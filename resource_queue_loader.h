#ifndef RESOURCE_QUEUE_LOADER_H
#define RESOURCE_QUEUE_LOADER_H

#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/variant/typed_array.h"

template <typename T>
class TypedArray;

class ResourceQueueLoader : public Object {
    GDCLASS(ResourceQueueLoader, Object);

public:
    ResourceQueueLoader();
    ~ResourceQueueLoader();

    static ResourceQueueLoader *get_singleton();

    String current_path;
    TypedArray<String> path_queue;

    String get_current_path() const;
    TypedArray<String> get_path_queue() const;

    void run_callbacks();

    void queue(const String &p_path);
    void queue_multiple(const TypedArray<String> &p_paths);
    void dequeue(const String &p_path);
    
    bool has(const String &p_path) const;
    int size() const;

protected:
    static ResourceQueueLoader* singleton;

    static void _bind_methods();

private:
    float* _progress;
    TypedArray<float> _progress_array;
};

#endif // RESOURCE_QUEUE_LOADER_H