extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var result = true
var data_vals = [
	[1, "text1", "text1.2", 1.2, PoolByteArray([0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C])],
	[2, "text2", null, 5.0, null],
	[3, "text3", null, null, null]]
var data_names = ["id", "name", "name2", "real", "blob"]
var data_types = ["INTEGER", "TEXT", "TEXT", "REAL", "BLOB"]

func _assert(name, value, expected_value):
	result = result and (value == expected_value)
	printerr(name, " - returned: ", value, ", expected: ", expected_value, ", match: ", (value == expected_value))

func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()
	var stmt = SQLiteStatement.new()

	var status = db.open("res://unit_tests/data/temp.db", SQLiteConstants.SQLITE_OPEN_READONLY, "godot_vfs")
	_assert("db.open", status, 0)

	var query = "SELECT * FROM test"
	status = db.prepare(query, stmt)
	_assert("db.prepare", status, 0)
	status = stmt.step()
	var row = 0
	while status == 100:
		var cnt = stmt.column_count()
		_assert("stmt.column_count", cnt, 5)
		for i in range(cnt):
			_assert("stmt.column_value", stmt.column_value(i), data_vals[row][i])
			_assert("stmt.column_name", stmt.column_name(i), data_names[i])
			_assert("stmt.column_database_name", stmt.column_database_name(i), "main")
			_assert("stmt.column_table_name", stmt.column_table_name(i), "test")
			_assert("stmt.column_origin_name", stmt.column_origin_name(i), data_names[i])
			_assert("stmt.column_decltype", stmt.column_decltype(i), data_types[i])
		row += 1
		status = stmt.step()
	_assert("row_count", row, 3)
	stmt.finalize()

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
