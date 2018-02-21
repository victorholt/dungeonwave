extends Spatial

var capture_mouse = true

func _ready():
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	
func _process(delta):
	if Input.is_action_just_pressed("ui_cancel"):
		if capture_mouse:
			Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			capture_mouse = false
		else:
			Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
			capture_mouse = true
		# get_tree().quit()
	if Input.is_action_just_pressed("restart"):
		get_tree().reload_current_scene()
