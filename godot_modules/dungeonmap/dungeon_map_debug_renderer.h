/**********************************************************
 * Author:  Victor Holt
 * Date:    3/3/2018
 **********************************************************/
#ifndef DUNGEON_MAP_DEBUG_RENDERER_H
#define DUNGEON_MAP_DEBUG_RENDERER_H
#include <reference.h>
#include <image.h>
#include <math/aabb.h>
#include <scene/3d/visual_instance.h>
#include <scene/3d/spatial.h>
#include <scene/3d/mesh_instance.h>
#include <scene/resources/mesh.h>
#include <scene/resources/material.h>

#include "dungeon_map.h"

class DungeonMapDebugRenderer : public Spatial
{
    GDCLASS(DungeonMapDebugRenderer, Spatial);
    OBJ_CATEGORY("3D Visual Nodes");

    /// Parent node for the debug renderer.
    DungeonMap* parent = NULL;

    /// Region view instances.
    Map<Vector2, RID> region_instances;
    /// Region debug meshes.
    Map<Vector2, RID> region_meshes;

    /// Tile view instances.
    Map<Vector2, RID> tile_instances;
    /// Tile debug meshes.
    Map<Vector2, RID> tile_meshes;

    /// Debug material instance for the region.
    Ref<SpatialMaterial> debug_region_material;
    /// Debug material instance for the tiles.
    Ref<SpatialMaterial> debug_tile_material;

    /// Flag to display all regions.
    bool region_display_all = false;
    /// Flag to display all tiles.
    bool tile_display_all = false;

protected:
    // Updates the visibility of the node.
	void _update_visibility();
    // Updates the grid transformation.
    void _update_transform();

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
    DungeonMapDebugRenderer();
    // Destructor.
    ~DungeonMapDebugRenderer();

    // Displays a region's debug aabb.
    void region_display(const Vector2& region_id);
    // Display's a tile's debug aabb.
    void tile_display(const Vector2& tile_id);
    // Displays a tile's aabb and the neighbor's aabb.
    void tile_display_neighbors(const Vector2& tile_id);

    // Sets whether or not to display all region debug meshes.
    void set_region_display_all(bool display);
    // Sets whether or not to display all tile debug meshes.
    void set_tile_display_all(bool display);

    // Clears and updates the debug renderer instances/meshes.
    void update();
    // Clears the debug renderer instances/meshes.
    void clear();
};

#endif