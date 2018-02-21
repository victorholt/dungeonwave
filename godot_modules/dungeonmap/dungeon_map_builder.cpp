#include "dungeon_map_builder.h"

#include <print_string.h>
#include <servers/physics_server.h>

void DungeonMapBuilder::generate_map_image()
{
    Math::seed(params.seed);

    // Reset our params.
    params.floors_placed = 0;
    params.lower_bound = Vector2(0, 0);
    params.upper_bound = Vector2(0, 0);

    // Clear the image.
    if (!map_image.is_valid()) {
        create_map_image();
    } else {
        clear_map_image();
    }

    _build_floors();

    // Update our map texture.
    map_texture->set_data(map_image);
}

void DungeonMapBuilder::create_map_image()
{
    if (map_image.is_valid()) {
        map_image.unref();
    }
    if (map_texture.is_valid()) {
        map_texture.unref();
    }

    Ref<Image> img = memnew(Image(
        params.dungeon_size,
        params.dungeon_size,
        false,
        Image::FORMAT_RGB8
    ));

    map_image = img;

    Ref<ImageTexture> tex = memnew(ImageTexture);
    tex->create_from_image(map_image);
    map_texture = tex;
}

void DungeonMapBuilder::clear_map_image()
{
    if (!map_image.is_valid()) {
        return;
    }

    int w = map_image->get_width();
    int h = map_image->get_height();

    map_image->lock();
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            map_image->set_pixel(x, y, Color(0, 0, 0));
        }
    }
    map_image->unlock();

    // Update the texture.
    map_texture->set_data(map_image);
}

void DungeonMapBuilder::set_map_tile_color(const Vector2& tile_id, Color color)
{
    map_image->lock();
    for (int x = 0; x < params.floor_size; x++) {
        for (int y = 0; y < params.floor_size; y++) {
            map_image->set_pixel((int)tile_id.x + x, (int)(tile_id.y * -1.0f) + y, color);
        }
    }
    map_image->unlock();
    map_texture->set_data(map_image);
}

void DungeonMapBuilder::_build_floors()
{
    if (params.is_generating) {
        return;
    }
    params.is_generating = true;

    int w = map_image->get_width();
    int h = map_image->get_height();

    // Set the initial position.
    //Math::randomize();
    params.rand_x = Math::random(0, params.dungeon_size - params.floor_size);
    params.rand_y = Math::random(0, params.dungeon_size - params.floor_size);

    params.current_x = params.rand_x - (params.rand_x % params.floor_size);
    params.current_y = params.rand_y - (params.rand_y % params.floor_size);
    params.lower_bound.x = params.current_x;
    params.upper_bound.y = params.current_y;

    // Update our bounds so that the initial start is what
    // we'd expect it to be.
    _update_bounds();

    // Create our floors for the dungeon.
    while (params.floors_placed < params.max_floors) {
        _place_floor();
    }

    // Update the status to let us know that we're finished.
    params.is_generating = false;
}

void DungeonMapBuilder::_place_floor()
{
    // Add our floor tile to the image.
    map_image->lock();
    for (int x = 0; x < params.floor_size; x++) {
        for (int y = 0; y < params.floor_size; y++) {
            map_image->set_pixel(params.current_x + x, params.current_y + y, Color(1, 1, 1));
        }
    }
    map_image->unlock();

    // Increment the number of floors.
    params.floors_placed += 1;

    // Determine the next location for the floor tile.
    params.dir = Math::random(0, 5);
    switch (params.dir) {
        case 1:
            params.current_x += params.floor_size;
            break;
        case 2:
            params.current_y += params.floor_size;
            break;
        case 3:
            params.current_x -= params.floor_size;
            break;
        case 4:
            params.current_y -= params.floor_size;
            break;
    }

    _update_bounds();
}

void DungeonMapBuilder::_update_bounds()
{
    // Check if we are outside of the room
    if (params.current_x < params.floor_size)
        params.current_x = params.floor_size;
    if (params.current_x > int(params.dungeon_size) - (params.floor_size * 2))
        params.current_x = int(params.dungeon_size) - (params.floor_size * 2);

    if (params.current_y < params.floor_size)
        params.current_y = params.floor_size;
    if (params.current_y > int(params.dungeon_size) - (params.floor_size * 2))
        params.current_y = int(params.dungeon_size) - (params.floor_size * 2);

    // Store the top-left and bottom-rigth corners of the map.
    if (params.current_x < params.lower_bound.x)
        params.lower_bound.x = params.current_x;
    if (params.current_x > params.upper_bound.x)
        params.upper_bound.x = params.current_x;
    if (params.current_y < params.lower_bound.y)
        params.lower_bound.y = params.current_y;
    if (params.current_y > params.upper_bound.y)
        params.upper_bound.y = params.current_y;
}

DungeonMapBuilder::DungeonMapBuilder()
{
    //set_notify_transform(true);
}

DungeonMapBuilder::~DungeonMapBuilder()
{
    // clear();
}