extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var data_vals = [["2", "test3"], ["1", "test2"]]
var data_names = ["id", "name"]

var result = true
var row = 0
func _assert(name, value, expected_value):
	result = result and (value == expected_value)
	printerr(name, " - returned: ", value, ", expected: ", expected_value, ", match: ", (value == expected_value))

func _iteration(delta):
	return true

func test_collat_func(string_a, string_b):
	_assert("collate a", string_a, "test2")
	_assert("collate b", string_b, "test3")
	return 51

func exec_func(col_count, names, values):
	for i in range(col_count):
		_assert("exec func value", values[i], data_vals[row][i])
		_assert("exec func value", names[i], data_names[i])
	row += 1
	return 0 #continue

func _initialize():
	var db = SQLite.new()
	var stmt = SQLiteStatement.new()

	var dir = Directory.new()
	dir.remove("user://_temp_test.db")

	var status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_READWRITE | SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
	_assert("db.open", status, 0)

	status = db.create_collation("test_collat", self, "test_collat_func")
	_assert("db.create_collation", status, 0)

	var query = "CREATE TABLE IF NOT EXISTS test ("
	query += "id integer PRIMARY KEY,"
	query += "name text NOT NULL COLLATE test_collat"
	query += ")"

	status = db.prepare(query, stmt)
	_assert("db.prepare (create)", status, 0)
	stmt.step()
	stmt.finalize()

	query = "INSERT INTO test (name) VALUES ('test2')"
	status = db.prepare(query, stmt)
	_assert("db.prepare (insert)", status, 0)
	stmt.step()
	stmt.finalize()

	query = "INSERT INTO test (name) VALUES ('test3')"
	status = db.prepare(query, stmt)
	_assert("db.prepare (insert)", status, 0)
	stmt.step()
	stmt.finalize()

	var errmsg = ""
	status = db.exec("SELECT * FROM test ORDER BY Name", self, "exec_func", errmsg)
	_assert("row count", row, 2)
	_assert("db.exec", status, 0)
	_assert("db.exec errormsg", errmsg, "")

	status = db.destroy_collation("test_collat")
	_assert("db.destroy_collation", status, 0)

	#fail - no callation
	status = db.exec("SELECT * FROM test ORDER BY Name", self, "exec_func", errmsg)
	_assert("db.exec", status, 1)
	_assert("db.exec errormsg", errmsg, "no such collation sequence: test_collat")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
