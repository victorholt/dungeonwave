#include "dungeon_map.h"
#include "dungeon_map_mesh_builder.h"

#include <print_string.h>
#include <servers/physics_server.h>

void DungeonMap::apply()
{
    if (!is_inside_tree()) {
        return;
    }

    if (!is_dirty()) {
        return;
    }

    // Clear the previous data.
    clear();

    _build_regions();
    _update_tile_neighbors();
    _build_region_meshes();
    _build_region_mesh_edges();

    // Update all regions as not dirty.
    for (Map<Vector2, Region*>::Element* e = regions.front(); e; e = e->next()) {
        Region* region = e->get();
        region->dirty = false;
    }
    dirty = false;

    // Attempt to apply the nav mesh.
    // Node* node = get_parent()->get_node(NodePath("Navigation"));
    // if (navigation_node) {
    //     add_navigation_meshes(navigation_node);
    // }

    emit_signal("dungeon_map_apply_completed");
}

void DungeonMap::clear()
{
    valid_tiles.clear();
    for (Map<Vector2, Tile*>::Element* e = tiles.front(); e; e = e->next()) {
        Tile* tile = e->get();
		memdelete(tile);
	}
    tiles.clear();

    // Attempt to remove the nav meshes.
    if (navigation_node) {
        remove_navigation_meshes(navigation_node);
    }
    navigation_node = NULL;

    // Delete all regions in our dictionary.
    for (Map<Vector2, Region*>::Element* e = regions.front(); e; e = e->next()) {
        Region* region = e->get();

        // Free meshes.
        if (region->mesh.is_valid()) {
            region->mesh.unref();
        }
        if (region->edge_mesh.is_valid()) {
            region->edge_mesh.unref();
        }

        // Free instance.
        if (region->instance.is_valid()) {
            VS::get_singleton()->free(region->instance);
        }

        if (region->edge_instance.is_valid()) {
            VS::get_singleton()->free(region->edge_instance);
        }

        // Delete collision.
        if (region->collision_shape) {
            region->collision_shape->queue_delete();
        }
        if (region->collision_body) {
            // region->collision_body->set_owner(NULL);
            // remove_child(region->collision_body);
            // PhysicsServer::get_singleton()->free(region->collision_body->get_rid());
            region->collision_body->queue_delete();
        }

        if (region->edge_collision_shape) {
            region->edge_collision_shape->queue_delete();
        }
        if (region->edge_collision_body) {
            // remove_child(region->edge_collision_body);
            // region->edge_collision_body->set_owner(NULL);
            // PhysicsServer::get_singleton()->free(region->edge_collision_body->get_rid());
            region->edge_collision_body->queue_delete();
        }

        region->tiles.clear();

        if (region->nav_mesh.is_valid()) {
            region->nav_mesh.unref();
        }
        memdelete(region);
	}
    regions.clear();
    dirty = true;
}

void DungeonMap::generate_map_image()
{
    if (!is_inside_tree()) {
        return;
    }

    // Create the map image/texture.
    map_builder.params.dungeon_size = region_size * map_builder.params.floor_size * tiles_per_region;
    print_line(String("Dungeon Size = ") + itos(map_builder.params.dungeon_size));

    map_builder.create_map_image();
    map_builder.generate_map_image();
    emit_signal("dungeon_map_image_generated");
}

DungeonMap::Region* DungeonMap::find_region(const Vector2& region_id)
{
    if (!regions.has(region_id)) {
        return NULL;
    }
    return regions[region_id];
}

DungeonMap::Tile* DungeonMap::find_tile(const Vector2& tile_id)
{
    if (!tiles.has(tile_id)) {
        return NULL;
    }
    return tiles[tile_id];
}

Vector2 DungeonMap::get_random_map_location()
{
    if (valid_tiles.size() == 0) {
        print_line("No valid tiles to spawn on!!!");
        return Vector2();
    }

    Vector2 position = valid_tiles[Math::rand() % valid_tiles.size()];
    Tile* tile = find_tile(position);
    float floor_size_half = (float)(get_dungeon_params().floor_size) / 2.0f;
    if (!tile) {
        return Vector2();
    }

    return Vector2(
        position.x + floor_size_half,
        position.y - floor_size_half
    );
}

