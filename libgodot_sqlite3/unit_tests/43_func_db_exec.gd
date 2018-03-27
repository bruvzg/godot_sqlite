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

	var status = db.exec(null, null, null)
	_assertn("db.exec", status, 0)

	status = db.exec(null, null)
	_assertn("db.exec", status, 0)

	status = db.exec(null)
	_assertn("db.exec", status, 0)

	status = db.exec()
	_assertn("db.exec", status, 0)

	status = db.exec("eqrgqerg34g", "43f34g43g", "43g34g34g43")
	_assertn("db.exec", status, 0)

	status = db.exec(db, db, db)
	_assertn("db.exec", status, 0)

	status = db.exec(0, 0, 0)
	_assertn("db.exec", status, 0)

	status = db.exec(0, self, "_nop")
	_assertn("db.exec", status, 0)

	status = db.open("tmp_db2455558")
	_assert("db.open", status, 0)

	status = db.exec(null, null, null)
	_assertn("db.exec", status, 0)

	status = db.exec(null, null)
	_assertn("db.exec", status, 0)

	status = db.exec(null)
	_assertn("db.exec", status, 0)

	status = db.exec()
	_assertn("db.exec", status, 0)

	status = db.exec("eqrgqerg34g", "43f34g43g", "43g34g34g43")
	_assertn("db.exec", status, 0)

	status = db.exec(db, db, db)
	_assertn("db.exec", status, 0)

	status = db.exec(0, 0, 0)
	_assertn("db.exec", status, 0)

	status = db.exec(0, self, "_nop")
	_assertn("db.exec", status, 0)

	db.queue_close()
	db.close()

	dir.remove("tmp_db2455558")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
