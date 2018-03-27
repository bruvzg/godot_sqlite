extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var result = true
var data_vals = [[1, "test2"], [2, "test3"]]
func _assert(name, value, expected_value):
	result = result and (value == expected_value)
	printerr(name, " - returned: ", value, ", expected: ", expected_value, ", match: ", (value == expected_value))

func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()
	var stmt = SQLiteStatement.new()

	var status = db.open("tmp_db2456245")
	_assert("db.open", status, 0)

	var query = "SELECT * FROM test"
	status = db.prepare(query, stmt)
	_assert("db.prepare", status, 0)
	status = stmt.step()
	var row = 0
	while status == 100:
		var cnt = stmt.column_count()
		_assert("stmt.column_count", cnt, 2)
		for i in range(cnt):
			_assert("stmt.column_value", stmt.column_value(i), data_vals[row][i])
		row += 1
		status = stmt.step()
	_assert("row_count", row, 2)
	stmt.finalize()

	db.close()

	var dir = Directory.new()
	dir.remove("tmp_db2456245")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
