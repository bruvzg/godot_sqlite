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

	var dir = Directory.new()
	dir.remove("tmp_db2455555")

	var status = db.prepare(null, null, null)
	_assertn("db.prepare", status, 0)

	status = db.prepare(null, null)
	_assertn("db.prepare", status, 0)

	status = db.prepare(null)
	_assertn("db.prepare", status, 0)

	status = db.prepare()
	_assertn("db.prepare", status, 0)

	status = db.prepare("eqrgqerg34g", "43f34g43g", "43g34g34g43")
	_assertn("db.prepare", status, 0)

	status = db.prepare(db, db, db)
	_assertn("db.prepare", status, 0)

	status = db.prepare(0, 0, 0)
	_assertn("db.prepare", status, 0)

	var stmt = SQLiteStatement.new()

	status = db.prepare(null, stmt, null)
	_assertn("db.prepare", status, 0)

	status = db.prepare(null, stmt)
	_assertn("db.prepare", status, 0)

	status = db.prepare(stmt)
	_assertn("db.prepare", status, 0)

	status = db.prepare("eqrgqerg34g", stmt, "43g34g34g43")
	_assertn("db.prepare", status, 0)

	status = db.prepare(db, stmt, db)
	_assertn("db.prepare", status, 0)

	status = db.prepare(0, stmt, 0)
	_assertn("db.prepare", status, 0)

	status = db.open("tmp_db2455555")
	_assert("db.open", status, 0)

	status = db.prepare(null, null, null)
	_assertn("db.prepare", status, 0)

	status = db.prepare(null, null)
	_assertn("db.prepare", status, 0)

	status = db.prepare(null)
	_assertn("db.prepare", status, 0)

	status = db.prepare()
	_assertn("db.prepare", status, 0)

	status = db.prepare("eqrgqerg34g", "43f34g43g", "43g34g34g43")
	_assertn("db.prepare", status, 0)

	status = db.prepare(db, db, db)
	_assertn("db.prepare", status, 0)

	status = db.prepare(0, 0, 0)
	_assertn("db.prepare", status, 0)

	status = db.prepare(null, stmt, null)
	_assertn("db.prepare", status, 0)

	status = db.prepare(null, stmt)
	_assertn("db.prepare", status, 0)

	status = db.prepare(stmt)
	_assertn("db.prepare", status, 0)

	status = db.prepare("eqrgqerg34g", stmt, "43g34g34g43")
	_assertn("db.prepare", status, 0)

	status = db.prepare(db, stmt, db)
	_assertn("db.prepare", status, 0)

	status = db.prepare(0, stmt, 0)
	_assertn("db.prepare", status, 0)

	db.queue_close()
	db.close()

	dir.remove("tmp_db2455555")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