Vector2 DungeonMap::get_tile_position(const Vector2& position)
{
    return Vector2(
        Math::floor(position.x - ((int)position.x % get_dungeon_params().floor_size)),
        Math::floor(position.y - ((int)position.y % get_dungeon_params().floor_size))
    );
}

bool DungeonMap::is_valid_position(const Vector2& position)
{
    Tile* tile = find_tile(get_tile_position(position));
    if (!tile) return false;

    return tile->type != EMPTY;
}

void DungeonMap::set_map_tile_color(const Vector2& tile_id, Color color)
{
    map_builder.set_map_tile_color(tile_id, color);
}

void DungeonMap::add_navigation_meshes(Node* navigation)
{
    if (!is_inside_tree())
        return;

    if (navigation && !navigation->is_class("Navigation")) {
        return;
    }

    navigation_node = navigation;
    for (auto e = regions.front(); e; e = e->next()) {
        auto region = e->get();
        if (region->nav_mesh.is_valid()) {
            region->nav_mesh_id = ((Navigation*)(navigation))->navmesh_add(region->nav_mesh, region->transform, navigation);
            // print_line(String("Added navmesh id: ") + itos(region->nav_mesh_id) + String(" with polygon count: ") + itos(region->nav_mesh->get_polygon_count()));
        }
    }
}

void DungeonMap::remove_navigation_meshes(Node* navigation)
{
    if (!is_inside_tree())
        return;

    if (navigation && !navigation->is_class("Navigation")) {
        return;
    }

    for (auto e = regions.front(); e; e = e->next()) {
        auto region = e->get();
        if (region->nav_mesh.is_valid()) {
            ((Navigation*)(navigation))->navmesh_remove(region->nav_mesh_id);
            region->nav_mesh_id = 0;
        }
    }
}

Array DungeonMap::get_nav_meshes() const
{
    Array nav_meshes;
    if (!is_inside_tree())
        return nav_meshes;

    for (auto e = regions.front(); e; e = e->next()) {
        auto region = e->get();
        if (region->nav_mesh.is_valid()) {
            nav_meshes.push_back(region->nav_mesh);
        }
    }
    return nav_meshes;
}

void DungeonMap::_build_regions()
{
    int tile_size = map_builder.params.floor_size;
    int region_width = tiles_per_region * tile_size;

    // Build out our region and the cells.
    for (int x = 0; x < region_size; x++) {
        for (int y = 0; y < region_size; y++) {
            Vector2 id = Vector2(x, y);
            if (regions.has(id))
                continue;

            Region* region = memnew(Region);
            region->dirty = true;
            region->aabb = AABB(
                Vector3(
                    x * region_width,
                    0.0f,
                    y * region_width * -1.0f
                ),
                Vector3(
                    region_width,
                    2,
                    region_width
                )
            );
            region->id = Vector2(region->aabb.position.x, region->aabb.position.z);

            region->transform.translated(region->aabb.position);
            regions[region->id] = region;
            _build_tiles(region->id);
        }
    }
}

void DungeonMap::_build_tiles(const Vector2& region_id)
{
    // Build out the tiles per region.
    Ref<Image> map_image = get_map_image();
    int tile_size = map_builder.params.floor_size;
    int region_width = tile_size * tile_size;

    for (int x = 0; x < tiles_per_region; x++) {
        for (int y = 0; y < tiles_per_region; y++) {
            if (!regions.has(region_id)) {
                continue;
            }

            // Create our tile.
            Region* region = regions[region_id];
            Tile* tile = memnew(Tile);
            tile->local_position = Vector2(x, y);
            tile->region_id = region_id;
            tile->aabb = AABB(
                Vector3(
                    (x * tile_size) + region->aabb.position.x,
                    0.0f,
                    (y * tile_size * -1.0f) + region->aabb.position.z
                ),
                Vector3(
                    tile_size,
                    tile->height,
                    tile_size
                )
            );
            tile->id = Vector2(tile->aabb.position.x, tile->aabb.position.z);

            int px = (int)Math::floor(tile->id.x);
            int py = (int)Math::floor(tile->id.y * -1.0f);

            map_image->lock();
            Color pixel = map_image->get_pixel(px, py);
            map_image->unlock();

            if (pixel != Color(0, 0, 0)) {
                tile->type = FLOOR;
                valid_tiles.push_back(tile->id);
            } else {
                tile->height = -1;
            }

            // Save our tile.
            tile->transform.translated(tile->aabb.position);
            tiles[tile->id] = tile;

            // Add tile to the region. Check for empty so
            // we know how to build the mesh.
            if (tile->type != EMPTY) {
                region->tiles.push_back(tile->id);
            }
        }
    }
}

