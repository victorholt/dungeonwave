extends KinematicBody

# Camera variables
var camera_angle = 0
var mouse_sensitivity = 0.3

# Movement variables
var velocity = Vector3()
var direction = Vector3()

# Fly constants
const FLY_SPEED = 40
const FLY_ACCEL = 4

# Walk variables/constants
var gravity = -9.8 * 3
const MAX_SPEED = 8
const MAX_RUNNING_SPEED = 20
const ACCEL = 2
const DEACCEL = 20

# Jumping variables/constants
var jump_height = 15

onready var dungeon_map = get_parent().get_node("DungeonMap")

func _ready():
	# Set the new player location.
	var rand_map_loc = dungeon_map.get_random_map_location()
	translation = Vector3(rand_map_loc.x, 0, rand_map_loc.y)
	translation.y += 0.05
	
func _physics_process(delta):
	# Ensure we don't process any input while the mouse
	# is not captured.
	if !get_parent().capture_mouse:
		return
		
	walk(delta)
	# fly(delta)
		
	fire(delta)

func _input(event):
	# Ensure we don't process any input while the mouse
	# is not captured.
	if !get_parent().capture_mouse:
		return
		
	if event is InputEventMouseMotion:
		$Head.rotate_y(deg2rad(-event.relative.x * mouse_sensitivity))
		
		# How far we've moved up/down
		var change = -event.relative.y * mouse_sensitivity
		if (change + camera_angle) < 90 && (change + camera_angle) > -90:
			$Head/Camera.rotate_x(deg2rad(change))
			camera_angle += change
			
func fly(delta):
	# Reset our direction for the player.
	direction = Vector3()
	
	# Get the rotation of the camera.
	var aim = $Head/Camera.get_global_transform().basis
	
	# Handle player movement.
	if Input.is_action_pressed("move_forward"):
		direction -= aim.z
	if Input.is_action_pressed("move_backward"):
		direction += aim.z
	if Input.is_action_pressed("move_left"):
		direction -= aim.x
	if Input.is_action_pressed("move_right"):
		direction += aim.x
	
	# Normalize our direction so that we move the same speed no matter
	# the direction we want to go.
	direction = direction.normalized()
	
	# How far will we go at max speed.
	var target = direction * FLY_SPEED
	
	# Accel to the max speed. Calculate a portion of the distance to go.
	velocity = velocity.linear_interpolate(target, FLY_ACCEL * delta)
	
	# Move our player.
	move_and_slide(velocity)
	
func walk(delta):
	# Reset our direction for the player.
	direction = Vector3()
	
	# Get the rotation of the camera.
	var aim = $Head/Camera.get_global_transform().basis
	
	# Handle player movement.
	if Input.is_action_pressed("move_forward"):
		direction -= aim.z
	if Input.is_action_pressed("move_backward"):
		direction += aim.z
	if Input.is_action_pressed("move_left"):
		direction -= aim.x
	if Input.is_action_pressed("move_right"):
		direction += aim.x
	
	# Normalize our direction so that we move the same speed no matter
	# the direction we want to go.
	direction = direction.normalized()
	
	# Apply our gravity.
	velocity.y += gravity * delta
	
	var temp_velocity = velocity
	temp_velocity.y = 0
	
	# Check if we should run/walk.
	var speed = MAX_SPEED
	if Input.is_action_pressed("move_sprint"):
		speed = MAX_RUNNING_SPEED
	
	# How far will we go at max speed.
	var target = direction * speed
	
	var acceleration = DEACCEL
	# If two vectors are going in the same general direction,
	# their dot product will be positive.
	if direction.dot(temp_velocity) > 0:
		acceleration = ACCEL
	
	# Accel to the max speed. Calculate a portion of the distance to go.
	temp_velocity = temp_velocity.linear_interpolate(target, acceleration * delta)
	velocity.x = temp_velocity.x
	velocity.z = temp_velocity.z
	
	# Move our player.
	velocity = move_and_slide(velocity, Vector3(0, 1, 0))
	
	if Input.is_action_just_pressed("jump"):
		velocity.y = jump_height
		
func fire(delta):
	if !Input.is_action_just_pressed("fire"):
		return
	
	# Create a projectile based on our current weapon.
	var projectile = preload("res://prefabs/base_projectile.tscn").instance()
	projectile.set_transform($Head/Camera.get_global_transform())	
	get_parent().add_child(projectile)
	
	var velocity = $Head/Camera.get_global_transform().basis[2].normalized() * Vector3(-1,-1,-1)	
	projectile.set_linear_velocity(velocity * 20)
	projectile.add_collision_exception_with(self)