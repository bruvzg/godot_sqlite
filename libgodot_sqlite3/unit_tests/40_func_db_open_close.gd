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

	var status = db.open(null, 0, null)
	db.queue_close()
	db.close()
	_assertn("db.open", status, 0)

	status = db.open("1g3443g4g4g4", 0, "5134t34gr3434")
	db.queue_close()
	db.close()
	_assertn("db.open", status, 0)

	status = db.open(null, "54y45y45", "5134t34gr3434")
	db.queue_close()
	db.close()
	_assertn("db.open", status, 0)

	status = db.open(null, null)
	db.queue_close()
	db.close()
	_assertn("db.open", status, 0)

	status = db.open()
	db.queue_close()
	db.close()
	_assertn("db.open", status, 0)

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
