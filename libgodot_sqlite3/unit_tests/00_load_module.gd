extends MainLoop

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

var result = true

func _iteration(delta):
	return true

func _initialize():
	var db = SQLite.new()

	printerr(db.version())
	printerr(db.sourceid())
	printerr(db.errstr(0))
	printerr(db.errstr(100))
	printerr(db.errstr(2333423))
	printerr(db.errstr(-1232))
	printerr(db.errstr("asddedf"))

	if result:
		OS.exit_code = 10  #PASS
	else:
		OS.exit_code = 100 #FAIL
