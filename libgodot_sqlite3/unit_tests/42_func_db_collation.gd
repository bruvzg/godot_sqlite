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
	dir.remove("tmp_db2455555")

	var status = db.create_collation(null, null, null)
	_assertn("db.create_collation", status, 0)

	status = db.create_collation(null, null)
	_assertn("db.create_collation", status, 0)

	status = db.create_collation(null)
	_assertn("db.create_collation", status, 0)

	status = db.create_collation()
	_assertn("db.create_collation", status, 0)

	status = db.create_collation("eqrgqerg34g", "43f34g43g", "43g34g34g43")
	_assertn("db.create_collation", status, 0)

	status = db.create_collation(db, db, db)
	_assertn("db.create_collation", status, 0)

	status = db.create_collation(0, 0, 0)
	_assertn("db.create_collation", status, 0)

	status = db.create_collation(0, self, "_nop")
	_assertn("db.create_collation", status, 0)

	status = db.destroy_collation(null)
	_assertn("db.destroy_collation", status, 0)

	status = db.destroy_collation()
	_assertn("db.destroy_collation", status, 0)

	status = db.destroy_collation("eqrgqerg34g")
	_assertn("db.destroy_collation", status, 0)

	status = db.destroy_collation(db)
	_assertn("db.destroy_collation", status, 0)

	status = db.destroy_collation(0)
	_assertn("db.destroy_collation", status, 0)

	status = db.destroy_collation(self)
	_assertn("db.destroy_collation", status, 0)

	status = db.open("tmp_db2455555")
	_assert("db.open", status, 0)

	status = db.create_collation(null, null, null)
	_assertn("db.create_collation", status, 0)

	status = db.create_collation(null, null)
	_assertn("db.create_collation", status, 0)

	status = db.create_collation(null)
	_assertn("db.create_collation", status, 0)

	status = db.create_collation()
	_assertn("db.create_collation", status, 0)

	status = db.create_collation("eqrgqerg34g", "43f34g43g", "43g34g34g43")
	_assertn("db.create_collation", status, 0)

	#ok -> db is valid object and can be converted to string
	status = db.create_collation(db, db, db)
	_assert("db.create_collation", status, 0)

	status = db.create_collation(0, 0, 0)
	_assertn("db.create_collation", status, 0)

	#ok -> valid func
	status = db.create_collation(0, self, "_nop")
	_assert("db.create_collation", status, 0)

	db.queue_close()
	db.close()

	dir.remove("tmp_db2455555")

	status = db.open("tmp_db2455557")
	_assert("db.open", status, 0)
	
	status = db.destroy_collation(null)
	_assert("db.destroy_collation", status, 0)

	status = db.destroy_collation()
	_assertn("db.destroy_collation", status, 0)

	status = db.destroy_collation("eqrgqerg34g")
	_assert("db.destroy_collation", status, 0)

	status = db.destroy_collation(db)
	_assert("db.destroy_collation", status, 0)

	status = db.destroy_collation(0)
	_assert("db.destroy_collation", status, 0)

	status = db.destroy_collation(self)
	_assert("db.destroy_collation", status, 0)

	db.queue_close()
	db.close()

	dir.remove("tmp_db2455557")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
