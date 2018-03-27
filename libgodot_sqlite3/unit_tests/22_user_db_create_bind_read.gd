extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var result = true
var data_vals = [[1, "text1", "text1.2", 1.2, PoolByteArray([0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C])]]

func _assert(name, value, expected_value):
	result = result and (value == expected_value)
	printerr(name, " - returned: ", value, ", expected: ", expected_value, ", match: ", (value == expected_value))

func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()
	var stmt = SQLiteStatement.new()
	var dir = Directory.new()
	dir.remove("user://_temp_test3344.db")

	var status = db.open("user://_temp_test3344.db", SQLiteConstants.SQLITE_OPEN_READWRITE | SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
	_assert("db.open", status, 0)

	var query = "CREATE TABLE IF NOT EXISTS test ("
	query += "id integer PRIMARY KEY,"
	query += "name text NOT NULL,"
	query += "name2 text,"
	query += "real real,"
	query += "blob blob"
	query += ")"

	status = db.prepare(query, stmt)
	_assert("db.prepare (create)", status, 0)
	stmt.step()
	stmt.finalize()

	query = "INSERT INTO test (id, name, name2, real, blob) VALUES (:A, :B, :C, :D, :E)"
	status = db.prepare(query, stmt)
	_assert("db.prepare (insert)", status, 0)

	var parcnt = stmt.bind_parameter_count()
	_assert("stmt.bind_parameter_count", parcnt, 5)

	var name = stmt.bind_parameter_name(1)
	_assert("stmt.bind_parameter_name", name, ":A")
	name = stmt.bind_parameter_name(2)
	_assert("stmt.bind_parameter_name", name, ":B")
	name = stmt.bind_parameter_name(3)
	_assert("stmt.bind_parameter_name", name, ":C")
	name = stmt.bind_parameter_name(4)
	_assert("stmt.bind_parameter_name", name, ":D")
	name = stmt.bind_parameter_name(5)
	_assert("stmt.bind_parameter_name", name, ":E")

	status = stmt.bind_parameter_value(1, data_vals[0][0])
	_assert("bind_parameter_value(1)", status, 0)
	status = stmt.bind_parameter_value(2, data_vals[0][1])
	_assert("bind_parameter_value(2)", status, 0)
	status = stmt.bind_parameter_value(3, data_vals[0][2])
	_assert("bind_parameter_value(3)", status, 0)
	status = stmt.bind_parameter_value(4, data_vals[0][3])
	_assert("bind_parameter_value(4)", status, 0)
	status = stmt.bind_parameter_value(5, data_vals[0][4])
	_assert("bind_parameter_value(5)", status, 0)

	status = stmt.step()
	_assert("stmt.step", status, 101)
	stmt.finalize()


	query = "SELECT * FROM test"
	status = db.prepare(query, stmt)
	_assert("db.prepare (select)", status, 0)
	status = stmt.step()
	var row = 0
	while status == 100:
		var cnt = stmt.column_count()
		_assert("stmt.column_count", cnt, 5)
		for i in range(cnt):
			_assert("stmt.column_value", stmt.column_value(i), data_vals[row][i])
		row += 1
		status = stmt.step()
	_assert("row_count", row, 1)
	stmt.finalize()

	db.close()

	dir.remove("user://_temp_test3344.db")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
