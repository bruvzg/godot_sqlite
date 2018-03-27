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
	var stmt = SQLiteStatement.new()
	var dir = Directory.new()

	var status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_READWRITE | SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
	_assert("db.open", status, 0)

	var query = "DROP TABLE IF EXISTS test"
	status = db.prepare(query, stmt)
	_assert("db.prepare (drop)", status, 0)
	stmt.step()
	stmt.finalize()

	db.close()

	dir.remove("user://_temp_test.db")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
