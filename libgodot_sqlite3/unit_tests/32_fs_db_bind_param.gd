extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var result = true
var data_vals = [[1, "text_string"]]
func _assert(name, value, expected_value):
	result = result and (value == expected_value)
	printerr(name, " - returned: ", value, ", expected: ", expected_value, ", match: ", (value == expected_value))

func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()
	var stmt = SQLiteStatement.new()

	var dir = Directory.new()
	dir.remove("tmp_db2455555")

	var status = db.open("tmp_db2455555")
	_assert("db.open", status, 0)

	var query = "CREATE TABLE IF NOT EXISTS test ("
	query += "id integer PRIMARY KEY,"
	query += "name text NOT NULL"
	query += ")"

	status = db.prepare(query, stmt)
	_assert("db.prepare(create)", status, 0)
	stmt.step()
	stmt.finalize()

	query = "INSERT INTO test (name) VALUES (:KEK)"
	status = db.prepare(query, stmt)
	_assert("db.prepare(insert)", status, 0)

	var parcnt = stmt.bind_parameter_count()
	_assert("stmt.bind_parameter_count", parcnt, 1)

	var name = stmt.bind_parameter_name(1)
	_assert("stmt.bind_parameter_name", name, ":KEK")

	stmt.bind_parameter_value(1, "text_string")
	status = stmt.step()
	_assert("stmt.step", status, 101)
	stmt.finalize()

	query = "SELECT * FROM test"
	status = db.prepare(query, stmt)
	_assert("db.prepare(select)", status, 0)
	status = stmt.step()
	var row = 0
	while status == 100:
		var cnt = stmt.column_count()
		_assert("stmt.column_count", cnt, 2)
		for i in range(cnt):
			_assert("stmt.column_value", stmt.column_value(i), data_vals[row][i])
		row += 1
		status = stmt.step()
	_assert("row_count", row, 1)
	stmt.finalize()

	db.close()
	dir.remove("tmp_db2455555")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
