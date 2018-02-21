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