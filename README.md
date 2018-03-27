# SQLite3 GDNative wrapper module for Godot

**IMPORTANT: This is work in progress, and should be considered "pre-alpha", there is no warranty that newer versions will maintain backward compatibility.**

## Requirements:
* Godot 3.1 or Godot 3.0.2 (With some limitations)

## Building requirements:
* C compiler
* Meson build system

## Example:
```
extends Control

const SQLite = preload("res://SQLite.gdns")
const SQLiteStatement = preload("res://SQLiteStatement.gdns")
const SQLiteConstants = preload("res://SQLiteConstants.gd")

func exec_func(col_count, names, values):
	print("User #", values[0], ": ", values[1])
	return 0 #continue

func _ready():
	var db = SQLite.new()
	db.open("user://users.db", SQLiteConstants.SQLITE_OPEN_READWRITE | SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")

	var stmt = SQLiteStatement.new()
	db.prepare("CREATE TABLE IF NOT EXISTS users (id integer PRIMARY KEY, username text NOT NULL)", stmt)
	stmt.step()
	stmt.finalize()

	db.prepare("INSERT INTO users (username) VALUES ('TestUser11')", stmt)
	stmt.step()
	stmt.finalize()

	db.prepare("INSERT INTO users (username) VALUES ('TestUser22')", stmt)
	stmt.step()
	stmt.finalize()

	db.exec("SELECT * FROM users", self, "exec_func")

	db.close()
```

## Additional features:
Access to Godot `res://` (exported .pck or project folder, read-only access) and `user://` (read-write access) VFS.
```
	db_res.open("res://persist_data.db", SQLiteConstants.SQLITE_OPEN_READONLY, "godot_vfs")
	db_usr.open("user://user_data.db", SQLiteConstants.SQLITE_OPEN_READWRITE | SQLiteConstants.SQLITE_OPEN_CREATE, "godot_vfs")
```

## Compiling:
You can compile this module by executing:
```
meson ./libgodot_sqlite3/bin
ninja -C ./libgodot_sqlite3/bin
```

## Automated testing:
You can run tests by executing:
```
./run_tests.py --godot-executable={PATH_TO_GODOT}
```

## Deploying:
Extract [release](https://github.com/bruvzg/godot_sqlite/releases) archive into your project directory.

## License:
The source code for the module is released under unlicense (see `LICENSE` file).
