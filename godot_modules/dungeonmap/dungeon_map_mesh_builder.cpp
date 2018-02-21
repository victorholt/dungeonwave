#include "dungeon_map_mesh_builder.h"
#include "dungeon_map.h"

#include <scene/resources/surface_tool.h>
#include <servers/visual_server.h>

// Using OpenGL (right-handed coord system) convention where -Z is forward...

void DungeonMapMeshBuilder::add_mesh_tile(DungeonMap* dungeon_map, SurfaceTool* tool, const Vector2& tile_id, int height, bool inverse)
{
    DungeonMap::Tile* tile = dungeon_map->find_tile(tile_id);
    if (!tile) {
        return;
    }
    float scale = dungeon_map->get_dungeon_params().floor_size;
    Vector2 origin = Vector2(tile->aabb.position.x, tile->aabb.position.z);
    Vector3 position = tile->aabb.position;

    if (height == -99999) {
        height = tile->height;
    }

    if (!inverse) {
        // Triangle 1
        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x, height, position.y), scale));
        tool->add_vertex(Vector3(origin.x, height, origin.y));

        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x, height, position.y + scale), scale));
        tool->add_vertex(Vector3(origin.x, height, origin.y - scale));

        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x + scale, height, position.y), scale));
        tool->add_vertex(Vector3(origin.x + scale, height, origin.y));

        // Triangle 2
        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x + scale, height, position.y), scale));
        tool->add_vertex(Vector3(origin.x + scale, height, origin.y));

        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x, height, position.y + scale), scale));
        tool->add_vertex(Vector3(origin.x, height, origin.y - scale));

        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x + scale, height, position.y + scale), scale));
        tool->add_vertex(Vector3(origin.x + scale, height, origin.y - scale));
    } else {
        // Triangle 1
        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x, height, position.y), scale));
        tool->add_vertex(Vector3(origin.x, height, origin.y));

        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x + scale, height, position.y), scale));
        tool->add_vertex(Vector3(origin.x + scale, height, origin.y));

        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x, height, position.y + scale), scale));
        tool->add_vertex(Vector3(origin.x, height, origin.y - scale));

        // Triangle 2
        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x + scale, height, position.y), scale));
        tool->add_vertex(Vector3(origin.x + scale, height, origin.y));

        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x + scale, height, position.y + scale), scale));
        tool->add_vertex(Vector3(origin.x + scale, height, origin.y - scale));

        tool->add_uv(get_uv(UVType::UV_XZ, Vector3(position.x, height, position.y + scale), scale));
        tool->add_vertex(Vector3(origin.x, height, origin.y - scale));
    }
}

void DungeonMapMeshBuilder::add_mesh_tile(DungeonMap* dungeon_map, SurfaceTool* tool, const Vector2& tile_id, bool inverse)
{
    add_mesh_tile(dungeon_map, tool, tile_id, -99999, inverse);
}

