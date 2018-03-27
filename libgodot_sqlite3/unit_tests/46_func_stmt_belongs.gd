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
	dir.remove("tmp_db2455559")

	var status = db.open("tmp_db2455559")
	_assert("db.open", status, 0)

	var stmt = SQLiteStatement.new()

	status = db.prepare("CREATE TABLE IF NOT EXISTS test1 (id integer PRIMARY KEY, name text NOT NULL)", stmt)
	_assert("db.prepare", status, 0)

	status = stmt.belongs_to(db)
	_assert("stmt.belongs_to", status, true)

	status = stmt.belongs_to(self)
	_assertn("stmt.belongs_to", status, true)

	status = stmt.belongs_to("34534534")
	_assertn("stmt.belongs_to", status, true)

	status = stmt.belongs_to(null)
	_assertn("stmt.belongs_to", status, true)

	status = stmt.belongs_to(0)
	_assertn("stmt.belongs_to", status, true)

	status = stmt.finalize()
	_assert("stmt.finalize", status, 0)

	status = stmt.step() #fail
	_assert("stmt.step", status, 1)

	status = stmt.belongs_to(db)
	_assertn("stmt.belongs_to", status, true)

	status = stmt.belongs_to(self)
	_assertn("stmt.belongs_to", status, true)

	status = stmt.belongs_to("34534534")
	_assertn("stmt.belongs_to", status, true)

	status = stmt.belongs_to(null)
	_assertn("stmt.belongs_to", status, true)

	status = stmt.belongs_to(0)
	_assertn("stmt.belongs_to", status, true)

	db.queue_close()
	db.close()

	dir.remove("tmp_db2455559")

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
