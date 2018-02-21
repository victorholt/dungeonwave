extends RigidBody

var has_target = false
var health = 100
var dead = false

func _ready():
	pass
		
func _physics_process(delta):
	_check_hit()
	_patrol(delta)
				
func _check_hit():
	if dead:
		return
		
	if contacts_reported > 0:
		var bodies = get_colliding_bodies()
		for c in bodies:
			if c.has_meta("projectile"):
				# Free up the projectile.
				c.queue_free()
				
				# Not the best way to handle this....
				hide()
				translation = Vector3(-9999,-9999,-9999)				
				dead = true
				return

func _patrol(delta):
	if dead:
		return
	
	var navigation = get_node("../../Navigation")
	var player = get_node("../../Player")
	var path_to_player = PoolVector3Array()
	
	if player and translation.distance_to(player.translation) > 30:
		return
	
	if navigation and player:
		path_to_player = navigation.get_simple_path(translation, player.translation, false)
		if path_to_player.size() == 0:
			return
		has_target = true
		
	if !has_target:
		return
		
	var next_pos = Vector3()
	next_pos = translation.linear_interpolate(path_to_player[path_to_player.size() - 1], 0.5 * delta)
	translation = next_pos
	translation.y = 0.51
	
#func _projectile_hit_event(projectile, body):
#	print("I am checking hit event")
#	if body == self:
#		print("I've been hit with " + String(projectile.damage) + " damage")
		
	
	
