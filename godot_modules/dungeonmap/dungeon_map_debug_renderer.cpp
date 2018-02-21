#include "dungeon_map_debug_renderer.h"
#include "dungeon_map_mesh_builder.h"

void DungeonMapDebugRenderer::region_display(const Vector2& region_id)
{

}

void DungeonMapDebugRenderer::tile_display(const Vector2& tile_id)
{

}

void DungeonMapDebugRenderer::tile_display_neighbors(const Vector2& tile_id)
{

}

void DungeonMapDebugRenderer::set_region_display_all(bool display)
{
    if (!is_inside_tree()) return;

    region_display_all = display;
    for (Map<Vector2, RID>::Element* e = region_instances.front(); e; e = e->next()) {
        if (e->get().is_valid()) {
            VS::get_singleton()->instance_set_visible(e->get(), display);
        }
    }
}

void DungeonMapDebugRenderer::set_tile_display_all(bool display)
{
    if (!is_inside_tree()) return;

    tile_display_all = display;
    for (Map<Vector2, RID>::Element* e = tile_instances.front(); e; e = e->next()) {
        if (e->get().is_valid()) {
            VS::get_singleton()->instance_set_visible(e->get(), display);
        }
    }
}

void DungeonMapDebugRenderer::update()
{
    if (!is_inside_tree())
        return;

    // Check our parent node.
    if (get_parent() && get_parent()->is_class("DungeonMap")) {
        parent = (DungeonMap*)get_parent();
    } else {
        return;
    }

    // Clear previous instances/meshes.
    clear();

    // Create the debug material for the region.
    debug_region_material.instance();
    debug_region_material->set_albedo(Color(0.0f, 1.0f, 0.0f, 0.2f));
    debug_region_material->set_on_top_of_alpha();
    debug_region_material->set_flag(SpatialMaterial::FLAG_UNSHADED, true);
    debug_region_material->set_line_width(3.0);
    debug_region_material->set_feature(SpatialMaterial::FEATURE_TRANSPARENT, true);

    // Create the debug material for the tiles.
    debug_tile_material.instance();
    debug_tile_material->set_albedo(Color(1.0f, 1.0f, 0.0f, 0.75f));
    debug_tile_material->set_on_top_of_alpha();
    debug_tile_material->set_flag(SpatialMaterial::FLAG_UNSHADED, true);
    debug_tile_material->set_line_width(3.0);
    debug_tile_material->set_feature(SpatialMaterial::FEATURE_TRANSPARENT, true);

    // Build instances/meshes.
    for (Map<Vector2, DungeonMap::Region*>::Element* e = parent->get_regions().front(); e; e = e->next()) {
        auto region = e->get();

        // Create the debug render aabb instance.
        RID mesh_instance = DungeonMapMeshBuilder::create_mesh_from_aabb(region->aabb);
        VS::get_singleton()->mesh_surface_set_material(
            mesh_instance,
            0,
            debug_region_material->get_rid()
        );
        RID instance = VS::get_singleton()->instance_create2(mesh_instance, get_world()->get_scenario());

        VS::get_singleton()->instance_set_transform(instance, region->transform);
        VS::get_singleton()->instance_set_visible(instance, false);

        region_instances[region->id] = instance;
        region_meshes[region->id] = mesh_instance;
    }

    // Build instances/meshes.
    for (Map<Vector2, DungeonMap::Tile*>::Element* e = parent->get_tiles().front(); e; e = e->next()) {
        auto tile = e->get();

        // Create the debug render aabb instance.
        RID mesh_instance = DungeonMapMeshBuilder::create_mesh_lines_from_aabb(tile->aabb);
        VS::get_singleton()->mesh_surface_set_material(
            mesh_instance,
            0,
            debug_tile_material->get_rid()
        );
        RID instance = VS::get_singleton()->instance_create2(mesh_instance, get_world()->get_scenario());

        VS::get_singleton()->instance_set_transform(instance, tile->transform);
        VS::get_singleton()->instance_set_visible(instance, false);

        tile_instances[tile->id] = instance;
        tile_meshes[tile->id] = mesh_instance;
    }
}