void DungeonMap::_update_tile_neighbors()
{
    // Update the neighbors.
    for (Map<Vector2, Tile*>::Element* e = tiles.front(); e; e = e->next()) {
        Tile* tile = e->get();
        if (tile->neighbors.size() == 0) {
            tile->neighbors.resize((uint8_t)MAX_TILE_NEIGHBORS);
        }

        int tile_size = map_builder.params.floor_size;
        Vector2 tile_id = tile->id;
        Vector2 north_pos = Vector2(tile_id.x, tile_id.y - tile_size);
        Vector2 east_pos = Vector2(tile_id.x + tile_size, tile_id.y);
        Vector2 south_pos = Vector2(tile_id.x, tile_id.y + tile_size);
        Vector2 west_pos = Vector2(tile_id.x - tile_size, tile_id.y);

        Vector2 north_east_pos = Vector2(tile_id.x + tile_size, tile_id.y - tile_size);
        Vector2 north_west_pos = Vector2(tile_id.x - tile_size, tile_id.y - tile_size);
        Vector2 south_east_pos = Vector2(tile_id.x + tile_size, tile_id.y + tile_size);
        Vector2 south_west_pos = Vector2(tile_id.x - tile_size, tile_id.y + tile_size);

        // Set the north tile.
        Tile* nTile = find_tile(north_pos);
        if (nTile) {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_NORTH] = nTile;
            tile->has_north_tile = true;
        } else {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_NORTH] = NULL;
        }

        // Set the east cell.
        nTile = find_tile(east_pos);
        if (nTile) {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_EAST] = nTile;
            tile->has_east_tile = true;
        } else {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_EAST] = NULL;
        }

        // Set the south cell.
        nTile = find_tile(south_pos);
        if (nTile) {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_SOUTH] = nTile;
            tile->has_south_tile = true;
        } else {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_SOUTH] = NULL;
        }

        // Set the west cell.
        nTile = find_tile(west_pos);
        if (nTile) {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_WEST] = nTile;
            tile->has_west_tile = true;
        } else {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_WEST] = NULL;
        }

        // Set the north-east cell.
        nTile = find_tile(north_east_pos);
        if (nTile) {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_NORTH_EAST] = nTile;
            tile->has_north_east_tile = true;
        } else {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_NORTH_EAST] = NULL;
        }

        // Set the north-west cell.
        nTile = find_tile(north_west_pos);
        if (nTile) {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_NORTH_WEST] = nTile;
            tile->has_north_west_tile = true;
        } else {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_NORTH_WEST] = NULL;
        }

        // Set the south-east cell.
        nTile = find_tile(south_east_pos);
        if (nTile) {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_SOUTH_EAST] = nTile;
            tile->has_south_east_tile = true;
        } else {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_SOUTH_EAST] = NULL;
        }

        // Set the south-west cell.
        nTile = find_tile(south_west_pos);
        if (nTile) {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_SOUTH_WEST] = nTile;
            tile->has_south_west_tile = true;
        } else {
            tile->neighbors[(uint8_t)Neighbor::NEIGHBOR_SOUTH_WEST] = NULL;
        }
    }
}