void DungeonMapMeshBuilder::add_mesh_tile_edge(DungeonMap* dungeon_map, SurfaceTool* tool, const Vector2& tile_id, int height, bool inverse)
{
    DungeonMap::Tile* tile = dungeon_map->find_tile(tile_id);
    if (!tile || tile->type == DungeonMap::TileType::EMPTY) {
        return;
    }

    Vector2 origin = Vector2(tile->aabb.position.x, tile->aabb.position.z);
    Vector3 position = tile->aabb.position;
    if (height == -99999) {
        height = tile->height;
    }
    float length = dungeon_map->get_dungeon_params().floor_size;
    float edge_slant = 0.0f;

    if (!tile->has_north_tile || ((DungeonMap::Tile*)tile->neighbors[(uint8_t)DungeonMap::NEIGHBOR_NORTH])->type == DungeonMap::EMPTY) {
        Vector3 v01 = Vector3(origin.x - edge_slant, 0.0f, origin.y - length - edge_slant);
        Vector3 v02 = Vector3(origin.x + length, height, origin.y - length);
        Vector3 v03 = Vector3(origin.x, height, origin.y - length);

        Vector3 v11 = Vector3(origin.x - edge_slant, 0.0f, origin.y - length - edge_slant);
        Vector3 v12 = Vector3(origin.x + length + edge_slant, 0.0f, origin.y - length - edge_slant);
        Vector3 v13 = Vector3(origin.x + length, height, origin.y - length);

        // Check if we need to inverse the mesh.
        if (inverse) {
            Vector3 tmp = v02;
            v02 = v03;
            v03 = tmp;

            tmp = v12;
            v12 = v13;
            v13 = tmp;
        }

        // Triangle #1
        tool->add_uv(get_uv(UVType::UV_XY, v01));
        tool->add_vertex(v01);

        tool->add_uv(get_uv(UVType::UV_XY, v02));
        tool->add_vertex(v02);

        tool->add_uv(get_uv(UVType::UV_XY, v03));
        tool->add_vertex(v03);

        // Triangle #2
        tool->add_uv(get_uv(UVType::UV_XY, v11));
        tool->add_vertex(v11);

        tool->add_uv(get_uv(UVType::UV_XY, v12));
        tool->add_vertex(v12);

        tool->add_uv(get_uv(UVType::UV_XY, v13));
        tool->add_vertex(v13);
    }

    if (!tile->has_east_tile || ((DungeonMap::Tile*)tile->neighbors[(uint8_t)DungeonMap::NEIGHBOR_EAST])->type == DungeonMap::EMPTY) {
        Vector3 v01 = Vector3(origin.x + length + edge_slant, 0.0f, origin.y + edge_slant);
        Vector3 v02 = Vector3(origin.x + length, height, origin.y);
        Vector3 v03 = Vector3(origin.x + length, height, origin.y - length);

        Vector3 v11 = Vector3(origin.x + length + edge_slant, 0.0f, origin.y + edge_slant);
        Vector3 v12 = Vector3(origin.x + length, height, origin.y - length);
        Vector3 v13 = Vector3(origin.x + length + edge_slant, 0.0f, origin.y - length - edge_slant);

        // Check if we need to inverse the mesh.
        if (inverse) {
            Vector3 tmp = v02;
            v02 = v03;
            v03 = tmp;

            tmp = v12;
            v12 = v13;
            v13 = tmp;
        }

        // Triangle #1
        tool->add_uv(get_uv(UVType::UV_ZY, v01));
        tool->add_vertex(v01);

        tool->add_uv(get_uv(UVType::UV_ZY, v02));
        tool->add_vertex(v02);

        tool->add_uv(get_uv(UVType::UV_ZY, v03));
        tool->add_vertex(v03);

        // Triangle #2
        tool->add_uv(get_uv(UVType::UV_ZY, v11));
        tool->add_vertex(v11);

        tool->add_uv(get_uv(UVType::UV_ZY, v12));
        tool->add_vertex(v12);

        tool->add_uv(get_uv(UVType::UV_ZY, v13));
        tool->add_vertex(v13);
    }

    if (!tile->has_south_tile || ((DungeonMap::Tile*)tile->neighbors[(uint8_t)DungeonMap::NEIGHBOR_SOUTH])->type == DungeonMap::EMPTY) {
        Vector3 v01 = Vector3(origin.x - edge_slant, 0.0f, origin.y + edge_slant);
        Vector3 v02 = Vector3(origin.x, height, origin.y);
        Vector3 v03 = Vector3(origin.x + length, height, origin.y);

        Vector3 v11 = Vector3(origin.x - edge_slant, 0.0f, origin.y + edge_slant);
        Vector3 v12 = Vector3(origin.x + length, height, origin.y);
        Vector3 v13 = Vector3(origin.x + length + edge_slant, 0.0f, origin.y + edge_slant);

        // Check if we need to inverse the mesh.
        if (inverse) {
            Vector3 tmp = v02;
            v02 = v03;
            v03 = tmp;

            tmp = v12;
            v12 = v13;
            v13 = tmp;
        }

        // Triangle #1
        tool->add_uv(get_uv(UVType::UV_XY, v01));
        tool->add_vertex(v01);

        tool->add_uv(get_uv(UVType::UV_XY, v02));
        tool->add_vertex(v02);

        tool->add_uv(get_uv(UVType::UV_XY, v03));
        tool->add_vertex(v03);

        // Triangle #2
        tool->add_uv(get_uv(UVType::UV_XY, v11));
        tool->add_vertex(v11);

        tool->add_uv(get_uv(UVType::UV_XY, v12));
        tool->add_vertex(v12);

        tool->add_uv(get_uv(UVType::UV_XY, v13));
        tool->add_vertex(v13);
    }

    if (!tile->has_west_tile || ((DungeonMap::Tile*)tile->neighbors[(uint8_t)DungeonMap::NEIGHBOR_WEST])->type == DungeonMap::EMPTY) {
        Vector3 v01 = Vector3(origin.x - edge_slant, 0.0f, origin.y + edge_slant);
        Vector3 v02 = Vector3(origin.x, height, origin.y - length);
        Vector3 v03 = Vector3(origin.x, height, origin.y);

        Vector3 v11 = Vector3(origin.x - edge_slant, 0.0f, origin.y + edge_slant);
        Vector3 v12 = Vector3(origin.x - edge_slant, 0.0f, origin.y - length - edge_slant);
        Vector3 v13 = Vector3(origin.x, height, origin.y - length);

        // Check if we need to inverse the mesh.
        if (inverse) {
            Vector3 tmp = v02;
            v02 = v03;
            v03 = tmp;

            tmp = v12;
            v12 = v13;
            v13 = tmp;
        }

        // Triangle #1
        tool->add_uv(get_uv(UVType::UV_ZY, v01));
        tool->add_vertex(v01);

        tool->add_uv(get_uv(UVType::UV_ZY, v02));
        tool->add_vertex(v02);

        tool->add_uv(get_uv(UVType::UV_ZY, v03));
        tool->add_vertex(v03);

        // Triangle #2
        tool->add_uv(get_uv(UVType::UV_ZY, v11));
        tool->add_vertex(v11);

        tool->add_uv(get_uv(UVType::UV_ZY, v12));
        tool->add_vertex(v12);

        tool->add_uv(get_uv(UVType::UV_ZY, v13));
        tool->add_vertex(v13);
    }
}

