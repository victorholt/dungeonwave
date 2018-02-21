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

#ifndef DUNGEON_MAP_BUILDER_H
#define DUNGEON_MAP_BUILDER_H
#include <reference.h>
#include <image.h>
#include <scene/resources/texture.h>

class DungeonMapBuilder
{
public:
    struct DungeonParams
    {
        // Seed for generating maps.
        // long seed = 345432342352;
        long seed = 1023321012;
        // The dungeon size.
        int dungeon_size = 256;
        // Number of floors placed.
        int floors_placed = 0;
        // Size of a single floor tile.
        int floor_size = 8;
        // Max floors that can be placed.
        int max_floors = 1500;
        // Current direction for the floor placement.
        int dir = 1;
        // Random starting x position.
        int rand_x = 0;
        // Random starting y position.
        int rand_y = 0;
        // Lower-bound position.
        Vector2 lower_bound;
        // Upper-bound position.
        Vector2 upper_bound;
        // Current x position.
        int current_x = 0;
        // Current y position.
        int current_y = 0;
        // Flag for whether or not the dungeon is generating.
        bool is_generating = false;
    };

private:
    // Image of the map.
    Ref<Image> map_image;
    // Texture for the map.
    Ref<ImageTexture> map_texture;
    // Flag for whether or not the dungeon is dirty.
    bool dirty = true;

    // Build the floors.
    void _build_floors();
    // Place a single floor.
    void _place_floor();

    // Check if we need to update the bounds.
    void _update_bounds();

public:
    // Dungeon generating parameters.
    DungeonParams params;

    // Constructor.
    DungeonMapBuilder();
    // Destructor.
    virtual ~DungeonMapBuilder();

    // Create the map image.
    void create_map_image();
    // Clears the map image.
    void clear_map_image();

    // Generates the map image.
    void generate_map_image();

    // Set the map image color for a specific tile.
    void set_map_tile_color(const Vector2& tile_id, Color color);

    // Returns the map image.
    _FORCE_INLINE_ Ref<Image> get_map_image() const { return map_image; }
    // Returns the map texture.
    _FORCE_INLINE_ Ref<ImageTexture> get_map_texture() const { return map_texture; }
};

#endif