/**********************************************************
 * Author:  Victor Holt
 * Date:    1/25/2018
 **********************************************************/
#ifndef DUNGEON_MAP_MESH_BUILDER_H
#define DUNGEON_MAP_MESH_BUILDER_H

#include <reference.h>
#include <math/aabb.h>
#include <scene/resources/surface_tool.h>

class DungeonMap;

class DungeonMapMeshBuilder
{
public:
    // Calculating the uv for the tile meshes.
    enum UVType
    {
        UV_XY = 0,
        UV_XZ,
        UV_YZ,
        UV_ZY
    };

    // Creates a tile mesh based on the given tile id.
    static void add_mesh_tile(DungeonMap* dungeon_map, SurfaceTool* tool, const Vector2& tile_id, int height, bool inverse = false);
    // Creates a tile mesh based on the given tile id.
    static void add_mesh_tile(DungeonMap* dungeon_map, SurfaceTool* tool, const Vector2& tile_id, bool inverse = false);

    // Creates a tile mesh based on the given tile id.
    static void add_mesh_tile_edge(DungeonMap* dungeon_map, SurfaceTool* tool, const Vector2& tile_id, int height, bool inverse = false);
    // Creates a tile mesh based on the given tile id.
    static void add_mesh_tile_edge(DungeonMap* dungeon_map, SurfaceTool* tool, const Vector2& tile_id, bool inverse = false);

    // Creates a mesh from a given aabb.
    static RID create_mesh_from_aabb(const AABB& aabb);
    // Creates mesh lines from a given aabb.
    static RID create_mesh_lines_from_aabb(const AABB& aabb);

    // Returns a uv coordinate for a vertex.
    static Vector2 get_uv(UVType uvType, const Vector3& vertex, float scale = 1.0f);
};

#endif