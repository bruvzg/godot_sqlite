extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var result = true
var data_vals = [
	[1, "text1", "text1.2", 1.2],
	[2, "text2", null, 5.0],
	[3, "text3", null, null]]

func _assert(name, value, expected_value):
	result = result and (value == expected_value)
	printerr(name, " - returned: ", value, ", expected: ", expected_value, ", match: ", (value == expected_value))

func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()
	var stmt = SQLiteStatement.new()
	var dir = Directory.new()
	dir.remove("user://_temp_test.db")

	var status = db.open("user://_temp_test.db", SQLiteConstants.SQLITE_OPEN_READWRITE | SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
	_assert("db.open", status, 0)

	var query = "CREATE TABLE IF NOT EXISTS test ("
	query += "id integer PRIMARY KEY,"
	query += "name text NOT NULL,"
	query += "name2 text,"
	query += "real real"
	query += ")"

	status = db.prepare(query, stmt)
	_assert("db.prepare (create)", status, 0)
	stmt.step()
	stmt.finalize()

	query = "INSERT INTO test (name, name2, real) VALUES ('text1', 'text1.2', 1.2)"
	status = db.prepare(query, stmt)
	_assert("db.prepare (insert)", status, 0)
	stmt.step()
	stmt.finalize()

	query = "INSERT INTO test (name, real) VALUES ('text2', 5.0)"
	status = db.prepare(query, stmt)
	_assert("db.prepare (insert)", status, 0)
	stmt.step()
	stmt.finalize()

	query = "INSERT INTO test (name) VALUES ('text3')"
	status = db.prepare(query, stmt)
	_assert("db.prepare (insert)", status, 0)
	stmt.step()
	stmt.finalize()

	query = "SELECT * FROM test"
	status = db.prepare(query, stmt)
	_assert("db.prepare (select)", status, 0)
	status = stmt.step()
	var row = 0
	while status == 100:
		var cnt = stmt.column_count()
		_assert("stmt.column_count", cnt, 4)
		for i in range(cnt):
			_assert("stmt.column_value", stmt.column_value(i), data_vals[row][i])
		row += 1
		status = stmt.step()
	_assert("row_count", row, 3)
	stmt.finalize()

	db.close()

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
