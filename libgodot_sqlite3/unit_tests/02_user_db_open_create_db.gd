extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var result = true
func _assert(name, value, expected_value):
	result = result and (value == expected_value)
	printerr(name, " - returned: ", value, ", expected: ", expected_value, ", match: ", (value == expected_value))

func _assertn(name, value, expected_value):
	result = result and (value != expected_value)
	printerr(name, " - returned: ", value, ", expected: NOT ", expected_value, ", match: ", (value != expected_value))
	
func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()
	var status = 0

	var dir = Directory.new()
	dir.remove("user://_temp_test.db")

	#open db ro -> missing file
	status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_READONLY, "godot_vfs")
	_assertn("db.open", status, 0) #fail with 12(notfound) or 14(cantopen)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#open db rw -> missing file
	status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_READWRITE, "godot_vfs")
	_assertn("db.open", status, 0) #fail with 12(notfound) or 14(cantopen)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#open db rwc -> ok, create and open rw
	status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_READWRITE | SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
	_assert("db.open", status, 0)
	status = db.readonly()
	_assert("db.readonly", status, 0)
	db.close()

	#invalid access mode should fail
	status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
	_assert("db.open", status, 21) #fail with 21(misuse)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#invalid access mode should fail
	status = db.open("user://_temp_test.db", 4564768, "godot_vfs")
	_assert("db.open", status, 21) #fail with 21(misuse)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#reopen db ro -> ok, open ro
	status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_READONLY, "godot_vfs")
	_assert("db.open", status, 0)
	status = db.readonly()
	_assert("db.readonly", status, 1)
	db.close()

	#reopen db rw -> ok, open rw
	status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_READWRITE, "godot_vfs")
	_assert("db.open", status, 0)
	status = db.readonly()
	_assert("db.readonly", status, 0)
	db.close()

	#invalid access mode should fail
	status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
	_assert("db.open", status, 21)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#invalid access mode should fail
	status = db.open("user://_temp_test.db", 4564768, "godot_vfs")
	_assert("db.open", status, 21)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	dir.remove("user://_temp_test.db")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
