extends MeshInstance

onready var level_map = get_parent().get_node('GUI/LevelGenMapUI')

func _ready():
	level_map.connect("level_image_generation_complete", self, "_on_start_mesh_generation")

func _on_start_mesh_generation():
	get_parent().get_node("GUI/GameStatusLabel").set_text("Finished")	
	
