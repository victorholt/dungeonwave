extends Node

# Dictionary of all enemies spawned.
var npc_dictionary = Dictionary()
onready var npc_enemy_prefab = preload("res://prefabs/EnemyNPC.scn")
onready var dungeon_map = get_parent().get_node("DungeonMap")
onready var level_gen_ui = get_node("../GUI/LevelGenMapUI")


func _ready():
	level_gen_ui.connect("level_image_generation_complete", self, "_generate_map_complete")
	_load_enemies()
	
func _generate_map_complete():
	_load_enemies()
	
func _load_enemies():
	if npc_dictionary.size() > 0:
		for i in range(20):
			npc_dictionary[i].queue_free()
	npc_dictionary.clear()
	
	for i in range(20):
		var rand_loc = dungeon_map.get_random_map_location()
		var npc_instance = npc_enemy_prefab.instance()
		npc_instance.translation = Vector3(rand_loc.x, 0.51, rand_loc.y)
		add_child(npc_instance)
		npc_dictionary[i] = npc_instance