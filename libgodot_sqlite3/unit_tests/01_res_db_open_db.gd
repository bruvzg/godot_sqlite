extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var result = true
func _assert(name, value, expected_value):
	result = result and (value == expected_value)
	printerr(name, " - returned: ", value, ", expected: ", expected_value, ", match: ", (value == expected_value))

func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()
	var status = 0

	#open db ro -> ok
	status = db.open("res://unit_tests/data/temp.db", SQLiteConstants.SQLITE_OPEN_READONLY, "godot_vfs")
	_assert("db.open", status, 0)
	status = db.readonly()
	_assert("db.readonly", status, 1)
	db.close()

	#open db rw -> ok, open ro
	status = db.open("res://unit_tests/data/temp.db", SQLiteConstants.SQLITE_OPEN_READWRITE, "godot_vfs")
	_assert("db.open", status, 0)
	status = db.readonly()
	_assert("db.readonly", status, 1)
	db.close()

	#open db rwc -> ok, open ro
	status = db.open("res://unit_tests/data/temp.db", SQLiteConstants.SQLITE_OPEN_READWRITE | SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
	_assert("db.open", status, 0)
	status = db.readonly()
	_assert("db.readonly", status, 1)
	db.close()

	#invalid access mode -> fail
	status = db.open("res://unit_tests/data/temp.db", SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
	_assert("db.open", status, 21) #fail with 21(misuse)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#invalid access mode -> fail
	status = db.open("res://unit_tests/data/temp.db", 4564768, "godot_vfs")
	_assert("db.open", status, 21) #fail with 21(misuse)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()
	
	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
