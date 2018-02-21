extends TextureRect

signal level_image_generating
signal level_image_generation_complete

onready var dungeon_map = get_parent().get_parent().get_node("DungeonMap")
onready var generate_button = get_parent().get_node("GenerateLevelBtn")
onready var player = get_parent().get_parent().get_node("Player")
onready var location_label = get_parent().get_node("LocationLabel")

var is_generating = false
var last_pos = Vector2()

func _ready():
	set_texture(dungeon_map.get_map_texture())
	generate_button.connect("pressed", self, "_generate_map")
	
	# Update the label with the seed.
	get_parent().get_node("GameStatusLabel").set_text("Seed: " + String(dungeon_map.get_dungeon_seed()))
	
func _process(delta):
	update_position()
	
func update_position():
	if !player:
		return
	
	var new_pos = Vector2(floor(player.translation.x), floor(player.translation.z))
	var map_image = dungeon_map.get_map_image()
	
	var snap_pos = dungeon_map.get_tile_position(new_pos)
	snap_pos.y *= -1
	location_label.text = "Location: " + String(snap_pos)
	
	# Update our map
	if new_pos != last_pos:		
		if dungeon_map.is_valid_position(last_pos):
			dungeon_map.set_map_tile_color(dungeon_map.get_tile_position(last_pos), Color(1,1,1))
		
		if dungeon_map.is_valid_position(new_pos):
			dungeon_map.set_map_tile_color(dungeon_map.get_tile_position(new_pos), Color(1,0,0))
	last_pos = new_pos
	
func _generate_map():
	if is_generating:
		return
		
	# Send signal that we're generating.
	emit_signal("level_image_generating")
		
	# Reset the focus.
	generate_button.set_focus_mode(Control.FOCUS_NONE)
		
	# Update the seed.
	dungeon_map.set_dungeon_seed(randi() % 2000000000)
	get_parent().get_node("GameStatusLabel").set_text("Seed: " + String(dungeon_map.get_dungeon_seed()))
		
	# Update generating flag and reset the map.
	is_generating = true
	
	# Update the status label.
	dungeon_map.mark_dirty()
	dungeon_map.generate_map_image()
	dungeon_map.apply()
	
	is_generating = false
	
	# Update the map texture.
	set_texture(dungeon_map.get_map_texture())
		
	# Set the new player location.
	if (player):
		var rand_map_loc = dungeon_map.get_random_map_location()
		player.translation = Vector3(rand_map_loc.x, 0, rand_map_loc.y)
		player.translation.y += 0.05
	
	# Emit our signal to generate the level mesh.
	emit_signal("level_image_generation_complete")