void DungeonMapMeshBuilder::add_mesh_tile_edge(DungeonMap* dungeon_map, SurfaceTool* tool, const Vector2& tile_id, bool inverse)
{
    add_mesh_tile_edge(dungeon_map, tool, tile_id, -99999, inverse);
}

RID DungeonMapMeshBuilder::create_mesh_from_aabb(const AABB& aabb)
{
    // Create the debug render aabb instance.
    RID mesh = VS::get_singleton()->mesh_create();
    Vector3 pos = aabb.position;
    Vector3 size = aabb.size;

    SurfaceTool st;
    st.begin(Mesh::PRIMITIVE_TRIANGLES);

    // Front
    st.add_vertex(Vector3(pos.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z));

    // Back
    st.add_vertex(Vector3(pos.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z - size.z));

    // Left
    st.add_vertex(Vector3(pos.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z));

    st.add_vertex(Vector3(pos.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z));

    // Right
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z - size.z));

    // Top
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z - size.z));

    // Bottom
    st.add_vertex(Vector3(pos.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y, pos.z - size.z));

    st.index();
    Ref<ArrayMesh> arrMesh = st.commit();

    VS::get_singleton()->mesh_add_surface_from_arrays(
        mesh,
        VisualServer::PRIMITIVE_TRIANGLES,
        arrMesh->surface_get_arrays(0)
    );

    return mesh;
}

RID DungeonMapMeshBuilder::create_mesh_lines_from_aabb(const AABB& aabb)
{
    RID mesh = VS::get_singleton()->mesh_create();
    Vector3 pos = aabb.position;
    Vector3 size = aabb.size;
    SurfaceTool st;
    st.begin(Mesh::PRIMITIVE_LINES);

    // Front
    st.add_vertex(Vector3(pos.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z));

    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y, pos.z));

    // Back
    st.add_vertex(Vector3(pos.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y, pos.z - size.z));

    // Left
    st.add_vertex(Vector3(pos.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z));

    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x, pos.y + size.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x, pos.y, pos.z));

    // Right
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y + size.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z - size.z));

    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z - size.z));
    st.add_vertex(Vector3(pos.x + size.x, pos.y, pos.z));

    st.index();
    Ref<ArrayMesh> arrMesh = st.commit();

    VS::get_singleton()->mesh_add_surface_from_arrays(
        mesh,
        VisualServer::PRIMITIVE_LINES,
        arrMesh->surface_get_arrays(0)
    );

    arrMesh.unref();
    return mesh;
}

Vector2 DungeonMapMeshBuilder::get_uv(UVType uvType, const Vector3& vertex, float scale)
{
    Vector2 ret;
    if (uvType == UVType::UV_XY) {
        ret = Vector2(vertex.x / scale, vertex.y / scale);
    } else if (uvType == UVType::UV_XZ) {
        ret = Vector2(vertex.x / scale, vertex.z / scale);
    } else if (uvType == UVType::UV_YZ) {
        ret = Vector2(vertex.y / scale, vertex.z / scale);
    } else if (uvType == UVType::UV_ZY) {
        ret = Vector2(vertex.z / scale, vertex.y / scale);
    }
    return ret;
}