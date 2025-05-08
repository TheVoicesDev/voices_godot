#ifndef SCREEN_H
#define SCREEN_H

#include "core/io/resource.h"
#include "core/object/class_db.h"
#include "core/variant/typed_array.h"
#include "scene/main/node.h"

template <typename T>
class TypedArray;

class Screen : public Node {
    GDCLASS(Screen, Node);

public:
    TypedArray<Resource> resources;
    bool needs_preloading = false;

    void set_resources(const TypedArray<Resource> &p_resources);
    TypedArray<Resource> get_resources() const;

    void set_needs_preloading(const bool p_value);
    bool get_needs_preloading() const;

    void loaded();
    String get_debug_info() const;
    bool is_loaded() const;

protected:
    void _notification(const int p_notification);
    static void _bind_methods();

    GDVIRTUAL0(_loaded);
    GDVIRTUAL0RC(String, _get_debug_info);

private:
    bool _preloaded = false;
};

#endif