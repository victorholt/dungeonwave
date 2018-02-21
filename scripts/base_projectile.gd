extends RigidBody

# class member variables go here, for example:
var timer = 0
var damage = 10

func _ready():
	set_meta("projectile", true)

func _physics_process(delta):
	# Check if we've collided.
	#if contacts_reported > 0:
	#	var bodies = get_colliding_bodies()
		#for c in bodies:
		#	emit_signal("projectile_hit_event", self, c)
	
	# Check if we need to delete the projectile.
	timer += delta
	if timer > 500:
		timer = 0
		queue_free()
	timer += 1
