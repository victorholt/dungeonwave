/**********************************************************
 * Author:  Victor Holt
 * The MIT License (MIT)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 **********************************************************/

#ifndef DUNGEON_MAP_H
#define DUNGEON_MAP_H
#include <reference.h>
#include <image.h>
#include <math/aabb.h>
#include <scene/3d/visual_instance.h>
#include <scene/3d/spatial.h>
#include <scene/3d/collision_object.h>
#include <scene/3d/physics_body.h>
#include <scene/3d/collision_shape.h>
#include <scene/3d/mesh_instance.h>
#include <scene/3d/navigation.h>
#include <scene/3d/navigation_mesh.h>
#include <scene/resources/mesh.h>
#include <scene/resources/multimesh.h>
#include <scene/resources/material.h>
#include <scene/resources/texture.h>

#include "dungeon_map_builder.h"

class DungeonMap : public Spatial
{
    GDCLASS(DungeonMap, Spatial);
    OBJ_CATEGORY("3D Visual Nodes");

public:
    // Types of tiles that can be generated.
    enum TileType
    {
        EMPTY = 0,
        FLOOR,
        WALL,
        WATER,
        MAX_TILE_TYPES
    };

    // Tile neighbors
    enum Neighbor
    {
        NEIGHBOR_NORTH = 0,
        NEIGHBOR_EAST,
        NEIGHBOR_SOUTH,
        NEIGHBOR_WEST,

        NEIGHBOR_NORTH_EAST,
        NEIGHBOR_NORTH_WEST,
        NEIGHBOR_SOUTH_EAST,
        NEIGHBOR_SOUTH_WEST,

        MAX_TILE_NEIGHBORS
    };

    // Regions are groups of tiles.
    struct Region
    {
        // Position of the region.
        Vector2 id;
        // Tiles in the region.
        Vector<Vector2> tiles;
        // AABB for the region.
        AABB aabb;

        // Region mesh instance.
        RID instance;
        // Region edge mesh instance.
        RID edge_instance;

        // Reference to the land mesh.
        Ref<Mesh> mesh;
        // Reference to the edge mesh.
        Ref<Mesh> edge_mesh;
        // Reference to the navigation mesh.
        Ref<NavigationMesh> nav_mesh;
        // NavigationMesh id when added to the Navigation node.
        int nav_mesh_id = 0;

        // Reference to the collision body.
        StaticBody* collision_body = NULL;
        // Reference to the collision shape.
        CollisionShape* collision_shape = NULL;

        // Reference to the collision body.
        StaticBody* edge_collision_body = NULL;
        // Reference to the collision shape.
        CollisionShape* edge_collision_shape = NULL;

        // Transform for the region.
        Transform transform;

        // Whether or not the region needs to be regenerated.
        bool dirty = true;
    };

    // Tile that makes up a level.
    struct Tile
    {
        // Position of the tile.
        Vector2 id;
        // Region id for the tile.
        Vector2 region_id;
        // Local position relative to the region.
        Vector2 local_position;
        // Type of tile when generating the mesh.
        TileType type = TileType::EMPTY;
        // Tile Neighbors
        Vector<Tile*> neighbors;

        // AABB for the tile.
        AABB aabb;

        // Height of the cell.
        int height = 0;

        // Transform for the tile.
        Transform transform;

        // Has a north tile neighbor.
        bool has_north_tile = false;
        // Has a east tile neighbor.
        bool has_east_tile = false;
        // Has a south tile neighbor.
        bool has_south_tile = false;
        // Has a west tile neighbor.
        bool has_west_tile = false;
        // Has a north-east tile neighbor.
        bool has_north_east_tile = false;
        // Has a north-west tile neighbor.
        bool has_north_west_tile = false;
        // Has a south-east tile neighbor.
        bool has_south_east_tile = false;
        // Has a south-west tile neighbor.
        bool has_south_west_tile = false;
    };

private:
    // Reference to the map builder.
    DungeonMapBuilder map_builder;

    // Regions of the map.
    Map<Vector2, Region*> regions;
    // Tiles of the map.
    Map<Vector2, Tile*> tiles;
    // List of valid tiles for the current map.
    Vector<Vector2> valid_tiles;

    // Reference to the navigation node.
    Node* navigation_node = NULL;

    // Number of regions (region_size * region_size).
    int region_size = 8;
    // Tiles per region.
    int tiles_per_region = 4;
    // Celling height.
    int ceiling_height = 8;

    // Flag for whether or not the dungeon is dirty.
    bool dirty = true;

