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
	dir.remove("tmp_db2456245")

	var status = db.open("tmp_db2456245")
	_assert("db.open", status, 0)

	var query = "CREATE TABLE IF NOT EXISTS test ("
	query += "id integer PRIMARY KEY,"
	query += "name text NOT NULL"
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

	db.close()

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
