/**
 * Copyright (c) 2018 GameSencha, LLC
 * @author  Victor Holt
 * @date    2/17/2018
 */
#include "register_types.h"
#include "dungeon_map.h"
#include "dungeon_map_debug_renderer.h"
#ifndef _3D_DISABLED
#include "class_db.h"
#endif

void register_dungeonmap_types()
{
#ifndef _3D_DISABLED
	ClassDB::register_class<DungeonMap>();
	ClassDB::register_class<DungeonMapDebugRenderer>();
#ifdef TOOLS_ENABLED
#endif
#endif
}

void unregister_dungeonmap_types()
{

}