void DungeonMap::_build_region_meshes()
{
    for (Map<Vector2, Region*>::Element* e = regions.front(); e; e = e->next()) {
        Region* region = e->get();
        if (region->tiles.size() == 0) continue;

        SurfaceTool tool;
        tool.begin(Mesh::PRIMITIVE_TRIANGLES);
        for (int i = 0; i < region->tiles.size(); i++) {
            DungeonMapMeshBuilder::add_mesh_tile(this, &tool, region->tiles[i], false);
            DungeonMapMeshBuilder::add_mesh_tile(this, &tool, region->tiles[i], ceiling_height, true);
        }
        tool.generate_normals();
        tool.index();
        region->mesh = tool.commit();

        // Create the mesh instance.
        region->instance = VS::get_singleton()->instance_create2(region->mesh->get_rid(), get_world()->get_scenario());
        VS::get_singleton()->instance_set_transform(region->instance, region->transform);
        VS::get_singleton()->instance_set_visible(region->instance, true);

        // Create the collision shape.
        _create_region_collision(region->id);

        // Create the navigation mesh.
        Ref<NavigationMesh> nav_mesh = memnew(NavigationMesh);
        region->nav_mesh = nav_mesh;
        region->nav_mesh->create_from_mesh(region->mesh);
    }
}

void DungeonMap::_build_region_mesh_edges()
{
    for (Map<Vector2, Region*>::Element* e = regions.front(); e; e = e->next()) {
        Region* region = e->get();
        if (region->tiles.size() == 0) continue;

        SurfaceTool tool;
        tool.begin(Mesh::PRIMITIVE_TRIANGLES);
        for (int i = 0; i < region->tiles.size(); i++) {
            DungeonMapMeshBuilder::add_mesh_tile_edge(this, &tool, region->tiles[i], ceiling_height, true);
        }
        tool.generate_normals();
        tool.index();
        region->edge_mesh = tool.commit();

        // Create the edge mesh instance.
        region->edge_instance = VS::get_singleton()->instance_create2(region->edge_mesh->get_rid(), get_world()->get_scenario());
        VS::get_singleton()->instance_set_transform(region->edge_instance, region->transform);
        VS::get_singleton()->instance_set_visible(region->edge_instance, true);

        // Create the collision shape.
        _create_region_edge_collision(region->id);
    }
}

void DungeonMap::_create_collision(StaticBody*& collision_body, CollisionShape*& collision_shape, Ref<Mesh> mesh, const Transform& transform)
{
    if (mesh.is_null())
        return;

    Ref<Shape> shape = mesh->create_trimesh_shape();
    if (shape.is_null())
        return;

    collision_body = memnew(StaticBody);
    collision_shape = memnew(CollisionShape);
    collision_shape->set_shape(shape);
    collision_body->add_child(collision_shape);
    collision_shape->set_owner(collision_body);

    // Build the collision.
    // String("region_") + region->id + "_col"
    collision_body->set_name("collision_body");
    collision_body->set_transform(transform);

    add_child(collision_body);
    collision_body->set_owner(this);
}

void DungeonMap::_create_region_collision(const Vector2& region_id)
{
    Region* region = find_region(region_id);
    if (!region || !region->dirty) return;

    if (region->mesh.is_null())
        return;

    if (region->collision_body != NULL) {
        region->collision_body->remove_child(region->collision_shape);
        remove_child(region->collision_body);
        region->collision_body->set_owner(NULL);

        memdelete(region->collision_shape);
        memdelete(region->collision_body);
    }

    _create_collision(region->collision_body, region->collision_shape, region->mesh, region->transform);
}

void DungeonMap::_create_region_edge_collision(const Vector2& region_id)
{
    Region* region = find_region(region_id);
    if (!region || !region->dirty) return;

    if (region->edge_mesh.is_null())
        return;

    if (region->edge_collision_body != NULL) {
        region->edge_collision_body->remove_child(region->edge_collision_shape);
        remove_child(region->edge_collision_body);
        region->edge_collision_body->set_owner(NULL);

        memdelete(region->edge_collision_shape);
        memdelete(region->edge_collision_body);
    }

    _create_collision(region->edge_collision_body, region->edge_collision_shape, region->edge_mesh, region->transform);
}

void DungeonMap::_update_visibility()
{
    if (!is_inside_tree())
        return;

    for (auto e = regions.front(); e; e = e->next()) {
        auto region = e->get();

        if (region->instance.is_valid()) {
            VS::get_singleton()->instance_set_visible(region->instance, is_visible());
        }
        if (region->edge_mesh.is_valid()) {
            VS::get_singleton()->instance_set_visible(region->edge_instance, is_visible());
        }
    }
    _change_notify("visible");
}

