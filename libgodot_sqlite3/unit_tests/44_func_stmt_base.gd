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

func _nop():
	return false
	
func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()

	var dir = Directory.new()
	dir.remove("tmp_db2455558")

	var status = db.open("tmp_db2455558")
	_assert("db.open", status, 0)

	var stmt = SQLiteStatement.new()

	status = db.prepare("CREATE TABLE IF NOT EXISTS test1 (id integer PRIMARY KEY, name text NOT NULL)", stmt)
	_assert("db.prepare", status, 0)

	status = stmt.reset()
	_assert("stmt.reset", status, 0)

	status = stmt.reset()
	_assert("stmt.reset", status, 0)

	status = stmt.step()
	_assert("stmt.step", status, 101)

	status = stmt.data_count()
	_assert("stmt.data_count", status, 0)

	status = stmt.column_count()
	_assert("stmt.column_count", status, 0)

	var value = stmt.column_value(0)
	_assert("stmt.column_value", value, null)

	value = stmt.column_value(2320)
	_assert("stmt.column_value", value, null)

	value = stmt.column_name(0)
	_assert("stmt.column_name", value, null)

	value = stmt.column_name(2320)
	_assert("stmt.column_name", value, null)

	value = stmt.column_database_name(0)
	_assert("stmt.column_database_name", value, null)

	value = stmt.column_database_name(2320)
	_assert("stmt.column_database_name", value, null)

	value = stmt.column_table_name(0)
	_assert("stmt.column_table_name", value, null)

	value = stmt.column_table_name(2320)
	_assert("stmt.column_table_name", value, null)

	value = stmt.column_origin_name(0)
	_assert("stmt.column_origin_name", value, null)

	value = stmt.column_origin_name(2320)
	_assert("stmt.column_origin_name", value, null)

	value = stmt.column_decltype(0)
	_assert("stmt.column_decltype", value, null)

	value = stmt.column_decltype(2320)
	_assert("stmt.column_decltype", value, null)

	status = stmt.is_valid()
	_assert("stmt.is_valid", status, true)

	status = stmt.is_busy()
	_assert("stmt.is_valid", status, false)

	status = stmt.is_readonly()
	_assert("stmt.is_valid", status, false)

	status = stmt.bind_parameter_index(0)
	_assert("stmt.bind_parameter_index", status, 0)

	status = stmt.bind_parameter_index(null)
	_assert("stmt.bind_parameter_index", status, 0)

	status = stmt.bind_parameter_index("23423")
	_assert("stmt.bind_parameter_index", status, 0)

	value = stmt.bind_parameter_name("23423")
	_assert("stmt.bind_parameter_name", value, null)

	value = stmt.bind_parameter_name(0)
	_assert("stmt.bind_parameter_name", value, null)

	value = stmt.bind_parameter_name(23423)
	_assert("stmt.bind_parameter_name", value, null)

	status = stmt.finalize()
	_assert("stmt.finalize", status, 0)

	status = stmt.step() #fail
	_assert("stmt.step", status, 1)

	status = stmt.data_count()
	_assert("stmt.data_count", status, 0)

	status = stmt.column_count()
	_assert("stmt.column_count", status, 0)

	value = stmt.column_value(0)
	_assert("stmt.column_value", value, null)

	value = stmt.column_value(2320)
	_assert("stmt.column_value", value, null)

	value = stmt.column_name(0)
	_assert("stmt.column_name", value, null)

	value = stmt.column_name(2320)
	_assert("stmt.column_name", value, null)

	value = stmt.column_database_name(0)
	_assert("stmt.column_database_name", value, null)

	value = stmt.column_database_name(2320)
	_assert("stmt.column_database_name", value, null)

	value = stmt.column_table_name(0)
	_assert("stmt.column_table_name", value, null)

	value = stmt.column_table_name(2320)
	_assert("stmt.column_table_name", value, null)

	value = stmt.column_origin_name(0)
	_assert("stmt.column_origin_name", value, null)

	value = stmt.column_origin_name(2320)
	_assert("stmt.column_origin_name", value, null)

	value = stmt.column_decltype(0)
	_assert("stmt.column_decltype", value, null)

	value = stmt.column_decltype(2320)
	_assert("stmt.column_decltype", value, null)

	status = stmt.is_valid()
	_assert("stmt.is_valid", status, false)

	status = stmt.is_busy()
	_assert("stmt.is_valid", status, false)

	status = stmt.is_readonly()
	_assert("stmt.is_valid", status, false)

	status = stmt.bind_parameter_index(0)
	_assert("stmt.bind_parameter_index", status, -1)

	status = stmt.bind_parameter_index(null)
	_assert("stmt.bind_parameter_index", status, -1)

	status = stmt.bind_parameter_index("23423")
	_assert("stmt.bind_parameter_index", status, -1)

	value = stmt.bind_parameter_name("23423")
	_assert("stmt.bind_parameter_name", value, null)

	value = stmt.bind_parameter_name(0)
	_assert("stmt.bind_parameter_name", value, null)

	value = stmt.bind_parameter_name(23423)
	_assert("stmt.bind_parameter_name", value, null)

	db.queue_close()
	db.close()

	dir.remove("tmp_db2455558")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
