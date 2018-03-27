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

func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()
	var status = 0

	var dir = Directory.new()
	dir.remove("temp_db_file_61232525521")

	#open db ro -> missing file
	status = db.open("temp_db_file_61232525521", SQLiteConstants.SQLITE_OPEN_READONLY)
	_assertn("db.open", status, 0) #fail with 12(notfound) or 14(cantopen)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#open db rw -> missing file
	status = db.open("temp_db_file_61232525521", SQLiteConstants.SQLITE_OPEN_READWRITE)
	_assertn("db.open", status, 0) #fail with 12(notfound) or 14(cantopen)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#open db rwc -> ok, create and open rw
	status = db.open("temp_db_file_61232525521", SQLiteConstants.SQLITE_OPEN_READWRITE | SQLiteConstants.SQLITE_OPEN_CREATE)
	_assert("db.open", status, 0)
	status = db.readonly()
	_assert("db.readonly", status, 0)
	db.close()

	#invalid access mode should fail
	status = db.open("temp_db_file_61232525521", SQLiteConstants.SQLITE_OPEN_CREATE)
	_assert("db.open", status, 21) #fail with 21(misuse)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#invalid access mode should fail
	status = db.open("temp_db_file_61232525521", 4564768)
	_assert("db.open", status, 21) #fail with 21(misuse)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#reopen db ro -> ok, open ro
	status = db.open("temp_db_file_61232525521", SQLiteConstants.SQLITE_OPEN_READONLY)
	_assert("db.open", status, 0)
	status = db.readonly()
	_assert("db.readonly", status, 1)
	db.close()

	#reopen db rw -> ok, open rw
	status = db.open("temp_db_file_61232525521", SQLiteConstants.SQLITE_OPEN_READWRITE)
	_assert("db.open", status, 0)
	status = db.readonly()
	_assert("db.readonly", status, 0)
	db.close()

	#invalid access mode should fail
	status = db.open("temp_db_file_61232525521", SQLiteConstants.SQLITE_OPEN_CREATE)
	_assert("db.open", status, 21) #fail with 21(misuse)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	#invalid access mode should fail
	status = db.open("temp_db_file_61232525521", 4564768)
	_assert("db.open", status, 21) #fail with 21(misuse)
	status = db.readonly()
	_assert("db.readonly", status, -1)
	db.close()

	dir.remove("temp_db_file_61232525521")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