void DungeonMap::_update_transform()
{

}

void DungeonMap::_update_material()
{

}

void DungeonMap::_notification(int p_what)
{
    switch (p_what) {
        case NOTIFICATION_ENTER_WORLD: {
            print_line("DungeonMap::entering world");
            generate_map_image();
            apply();
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

bool DungeonMap::_set(const StringName &p_name, const Variant &p_value)
{
    return false;
}

bool DungeonMap::_get(const StringName &p_name, Variant &r_ret) const
{
    return false;
}

void DungeonMap::_get_property_list(List<PropertyInfo> *p_list) const
{
    p_list->push_back(PropertyInfo(Variant::OBJECT, "material/floor", PROPERTY_HINT_RESOURCE_TYPE, "ShaderMaterial,SpatialMaterial"));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "material/wall", PROPERTY_HINT_RESOURCE_TYPE, "ShaderMaterial,SpatialMaterial"));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "material/water", PROPERTY_HINT_RESOURCE_TYPE, "ShaderMaterial,SpatialMaterial"));
}

void DungeonMap::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("apply"), &DungeonMap::apply);
    ClassDB::bind_method(D_METHOD("clear"), &DungeonMap::clear);

    ClassDB::bind_method(D_METHOD("mark_dirty"), &DungeonMap::mark_dirty);
    ClassDB::bind_method(D_METHOD("is_dirty"), &DungeonMap::is_dirty);

    ClassDB::bind_method(D_METHOD("generate_map_image"), &DungeonMap::generate_map_image);

    ClassDB::bind_method(D_METHOD("get_nav_meshes"), &DungeonMap::get_nav_meshes);
    ClassDB::bind_method(D_METHOD("add_navigation_meshes", "navigation"), &DungeonMap::add_navigation_meshes);
    ClassDB::bind_method(D_METHOD("remove_navigation_meshes", "navigation"), &DungeonMap::remove_navigation_meshes);

    ClassDB::bind_method(D_METHOD("get_map_image"), &DungeonMap::get_map_image);
    ClassDB::bind_method(D_METHOD("get_map_texture"), &DungeonMap::get_map_texture);

    ClassDB::bind_method(D_METHOD("set_region_size", "size"), &DungeonMap::set_region_size);
    ClassDB::bind_method(D_METHOD("get_region_size"), &DungeonMap::get_region_size);
    ClassDB::bind_method(D_METHOD("set_tiles_per_region", "tiles"), &DungeonMap::set_tiles_per_region);
    ClassDB::bind_method(D_METHOD("get_tiles_per_region"), &DungeonMap::get_tiles_per_region);
    ClassDB::bind_method(D_METHOD("set_ceiling_height", "height"), &DungeonMap::set_ceiling_height);
    ClassDB::bind_method(D_METHOD("get_ceiling_height"), &DungeonMap::get_ceiling_height);

    ClassDB::bind_method(D_METHOD("set_dungeon_seed", "seed"), &DungeonMap::set_dungeon_seed);
    ClassDB::bind_method(D_METHOD("get_dungeon_seed"), &DungeonMap::get_dungeon_seed);

    ClassDB::bind_method(D_METHOD("get_random_map_location"), &DungeonMap::get_random_map_location);
    ClassDB::bind_method(D_METHOD("get_tile_position", "position"), &DungeonMap::get_tile_position);
    ClassDB::bind_method(D_METHOD("is_valid_position", "position"), &DungeonMap::is_valid_position);

    ClassDB::bind_method(D_METHOD("set_map_tile_color", "tile_id", "color"), &DungeonMap::set_map_tile_color);

    ADD_SIGNAL(MethodInfo("dungeon_map_image_generated"));
    ADD_SIGNAL(MethodInfo("dungeon_map_apply_completed"));
}

DungeonMap::DungeonMap()
{
    //set_notify_transform(true);
}

DungeonMap::~DungeonMap()
{
    // clear();
}