extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var result = true
var data_vals = [
	["1", "text1", "text1.2", "1.2", ""],
	["2", "text2", null, "5.0", null],
	["3", "text3", null, null, null]]
var data_names = ["id", "name", "name2", "real", "blob"]
var row = 0

func _assert(name, value, expected_value):
	result = result and (value == expected_value)
	printerr(name, " - returned: ", value, ", expected: ", expected_value, ", match: ", (value == expected_value))

func _iteration(delta):
	return true

func exec_func(col_count, names, values):
	_assert("col count", col_count, 5)
	for i in range(col_count):
		_assert("exec value", values[i], data_vals[row][i])
		_assert("exec name", names[i], data_names[i])
	row += 1
	return 0 #continue

func _initialize():
	var db = SQLite.new()

	var status = db.open("res://unit_tests/data/temp.db", SQLiteConstants.SQLITE_OPEN_READONLY, "godot_vfs")
	_assert("db.open", status, 0)

	var errmsg = ""
	status = db.exec("SELECT * FROM test", self, "exec_func", errmsg)
	_assert("row count", row, 3)
	_assert("db.exec", status, 0)
	_assert("db.exec errormsg", errmsg, "")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