    // Build the regions/tiles.
    void _build_regions();
    // Build the tiles.
    void _build_tiles(const Vector2& region_id);
    // Update the tile neighbors.
    void _update_tile_neighbors();

    // Builds the region meshes.
    void _build_region_meshes();
    // Builds the region mesh edges.
    void _build_region_mesh_edges();

    // Create a collision mesh from a given mesh.
    void _create_collision(StaticBody*& collision_body, CollisionShape*& collision_shape, Ref<Mesh> mesh, const Transform& transform);

    // Create the collisions for a region.
    void _create_region_collision(const Vector2& region_id);

    // Create the edge collisions for a region.
    void _create_region_edge_collision(const Vector2& region_id);

    // Updates the visibility of the node.
	void _update_visibility();
    // Updates the grid transformation.
    void _update_transform();
    // Updates the material for the terrain.
    void _update_material();

protected:
    // Handles notifications for the node.
    void _notification(int p_what);
    // Set property from the editor.
    bool _set(const StringName &p_name, const Variant &p_value);
    // Retrieve property from the editor.
    bool _get(const StringName &p_name, Variant &r_ret) const;
    // Retrieves the properly list for the editor.
    void _get_property_list(List<PropertyInfo> *p_list) const;
    // Binds methods for the terrain.
    static void _bind_methods();

public:
    // Constructor.
    DungeonMap();
    // Destructor.
    virtual ~DungeonMap();

    // Applies all changes to the dungeon.
    void apply();
    // Clears the dungeon objects.
    void clear();

    // Generates the map image.
    void generate_map_image();

    // Attempts to return a region based on the given coordinates/id.
    Region* find_region(const Vector2& region_id);
    // Attempts to return a tile based on the given coordinates/id.
    Tile* find_tile(const Vector2& tile_id);

    // Returns a random map location (based on the initial seed).
    Vector2 get_random_map_location();

    // Returns the "snapped" tile position given a position.
    Vector2 get_tile_position(const Vector2& position);

    // Checks if a position is valid.
    bool is_valid_position(const Vector2& position);

    // Set the map image color for a specific tile.
    void set_map_tile_color(const Vector2& tile_id, Color color);

    // Adds the navigation meshes.
    void add_navigation_meshes(Node* navigation);
    // Removes the navigation meshes.
    void remove_navigation_meshes(Node* navigation);

    // Returns the navigation meshes.
    Array get_nav_meshes() const;

    // Marks the terrain as dirty.
    _FORCE_INLINE_ void mark_dirty() { dirty = true; }
    // Check if the terrain is dirty.
    _FORCE_INLINE_ bool is_dirty() { return dirty; }

    // Returns the map image.
    _FORCE_INLINE_ Ref<Image> get_map_image() const { return map_builder.get_map_image(); }
    // Returns the map texture.
    _FORCE_INLINE_ Ref<ImageTexture> get_map_texture() const { return map_builder.get_map_texture(); }

    // Sets the size of the region.
    _FORCE_INLINE_ void set_region_size(int size) { mark_dirty(); region_size = size; }
    // Returns the size of the region.
    _FORCE_INLINE_ int get_region_size() const { return region_size; }
    // Sets the tiles per region.
    _FORCE_INLINE_ void set_tiles_per_region(int tiles) { mark_dirty(); tiles_per_region = tiles; }
    // Returns the tile per region.
    _FORCE_INLINE_ int get_tiles_per_region() const { return tiles_per_region; }

    // Set the ceiling height.
    _FORCE_INLINE_ void set_ceiling_height(int height) { ceiling_height = height; }
    // Returns the ceiling height.
    _FORCE_INLINE_ int get_ceiling_height() const { return ceiling_height; }

    // Returns the dungeon map builder params.
    _FORCE_INLINE_ const DungeonMapBuilder::DungeonParams& get_dungeon_params() const { return map_builder.params; }

    // Sets the dungeon seed.
    _FORCE_INLINE_ void set_dungeon_seed(int64_t seed) { mark_dirty(); map_builder.params.seed = seed; }
    // Returns the dungeon seed.
    _FORCE_INLINE_ int64_t get_dungeon_seed() const { return map_builder.params.seed; }

    // Returns all regions in the map.
    _FORCE_INLINE_ const Map<Vector2, Region*>& get_regions() const { return regions; }
    // Returns all tiles in the map.
    _FORCE_INLINE_ const Map<Vector2, Tile*>& get_tiles() const { return tiles; }
};

#endif