void DungeonMapDebugRenderer::clear()
{
    // Clear materials.
    if (debug_region_material.is_valid()) {
        debug_region_material.unref();
    }
    if (debug_tile_material.is_valid()) {
        debug_tile_material.unref();
    }

    // Delete cells.
    for (Map<Vector2, RID>::Element* e = tile_instances.front(); e; e = e->next()) {
        RID instance = e->get();
        RID mesh = tile_meshes[e->key()];

        if (mesh.is_valid()) {
            VS::get_singleton()->free(mesh);
        }

        if (instance.is_valid()) {
            VS::get_singleton()->free(instance);
        }
    }
    tile_meshes.clear();
    tile_instances.clear();

    // Delete regions.
    for (Map<Vector2, RID>::Element* e = region_instances.front(); e; e = e->next()) {
        RID instance = e->get();
        RID mesh = region_meshes[e->key()];

        if (mesh.is_valid()) {
            VS::get_singleton()->free(mesh);
        }

        if (instance.is_valid()) {
            VS::get_singleton()->free(instance);
        }
    }
    region_meshes.clear();
    region_instances.clear();
}

void DungeonMapDebugRenderer::_update_visibility()
{
    if (!is_inside_tree())
        return;

    if (region_display_all) {
        set_region_display_all(is_visible());
    }

    if (tile_display_all) {
        set_tile_display_all(is_visible());
    }

    _change_notify("visible");
}

void DungeonMapDebugRenderer::_update_transform()
{

}

void DungeonMapDebugRenderer::_notification(int p_what)
{
    switch (p_what) {
        case NOTIFICATION_ENTER_WORLD: {
            update();
        } break;

        case NOTIFICATION_EXIT_WORLD: {
            clear();
        } break;

        case NOTIFICATION_VISIBILITY_CHANGED: {
            _update_visibility();
        } break;

        case NOTIFICATION_TRANSFORM_CHANGED: {
            _update_transform();
        } break;
	}
}

bool DungeonMapDebugRenderer::_set(const StringName &p_name, const Variant &p_value)
{
    if (p_name == "display_regions") {
        set_region_display_all((bool)p_value);
        return true;
    }
    if (p_name == "display_tile") {
        set_tile_display_all((bool)p_value);
        return true;
    }
    return false;
}

bool DungeonMapDebugRenderer::_get(const StringName &p_name, Variant &r_ret) const
{
    if (p_name == "display_regions") {
        r_ret = region_display_all;
        return true;
    }
    if (p_name == "display_tile") {
        r_ret = tile_display_all;
        return true;
    }
    return false;
}

void DungeonMapDebugRenderer::_get_property_list(List<PropertyInfo> *p_list) const
{
    p_list->push_back(PropertyInfo(Variant::BOOL, "display_regions", PROPERTY_HINT_NONE, "bool"));
    p_list->push_back(PropertyInfo(Variant::BOOL, "display_tile", PROPERTY_HINT_NONE, "bool"));
}

void DungeonMapDebugRenderer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("region_display", "region_id"), &DungeonMapDebugRenderer::region_display);
    ClassDB::bind_method(D_METHOD("tile_display", "tile_id"), &DungeonMapDebugRenderer::tile_display);
    ClassDB::bind_method(D_METHOD("tile_display_neighbors", "tile_id"), &DungeonMapDebugRenderer::tile_display_neighbors);

    ClassDB::bind_method(D_METHOD("set_region_display_all", "display"), &DungeonMapDebugRenderer::set_region_display_all);
    ClassDB::bind_method(D_METHOD("set_tile_display_all", "display"), &DungeonMapDebugRenderer::set_tile_display_all);

    ClassDB::bind_method(D_METHOD("update"), &DungeonMapDebugRenderer::update);
    ClassDB::bind_method(D_METHOD("clear"), &DungeonMapDebugRenderer::clear);
}

DungeonMapDebugRenderer::DungeonMapDebugRenderer()
{

}

DungeonMapDebugRenderer::~DungeonMapDebugRenderer()
{

}