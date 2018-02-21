extends Navigation

onready var dungeon_map = get_parent().get_node("DungeonMap")
onready var level_gen_ui = get_node("../GUI/LevelGenMapUI")

func _ready():
	dungeon_map.add_navigation_meshes(self)
	level_gen_ui.connect("level_image_generating", self, "_generating_map")
	level_gen_ui.connect("level_image_generation_complete", self, "_generate_map_complete")

func _generating_map():
	dungeon_map.remove_navigation_meshes(self)

func _generate_map_complete():
	dungeon_map.add_navigation_meshes(self)