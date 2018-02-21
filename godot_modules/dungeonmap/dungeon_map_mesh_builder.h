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