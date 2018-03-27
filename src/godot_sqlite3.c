/*************************************************************************/
/*  godot_sqlite3.c                                                      */
/*************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "gdnative_api_struct.gen.h"
#include "sqlite3.h"

typedef GDCALLINGCONV godot_variant (*gdn_method)(godot_object *, void *, void *, int, godot_variant **);
typedef GDCALLINGCONV void *(*gdn_create_func)(godot_object *, void *);
typedef GDCALLINGCONV void (*gdn_destroy_func)(godot_object *, void *, void *);
typedef GDCALLINGCONV void (*gdn_set_func)(godot_object *, void *, void *, godot_variant *);
typedef GDCALLINGCONV godot_variant (*gdn_get_func)(godot_object *, void *, void *);

typedef enum {
	GDN_SQLITE3_TYPE,
	GDN_SQLITE3_STMT_TYPE
} gdn_type;

typedef struct {
	gdn_type tag;
} gdn_type_tag;

typedef struct {
	sqlite3 *db;
} gdn_sqlite3_data;

typedef struct {
	sqlite3_stmt *stmt;
} gdn_sqlite3_stmt_data;

typedef struct {
	godot_object *owner;
	godot_object *compare_callback_host;
	godot_string name;
} gdn_sqlite3_callback_data;

typedef struct {
	sqlite3_file base;
	godot_object *file_object;
} gdn_vfs_sqlite3_file;

const godot_gdnative_core_api_struct *gdn_core_api = NULL;
const godot_gdnative_ext_nativescript_api_struct *gdn_nativescript_api = NULL;
const godot_gdnative_ext_nativescript_1_1_api_struct *gdn_nativescript_1_1_api = NULL;
const godot_gdnative_ext_pluginscript_api_struct *gdn_pluginscript_api = NULL;
const godot_gdnative_ext_arvr_api_struct *gdn_arvr_api = NULL;

#define DEBUG_PRINT_WARNING(m_text) gdn_core_api->godot_print_warning(m_text, __FUNCTION__, __FILE__, __LINE__);
#define DEBUG_PRINT_ERROR(m_text) gdn_core_api->godot_print_error(m_text, __FUNCTION__, __FILE__, __LINE__);

#ifndef _ALWAYS_INLINE_
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#elif defined(__llvm__)
#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define _ALWAYS_INLINE_ __forceinline
#else
#define _ALWAYS_INLINE_ inline
#endif
#endif

/*************************************************************************/
/*   Godot Object Methods                                                */
/*************************************************************************/

godot_method_bind *method_bind_object_callv = NULL;

/*************************************************************************/
/*   Godot OS Methods                                                    */
/*************************************************************************/

godot_method_bind *method_bind_os_delay_msec = NULL;
godot_method_bind *method_bind_os_get_unix_time = NULL;

/*************************************************************************/
/*   Godot Directory Methods                                             */
/*************************************************************************/

godot_method_bind *method_bind_dir_remove = NULL;
godot_method_bind *method_bind_dir_file_exists = NULL;
godot_method_bind *method_bind_dir_dir_exists = NULL;

/*************************************************************************/
/*   Godot File Methods                                                  */
/*************************************************************************/

godot_method_bind *method_bind_file_open = NULL;
godot_method_bind *method_bind_file_close = NULL;
godot_method_bind *method_bind_file_get_path = NULL;
godot_method_bind *method_bind_file_get_path_absolute = NULL;
godot_method_bind *method_bind_file_is_open = NULL;
godot_method_bind *method_bind_file_seek = NULL;
godot_method_bind *method_bind_file_seek_end = NULL;
godot_method_bind *method_bind_file_get_position = NULL;
godot_method_bind *method_bind_file_get_len = NULL;
godot_method_bind *method_bind_file_eof_reached = NULL;
godot_method_bind *method_bind_file_get_buffer = NULL;
godot_method_bind *method_bind_file_get_error = NULL;
godot_method_bind *method_bind_file_store_buffer = NULL;

/*************************************************************************/
/*   Helpers: Variant Constructors                                       */
/*************************************************************************/

_ALWAYS_INLINE_ void godot_variant_chars_to_utf8(godot_variant *p_result, const char *p_string) {

	godot_string value;
	gdn_core_api->godot_string_new(&value);
	gdn_core_api->godot_string_parse_utf8(&value, p_string);
	gdn_core_api->godot_variant_new_string(p_result, &value);
	gdn_core_api->godot_string_destroy(&value);
};

_ALWAYS_INLINE_ void godot_variant_chars_to_utf8_with_len(godot_variant *p_result, const char *p_string, godot_int p_len) {

	godot_string value;
	gdn_core_api->godot_string_new(&value);
	gdn_core_api->godot_string_parse_utf8_with_len(&value, p_string, p_len);
	gdn_core_api->godot_variant_new_string(p_result, &value);
	gdn_core_api->godot_string_destroy(&value);
};

_ALWAYS_INLINE_ godot_char_string godot_variant_as_char_string(godot_variant *p_varinat) {

	godot_char_string ret;
	godot_string gds = gdn_core_api->godot_variant_as_string(p_varinat);
	ret = gdn_core_api->godot_string_utf8(&gds);
	gdn_core_api->godot_string_destroy(&gds);

	return ret;
};

/*************************************************************************/
/*  GodotVFS                                                             */
/*************************************************************************/

int godot_vfs_file_close(sqlite3_file *p_file) {

	gdn_vfs_sqlite3_file *file_data = (gdn_vfs_sqlite3_file *)p_file;
	godot_variant_call_error call_error;
	const godot_variant *c_args[] = {};
	gdn_core_api->godot_method_bind_call(method_bind_file_close, file_data->file_object, c_args, 0, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_File:close call failed!");
	}
	gdn_core_api->godot_object_destroy(file_data->file_object);
	return SQLITE_OK;
};

int godot_vfs_file_get_position(sqlite3_file *p_file) {

	gdn_vfs_sqlite3_file *file_data = (gdn_vfs_sqlite3_file *)p_file;
	godot_variant_call_error call_error;
	const godot_variant *c_args[] = {};
	godot_variant return_buffer = gdn_core_api->godot_method_bind_call(method_bind_file_get_position, file_data->file_object, c_args, 0, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_File:get_position call failed!");
		gdn_core_api->godot_variant_destroy(&return_buffer);
		return 0;
	}
	int offset = gdn_core_api->godot_variant_as_int(&return_buffer);
	gdn_core_api->godot_variant_destroy(&return_buffer);
	return offset;
};

int godot_vfs_file_read(sqlite3_file *p_file, void *p_buffer, int p_size, sqlite3_int64 p_offset) {

	gdn_vfs_sqlite3_file *file_data = (gdn_vfs_sqlite3_file *)p_file;
	godot_variant_call_error call_error;
	godot_variant offset;
	gdn_core_api->godot_variant_new_int(&offset, p_offset);
	const godot_variant *seek_args[] = {
		&offset
	};
	gdn_core_api->godot_method_bind_call(method_bind_file_seek, file_data->file_object, seek_args, 1, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_File:seek call failed!");
		gdn_core_api->godot_variant_destroy(&offset);
		return SQLITE_IOERR_READ;
	}
	gdn_core_api->godot_variant_destroy(&offset);
	if (godot_vfs_file_get_position(p_file) != p_offset) {
		DEBUG_PRINT_ERROR("offset mismatch!");
		return SQLITE_IOERR_READ;
	}
	godot_variant size;
	gdn_core_api->godot_variant_new_int(&size, p_size);
	const godot_variant *read_args[] = {
		&size
	};
	godot_variant return_buffer = gdn_core_api->godot_method_bind_call(method_bind_file_get_buffer, file_data->file_object, read_args, 1, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_File:get_buffer call failed!");
		gdn_core_api->godot_variant_destroy(&size);
		gdn_core_api->godot_variant_destroy(&return_buffer);
		return SQLITE_IOERR_READ;
	}
	gdn_core_api->godot_variant_destroy(&size);
	godot_pool_byte_array read_buffer = gdn_core_api->godot_variant_as_pool_byte_array(&return_buffer);
	godot_pool_byte_array_read_access *read_buffer_access = gdn_core_api->godot_pool_byte_array_read(&read_buffer);
	memcpy(p_buffer, gdn_core_api->godot_pool_byte_array_read_access_ptr(read_buffer_access), p_size);
	gdn_core_api->godot_pool_byte_array_read_access_destroy(read_buffer_access);
	gdn_core_api->godot_pool_byte_array_destroy(&read_buffer);
	gdn_core_api->godot_variant_destroy(&return_buffer);

	return SQLITE_OK;
};

int godot_vfs_file_write(sqlite3_file *p_file, const void *p_buffer, int p_size, sqlite3_int64 p_offset) {

	gdn_vfs_sqlite3_file *file_data = (gdn_vfs_sqlite3_file *)p_file;
	godot_variant_call_error call_error;
	godot_variant offset;
	gdn_core_api->godot_variant_new_int(&offset, p_offset);
	const godot_variant *seek_args[] = {
		&offset
	};
	gdn_core_api->godot_method_bind_call(method_bind_file_seek, file_data->file_object, seek_args, 1, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_File:seek call failed!");
		gdn_core_api->godot_variant_destroy(&offset);
		return SQLITE_IOERR_WRITE;
	}
	gdn_core_api->godot_variant_destroy(&offset);
	if (godot_vfs_file_get_position(p_file) != p_offset) {
		DEBUG_PRINT_ERROR("offset mismatch!");
		return SQLITE_IOERR_WRITE;
	}
	godot_pool_byte_array write_buffer;
	gdn_core_api->godot_pool_byte_array_new(&write_buffer);
	gdn_core_api->godot_pool_byte_array_resize(&write_buffer, p_size);
	godot_pool_byte_array_write_access *write_buffer_access = gdn_core_api->godot_pool_byte_array_write(&write_buffer);
	memcpy(gdn_core_api->godot_pool_byte_array_write_access_ptr(write_buffer_access), p_buffer, p_size);
	godot_variant buffer;
	gdn_core_api->godot_variant_new_pool_byte_array(&buffer, &write_buffer);
	const godot_variant *write_args[] = {
		&buffer
	};
	gdn_core_api->godot_method_bind_call(method_bind_file_store_buffer, file_data->file_object, write_args, 1, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_File:store_buffer call failed!");
		gdn_core_api->godot_variant_destroy(&buffer);
		gdn_core_api->godot_pool_byte_array_write_access_destroy(write_buffer_access);
		gdn_core_api->godot_pool_byte_array_destroy(&write_buffer);
		return SQLITE_IOERR_WRITE;
	}
	gdn_core_api->godot_variant_destroy(&buffer);
	gdn_core_api->godot_pool_byte_array_write_access_destroy(write_buffer_access);
	gdn_core_api->godot_pool_byte_array_destroy(&write_buffer);
	return SQLITE_OK;
};

int godot_vfs_file_truncate(sqlite3_file *p_file, sqlite3_int64 p_size) {

	DEBUG_PRINT_ERROR("Truncate is not supported!");
	return SQLITE_IOERR_TRUNCATE;
};

int godot_vfs_file_sync(sqlite3_file *p_file, int p_flags) {

	//DO NOTHING
	return SQLITE_OK;
};

int godot_vfs_file_filesize(sqlite3_file *p_file, sqlite3_int64 *p_size) {

	gdn_vfs_sqlite3_file *file_data = (gdn_vfs_sqlite3_file *)p_file;
	godot_variant_call_error call_error;
	const godot_variant *c_args[] = {};
	godot_variant size = gdn_core_api->godot_method_bind_call(method_bind_file_get_len, file_data->file_object, c_args, 0, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_File:get_len call failed!");
		gdn_core_api->godot_variant_destroy(&size);
		return SQLITE_IOERR_FSTAT;
	}
	*p_size = gdn_core_api->godot_variant_as_int(&size);
	gdn_core_api->godot_variant_destroy(&size);
	return SQLITE_OK;
};

int godot_vfs_file_lock(sqlite3_file *p_file, int p_lock) {

	//DO NOTHING
	return SQLITE_OK;
};

int godot_vfs_file_unlock(sqlite3_file *p_file, int p_lock) {

	//DO NOTHING
	return SQLITE_OK;
};

int godot_vfs_file_check_reserved_lock(sqlite3_file *p_file, int *p_resout) {

	//DO NOTHING
	*p_resout = 0;
	return SQLITE_OK;
};

int godot_vfs_file_file_control(sqlite3_file *p_file, int p_op, void *p_arg) {

	//DO NOTHING
	return SQLITE_OK;
};

int godot_vfs_file_sector_size(sqlite3_file *p_file) {

	//DO NOTHING
	return 0;
};

int godot_vfs_file_device_characteristics(sqlite3_file *p_file) {

	//DO NOTHING
	return 0;
};

static sqlite3_io_methods godot_vfs_io = {
	.iVersion = 1,
	.xClose = godot_vfs_file_close,
	.xRead = godot_vfs_file_read,
	.xWrite = godot_vfs_file_write,
	.xTruncate = godot_vfs_file_truncate,
	.xSync = godot_vfs_file_sync,
	.xFileSize = godot_vfs_file_filesize,
	.xLock = godot_vfs_file_lock,
	.xUnlock = godot_vfs_file_unlock,
	.xCheckReservedLock = godot_vfs_file_check_reserved_lock,
	.xFileControl = godot_vfs_file_file_control,
	.xSectorSize = godot_vfs_file_sector_size,
	.xDeviceCharacteristics = godot_vfs_file_device_characteristics,
	.xShmMap = NULL,
	.xShmLock = NULL,
	.xShmBarrier = NULL,
	.xShmUnmap = NULL,
	.xFetch = NULL,
	.xUnfetch = NULL
};

enum GodotVFSModeFlags {
	READ = 1,
	WRITE = 2,
	READ_WRITE = 3,
	WRITE_READ = 7,
};

int godot_vfs_open(sqlite3_vfs *p_vfs, const char *p_name, sqlite3_file *p_file, int p_flags, int *p_out_flags) {

	if ((p_flags & SQLITE_OPEN_DELETEONCLOSE) || !p_name) {
		DEBUG_PRINT_ERROR("Temporary DeleteOnClose files are not supported!");
		return SQLITE_IOERR;
	}
	godot_variant flags;
	godot_variant filename;
	godot_variant_chars_to_utf8(&filename, p_name);
	int oflags = 0;
	if (p_flags & SQLITE_OPEN_READONLY) {
		oflags |= READ;
		*p_out_flags = SQLITE_OPEN_READONLY;
	} else if (p_flags & SQLITE_OPEN_READWRITE) {
		if (strncmp("user://", p_name, 7) == 0) {
			if (p_flags & SQLITE_OPEN_CREATE) {
				godot_object *dir_object = gdn_core_api->godot_get_class_constructor("_Directory")();
				const godot_variant *c_args[] = {
					&filename,
				};
				godot_variant_call_error call_error;
				godot_variant return_val_file = gdn_core_api->godot_method_bind_call(method_bind_dir_file_exists, dir_object, c_args, 1, &call_error);
				if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
					DEBUG_PRINT_ERROR("_Directory:file_exists call failed!");
					gdn_core_api->godot_variant_destroy(&filename);
					gdn_core_api->godot_object_destroy(dir_object);
					return SQLITE_IOERR;
				}
				gdn_core_api->godot_object_destroy(dir_object);
				if (gdn_core_api->godot_variant_as_bool(&return_val_file)) {
					oflags |= READ_WRITE;
				} else {
					oflags |= WRITE_READ;
				}
				gdn_core_api->godot_variant_destroy(&return_val_file);
			} else {
				oflags |= READ_WRITE;
			}
		} else if (strncmp("res://", p_name, 6) == 0) {
			oflags |= READ;
			*p_out_flags = SQLITE_OPEN_READONLY;
			DEBUG_PRINT_WARNING("res://* files are read-only!");
		} else {
			gdn_core_api->godot_variant_destroy(&filename);
			DEBUG_PRINT_ERROR("Invalid file path!");
			return SQLITE_IOERR;
		}
	} else {
		gdn_core_api->godot_variant_destroy(&filename);
		DEBUG_PRINT_ERROR("Invalid access mode!");
		return SQLITE_IOERR;
	}
	gdn_vfs_sqlite3_file *file_data = (gdn_vfs_sqlite3_file *)p_file;
	file_data->base.pMethods = &godot_vfs_io;
	file_data->file_object = gdn_core_api->godot_get_class_constructor("_File")();
	gdn_core_api->godot_variant_new_int(&flags, oflags);
	const godot_variant *c_args[] = {
		&filename,
		&flags
	};
	godot_variant_call_error call_error;
	godot_variant return_val = gdn_core_api->godot_method_bind_call(method_bind_file_open, file_data->file_object, c_args, 2, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_File:open call failed!");
		gdn_core_api->godot_variant_destroy(&filename);
		gdn_core_api->godot_variant_destroy(&flags);
		return SQLITE_ERROR;
	}
	gdn_core_api->godot_variant_destroy(&filename);
	gdn_core_api->godot_variant_destroy(&flags);

	return gdn_core_api->godot_variant_as_int(&return_val);
};

int godot_vfs_delete(sqlite3_vfs *p_vfs, const char *p_name, int p_syncdir) {

	godot_object *dir_object = gdn_core_api->godot_get_class_constructor("_Directory")();
	godot_variant filename;
	godot_variant_chars_to_utf8(&filename, p_name);
	const godot_variant *c_args[] = {
		&filename,
	};
	godot_variant_call_error call_error;
	gdn_core_api->godot_method_bind_call(method_bind_dir_remove, dir_object, c_args, 1, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_Directory:remove call failed!");
		gdn_core_api->godot_variant_destroy(&filename);
		gdn_core_api->godot_object_destroy(dir_object);
		return SQLITE_IOERR;
	}
	gdn_core_api->godot_variant_destroy(&filename);
	gdn_core_api->godot_object_destroy(dir_object);
	return SQLITE_OK;
};

int godot_vfs_access(sqlite3_vfs *p_vfs, const char *p_name, int p_flags, int *p_resout) {

	if (p_flags == SQLITE_ACCESS_EXISTS) {
		godot_object *dir_object = gdn_core_api->godot_get_class_constructor("_Directory")();
		godot_variant filename;
		godot_variant_chars_to_utf8(&filename, p_name);
		const godot_variant *c_args[] = {
			&filename,
		};
		godot_variant_call_error call_error;
		godot_variant return_val_file = gdn_core_api->godot_method_bind_call(method_bind_dir_file_exists, dir_object, c_args, 1, &call_error);
		if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
			DEBUG_PRINT_ERROR("_Directory:file_exists call failed!");
			gdn_core_api->godot_variant_destroy(&return_val_file);
			gdn_core_api->godot_variant_destroy(&filename);
			gdn_core_api->godot_object_destroy(dir_object);
			return SQLITE_IOERR;
		}
		godot_variant return_val_dir = gdn_core_api->godot_method_bind_call(method_bind_dir_dir_exists, dir_object, c_args, 1, &call_error);
		if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
			DEBUG_PRINT_ERROR("_Directory:dir_exists call failed!");
			gdn_core_api->godot_variant_destroy(&return_val_file);
			gdn_core_api->godot_variant_destroy(&return_val_dir);
			gdn_core_api->godot_variant_destroy(&filename);
			gdn_core_api->godot_object_destroy(dir_object);
			return SQLITE_IOERR;
		}
		*p_resout = gdn_core_api->godot_variant_as_bool(&return_val_dir) || gdn_core_api->godot_variant_as_bool(&return_val_file);
		gdn_core_api->godot_variant_destroy(&return_val_file);
		gdn_core_api->godot_variant_destroy(&return_val_dir);
		gdn_core_api->godot_variant_destroy(&filename);
		gdn_core_api->godot_object_destroy(dir_object);
	} else if (p_flags == SQLITE_ACCESS_READ) {
		*p_resout = (strncmp("user://", p_name, 7) == 0) || (strncmp("res://", p_name, 6) == 0);
	} else if (p_flags == SQLITE_ACCESS_READWRITE) {
		*p_resout = (strncmp("user://", p_name, 7) == 0);
	} else {
		*p_resout = false;
	}
	return SQLITE_OK;
};

int godot_vfs_full_pathname(sqlite3_vfs *p_vfs, const char *p_name, int p_sizeout, char *p_bufout) {

	sqlite3_snprintf(p_sizeout, p_bufout, "%s", p_name);
	p_bufout[p_sizeout - 1] = '\0';
	return SQLITE_OK;
};

void *godot_vfs_full_dlopen(sqlite3_vfs *p_vfs, const char *p_path) {

	DEBUG_PRINT_ERROR("Loadable extensions are not supported!");
	return NULL;
};

void godot_vfs_full_dlerror(sqlite3_vfs *p_vfs, int p_size, char *p_errmsg) {

	sqlite3_snprintf(p_size, p_errmsg, "Loadable extensions are not supported!");
	p_errmsg[p_size - 1] = '\0';
};

void (*godot_vfs_full_dlsym(sqlite3_vfs *p_vfs, void *p_handle, const char *p_name))(void) {

	DEBUG_PRINT_ERROR("Loadable extensions are not supported!");
	return NULL;
}

void godot_vfs_full_dlclose(sqlite3_vfs *p_vfs, void *p_handle) {

	DEBUG_PRINT_ERROR("Loadable extensions are not supported!");
	return;
}

int godot_vfs_randomness(sqlite3_vfs *p_vfs, int p_size, char *p_bufout) {

	//DO NOTHING
	return 0;
};

int godot_vfs_sleep(sqlite3_vfs *p_vfs, int p_microseconds) {

	godot_object *os = gdn_core_api->godot_global_get_singleton("_OS");
	godot_variant time;
	gdn_core_api->godot_variant_new_int(&time, p_microseconds);
	const godot_variant *c_args[] = {
		&time
	};
	godot_variant_call_error call_error;
	gdn_core_api->godot_method_bind_call(method_bind_os_delay_msec, os, c_args, 1, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_OS:delay_msec failed!");
		gdn_core_api->godot_variant_destroy(&time);
		return 0;
	}
	gdn_core_api->godot_variant_destroy(&time);
	return p_microseconds;
};

int godot_vfs_current_time(sqlite3_vfs *p_vfs, double *p_time) {

	godot_object *os = gdn_core_api->godot_global_get_singleton("_OS");
	const godot_variant *c_args[] = {};
	godot_variant_call_error call_error;
	godot_variant return_val = gdn_core_api->godot_method_bind_call(method_bind_os_get_unix_time, os, c_args, 0, &call_error);
	if (call_error.error != GODOT_CALL_ERROR_CALL_OK) {
		DEBUG_PRINT_ERROR("_OS:get_unix_time call failed!");
		gdn_core_api->godot_variant_destroy(&return_val);
		return SQLITE_ERROR;
	}
	*p_time = gdn_core_api->godot_variant_as_int(&return_val) / 86400.0 + 2440587.5;
	gdn_core_api->godot_variant_destroy(&return_val);
	return SQLITE_OK;
};

static sqlite3_vfs godot_vfs = {

	.iVersion = 1,
	.szOsFile = sizeof(gdn_vfs_sqlite3_file),
	.mxPathname = 256,
	.pNext = NULL,
	.zName = "godot_vfs",
	.pAppData = NULL,
	.xOpen = godot_vfs_open,
	.xDelete = godot_vfs_delete,
	.xAccess = godot_vfs_access,
	.xFullPathname = godot_vfs_full_pathname,
	.xDlOpen = godot_vfs_full_dlopen,
	.xDlError = godot_vfs_full_dlerror,
	.xDlSym = godot_vfs_full_dlsym,
	.xDlClose = godot_vfs_full_dlclose,
	.xRandomness = godot_vfs_randomness,
	.xSleep = godot_vfs_sleep,
	.xCurrentTime = godot_vfs_current_time,
	.xGetLastError = NULL,
	.xCurrentTimeInt64 = NULL,
	.xSetSystemCall = NULL,
	.xGetSystemCall = NULL,
	.xNextSystemCall = NULL
};

void register_godot_vfs() {

	sqlite3_vfs_register(&godot_vfs, 0);
};

/*************************************************************************/
/*  SQLite Class                                                         */
/*************************************************************************/

static gdn_type_tag gdn_sqlite3_tag = { GDN_SQLITE3_TYPE };

void GDCALLINGCONV *gdn_sqlite3_init(godot_object *p_instance, void *p_method_data) {

	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)gdn_core_api->godot_alloc(sizeof(gdn_sqlite3_data));
	db_data->db = NULL;
	return db_data;
};

void GDCALLINGCONV gdn_sqlite3_terminate(godot_object *p_instance, void *p_method_data, void *p_user_data) {

	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (db_data->db) {
		while (sqlite3_close(db_data->db) == SQLITE_BUSY) {
			//WAIT
		};
		db_data->db = NULL;
	}
	gdn_core_api->godot_free(db_data);
};

godot_variant GDCALLINGCONV gdn_sqlite3_queue_close(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	sqlite3_close_v2(db_data->db);
	db_data->db = NULL;
	gdn_core_api->godot_variant_new_nil(&ret);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_close(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	while (sqlite3_close(db_data->db) == SQLITE_BUSY) {
		//WAIT
	};
	db_data->db = NULL;
	gdn_core_api->godot_variant_new_nil(&ret);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_open(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (p_num_args < 1) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	if (db_data->db) {
		while (sqlite3_close_v2(db_data->db) == SQLITE_BUSY) {
		};
		db_data->db = NULL;
	}
	godot_char_string filename_buf = godot_variant_as_char_string(p_args[0]);
	const char *filename = gdn_core_api->godot_char_string_get_data(&filename_buf);
	godot_int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	if (p_num_args >= 2) {
		flags = gdn_core_api->godot_variant_as_int(p_args[1]);
	}
	godot_char_string vfsname_buf;
	const char *vfsname = NULL;
	if (p_num_args >= 3) {
		vfsname_buf = godot_variant_as_char_string(p_args[2]);
		vfsname = gdn_core_api->godot_char_string_get_data(&vfsname_buf);
	}
	int status = sqlite3_open_v2(filename, &db_data->db, flags, vfsname);
	if (p_num_args >= 3) {
		gdn_core_api->godot_char_string_destroy(&vfsname_buf);
	}
	gdn_core_api->godot_char_string_destroy(&filename_buf);
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_prepare(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db || p_num_args < 2) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	godot_object *stmt_obj = gdn_core_api->godot_variant_as_object(p_args[1]);
	if (!stmt_obj) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	if (gdn_nativescript_1_1_api) {
		gdn_type_tag *stmt_tag = (gdn_type_tag *)gdn_nativescript_1_1_api->godot_nativescript_get_type_tag(stmt_obj);
		if (!stmt_tag || (stmt_tag->tag != GDN_SQLITE3_STMT_TYPE)) {
			DEBUG_PRINT_ERROR("Type mismatch, SQLiteStatement expected!");
			gdn_core_api->godot_variant_new_int(&ret, SQLITE_MISMATCH);
			return ret;
		}
	}
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)(gdn_nativescript_api->godot_nativescript_get_userdata(stmt_obj));
	if (!stmt_data) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	godot_char_string statement_buf = godot_variant_as_char_string(p_args[0]);
	const char *statement = gdn_core_api->godot_char_string_get_data(&statement_buf);
	const char *statement_tail = NULL;
	int status = sqlite3_prepare_v2(db_data->db, statement, -1, &stmt_data->stmt, &statement_tail);
	if (status != SQLITE_OK) {
		stmt_data->stmt = NULL;
	}
	if ((p_num_args >= 3) && (status == SQLITE_OK) && statement_tail) {
		godot_variant_chars_to_utf8(p_args[2], statement_tail);
	}
	gdn_core_api->godot_char_string_destroy(&statement_buf);
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

int gdn_sqlite3_collation_compare_callback(void *p_user_data, int p_size_a, const void *p_string_a, int p_size_b, const void *p_string_b) {

	gdn_sqlite3_callback_data *cb_data = (gdn_sqlite3_callback_data *)p_user_data;
	godot_variant value_a_var;
	godot_variant_chars_to_utf8_with_len(&value_a_var, p_string_a, p_size_a);
	godot_variant value_b_var;
	godot_variant_chars_to_utf8_with_len(&value_b_var, p_string_b, p_size_b);
	godot_array args;
	gdn_core_api->godot_array_new(&args);
	gdn_core_api->godot_array_append(&args, &value_a_var);
	gdn_core_api->godot_array_append(&args, &value_b_var);
	const void *c_args[] = {
		&cb_data->name,
		&args
	};
	godot_variant return_val;
	gdn_core_api->godot_method_bind_ptrcall(method_bind_object_callv, cb_data->compare_callback_host, c_args, &return_val);
	gdn_core_api->godot_array_destroy(&args);
	gdn_core_api->godot_variant_destroy(&value_b_var);
	gdn_core_api->godot_variant_destroy(&value_a_var);
	return gdn_core_api->godot_variant_as_int(&return_val);
};

void gdn_sqlite3_collation_destroy_callback(void *p_user_data) {

	gdn_sqlite3_callback_data *cb_data = (gdn_sqlite3_callback_data *)p_user_data;
	gdn_core_api->godot_string_destroy(&cb_data->name);
	gdn_core_api->godot_free(cb_data);
};

godot_variant GDCALLINGCONV gdn_sqlite3_create_collation(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db || p_num_args < 3) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	if (!gdn_core_api->godot_variant_as_object(p_args[1])) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	godot_char_string name_buf = godot_variant_as_char_string(p_args[0]);
	const char *name = gdn_core_api->godot_char_string_get_data(&name_buf);
	if (!name || (strlen(name) == 0)) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	godot_string func_name_gds = gdn_core_api->godot_variant_as_string(p_args[2]);
	gdn_sqlite3_callback_data *cb_data = (gdn_sqlite3_callback_data *)gdn_core_api->godot_alloc(sizeof(gdn_sqlite3_callback_data));
	cb_data->owner = p_instance;
	cb_data->compare_callback_host = gdn_core_api->godot_variant_as_object(p_args[1]);
	gdn_core_api->godot_string_new_copy(&cb_data->name, &func_name_gds);
	int status = sqlite3_create_collation_v2(db_data->db, name, SQLITE_UTF8, (void *)cb_data, &gdn_sqlite3_collation_compare_callback, &gdn_sqlite3_collation_destroy_callback);
	gdn_core_api->godot_char_string_destroy(&name_buf);
	gdn_core_api->godot_string_destroy(&func_name_gds);
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_destroy_collation(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db || p_num_args < 1) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	godot_char_string name_buf = godot_variant_as_char_string(p_args[0]);
	const char *name = gdn_core_api->godot_char_string_get_data(&name_buf);
	if (!name || (strlen(name) == 0)) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	int status = sqlite3_create_collation_v2(db_data->db, name, SQLITE_UTF8, NULL, NULL, NULL);
	gdn_core_api->godot_char_string_destroy(&name_buf);
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

int gdn_sqlite3_exec_callback(void *p_user_data, int p_col_num, char **p_vaules, char **p_names) {

	gdn_sqlite3_callback_data *cb_data = (gdn_sqlite3_callback_data *)p_user_data;
	godot_array names;
	godot_variant names_var;
	godot_array values;
	godot_variant values_var;
	gdn_core_api->godot_array_new(&names);
	gdn_core_api->godot_array_new(&values);
	godot_variant count_var;
	for (unsigned int i = 0; i < p_col_num; i++) {
		godot_variant value_var;
		if (p_vaules[i]) {
			godot_variant_chars_to_utf8(&value_var, p_vaules[i]);
		} else {
			gdn_core_api->godot_variant_new_nil(&value_var);
		}
		gdn_core_api->godot_array_append(&values, &value_var);
		gdn_core_api->godot_variant_destroy(&value_var);

		godot_variant name_var;
		if (p_names[i]) {
			godot_variant_chars_to_utf8(&name_var, p_names[i]);
		} else {
			gdn_core_api->godot_variant_new_nil(&name_var);
		}
		gdn_core_api->godot_array_append(&names, &name_var);
		gdn_core_api->godot_variant_destroy(&name_var);
	}
	gdn_core_api->godot_variant_new_int(&count_var, p_col_num);
	gdn_core_api->godot_variant_new_array(&names_var, &names);
	gdn_core_api->godot_variant_new_array(&values_var, &values);
	godot_array args;
	gdn_core_api->godot_array_new(&args);
	gdn_core_api->godot_array_append(&args, &count_var);
	gdn_core_api->godot_array_append(&args, &names_var);
	gdn_core_api->godot_array_append(&args, &values_var);
	const void *c_args[] = {
		&cb_data->name,
		&args
	};
	godot_variant return_val;
	gdn_core_api->godot_method_bind_ptrcall(method_bind_object_callv, cb_data->compare_callback_host, c_args, &return_val);
	gdn_core_api->godot_array_destroy(&args);
	gdn_core_api->godot_variant_destroy(&names_var);
	gdn_core_api->godot_array_destroy(&names);
	gdn_core_api->godot_variant_destroy(&values_var);
	gdn_core_api->godot_array_destroy(&values);
	gdn_core_api->godot_variant_destroy(&count_var);
	return gdn_core_api->godot_variant_as_int(&return_val);
};

godot_variant GDCALLINGCONV gdn_sqlite3_exec(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db || p_num_args < 3) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	if (!gdn_core_api->godot_variant_as_object(p_args[1])) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	godot_char_string sql_buf = godot_variant_as_char_string(p_args[0]);
	const char *sql = gdn_core_api->godot_char_string_get_data(&sql_buf);
	gdn_sqlite3_callback_data cb_data;
	cb_data.owner = p_instance;
	cb_data.compare_callback_host = gdn_core_api->godot_variant_as_object(p_args[1]);
	cb_data.name = gdn_core_api->godot_variant_as_string(p_args[2]);
	char *errormsg = NULL;
	int status = sqlite3_exec(db_data->db, sql, &gdn_sqlite3_exec_callback, &cb_data, &errormsg);
	if ((p_num_args >= 4) && (errormsg != NULL)) {
		godot_variant_chars_to_utf8(p_args[3], errormsg);
	}
	gdn_core_api->godot_char_string_destroy(&sql_buf);
	gdn_core_api->godot_string_destroy(&cb_data.name);
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_errcode(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	int errorcode = sqlite3_errcode(db_data->db);
	gdn_core_api->godot_variant_new_int(&ret, errorcode);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_errmsg(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	const char *errormsg = sqlite3_errmsg(db_data->db);
	if (!errormsg) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, errormsg);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_errstr(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	if (p_num_args < 1) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	const char *errormsg = sqlite3_errstr(gdn_core_api->godot_variant_as_int(p_args[0]));
	if (!errormsg) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, errormsg);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_version(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	const char *ver = sqlite3_libversion();
	if (!ver) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, ver);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_sourceid(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	const char *ver = sqlite3_sourceid();
	if (!ver) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, ver);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_changes(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_int(&ret, 0);
		return ret;
	}
	int changes = sqlite3_changes(db_data->db);
	gdn_core_api->godot_variant_new_int(&ret, changes);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_total_changes(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_int(&ret, 0);
		return ret;
	}
	int changes = sqlite3_total_changes(db_data->db);
	gdn_core_api->godot_variant_new_int(&ret, changes);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_release_memory(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	int status = sqlite3_db_release_memory(db_data->db);
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_get_last_insert_rowid(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	int id = sqlite3_last_insert_rowid(db_data->db);
	gdn_core_api->godot_variant_new_int(&ret, id);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_set_last_insert_rowid(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db || p_num_args < 1) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	sqlite3_set_last_insert_rowid(db_data->db, gdn_core_api->godot_variant_as_int(p_args[0]));
	gdn_core_api->godot_variant_new_nil(&ret);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_readonly(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_int(&ret, -1);
		return ret;
	}
	godot_char_string name_buf;
	const char *name = NULL;
	if (p_num_args >= 1) {
		name_buf = godot_variant_as_char_string(p_args[0]);
		name = gdn_core_api->godot_char_string_get_data(&name_buf);
	}
	gdn_core_api->godot_variant_new_int(&ret, sqlite3_db_readonly(db_data->db, name));
	if (p_num_args >= 1) {
		gdn_core_api->godot_char_string_destroy(&name_buf);
	}
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_filename(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_char_string name_buf;
	const char *name = NULL;
	if (p_num_args >= 1) {
		name_buf = godot_variant_as_char_string(p_args[0]);
		name = gdn_core_api->godot_char_string_get_data(&name_buf);
	}
	const char *filename = sqlite3_db_filename(db_data->db, name);
	if (p_num_args >= 1) {
		gdn_core_api->godot_char_string_destroy(&name_buf);
	}
	if (!filename) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, filename);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_config(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)p_user_data;
	if (!db_data->db || p_num_args < 1) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	int verb = gdn_core_api->godot_variant_as_int(p_args[0]);
	switch (verb) {
		case SQLITE_DBCONFIG_MAINDBNAME: {
			if (p_num_args < 2) {
				gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
				return ret;
			}
			godot_char_string name_buf = godot_variant_as_char_string(p_args[0]);
			const char *name = gdn_core_api->godot_char_string_get_data(&name_buf);
			int status = sqlite3_db_config(db_data->db, verb, name);
			gdn_core_api->godot_variant_new_int(&ret, status);
			gdn_core_api->godot_char_string_destroy(&name_buf);
		} break;
		case SQLITE_DBCONFIG_LOOKASIDE: {
			if (p_num_args < 3) {
				gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
				return ret;
			}
			int status = sqlite3_db_config(db_data->db, verb, NULL, gdn_core_api->godot_variant_as_int(p_args[1]), gdn_core_api->godot_variant_as_int(p_args[2]));
			gdn_core_api->godot_variant_new_int(&ret, status);
		} break;
		case SQLITE_DBCONFIG_ENABLE_FKEY:
		case SQLITE_DBCONFIG_ENABLE_TRIGGER:
		case SQLITE_DBCONFIG_ENABLE_FTS3_TOKENIZER:
		case SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION:
		case SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE:
		case SQLITE_DBCONFIG_ENABLE_QPSG:
		case SQLITE_DBCONFIG_TRIGGER_EQP: {
			if (p_num_args < 3) {
				gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
				return ret;
			}
			int status = sqlite3_db_config(db_data->db, verb, gdn_core_api->godot_variant_as_int(p_args[1]), gdn_core_api->godot_variant_as_int(p_args[2]));
			gdn_core_api->godot_variant_new_int(&ret, status);
		} break;
		default: {
			gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		}
	}
	return ret;
};

/*************************************************************************/
/*  SQLiteStatement Class                                                */
/*************************************************************************/

static gdn_type_tag gdn_sqlite3_stmt_tag = { GDN_SQLITE3_STMT_TYPE };

void GDCALLINGCONV *gdn_sqlite3_stmt_init(godot_object *p_instance, void *p_method_data) {

	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)gdn_core_api->godot_alloc(sizeof(gdn_sqlite3_stmt_data));
	stmt_data->stmt = NULL;
	return stmt_data;
};

void GDCALLINGCONV gdn_sqlite3_stmt_terminate(godot_object *p_instance, void *p_method_data, void *p_user_data) {

	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (stmt_data->stmt) {
		sqlite3_finalize(stmt_data->stmt);
		stmt_data->stmt = NULL;
	}
	gdn_core_api->godot_free(stmt_data);
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_step(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	int status = sqlite3_step(stmt_data->stmt);
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_finalize(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	int status = sqlite3_finalize(stmt_data->stmt);
	stmt_data->stmt = NULL;
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_reset(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	int status = sqlite3_reset(stmt_data->stmt);
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_data_count(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_int(&ret, 0);
		return ret;
	}
	int data_count = sqlite3_data_count(stmt_data->stmt);
	gdn_core_api->godot_variant_new_int(&ret, data_count);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_column_count(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_int(&ret, 0);
		return ret;
	}
	int column_count = sqlite3_column_count(stmt_data->stmt);
	gdn_core_api->godot_variant_new_int(&ret, column_count);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_column_value(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || (p_num_args < 1)) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	int index = gdn_core_api->godot_variant_as_int(p_args[0]);
	switch (sqlite3_column_type(stmt_data->stmt, index)) {
		case SQLITE_INTEGER: {
			gdn_core_api->godot_variant_new_int(&ret, sqlite3_column_int(stmt_data->stmt, index));
		} break;
		case SQLITE_FLOAT: {
			gdn_core_api->godot_variant_new_real(&ret, sqlite3_column_double(stmt_data->stmt, index));
		} break;
		case SQLITE_BLOB: {
			godot_pool_byte_array value;
			gdn_core_api->godot_pool_byte_array_new(&value);
			gdn_core_api->godot_pool_byte_array_resize(&value, sqlite3_column_bytes(stmt_data->stmt, index));
			godot_pool_byte_array_write_access *waccess = gdn_core_api->godot_pool_byte_array_write(&value);
			memcpy(gdn_core_api->godot_pool_byte_array_write_access_ptr(waccess), sqlite3_column_blob(stmt_data->stmt, index), sqlite3_column_bytes(stmt_data->stmt, index));
			gdn_core_api->godot_pool_byte_array_write_access_destroy(waccess);
			gdn_core_api->godot_variant_new_pool_byte_array(&ret, &value);
			gdn_core_api->godot_pool_byte_array_destroy(&value);
		} break;
		case SQLITE_TEXT: {
			godot_variant_chars_to_utf8_with_len(&ret, (const char *)sqlite3_column_text(stmt_data->stmt, index), sqlite3_column_bytes(stmt_data->stmt, index));
		} break;
		default: {
			gdn_core_api->godot_variant_new_nil(&ret);
		}
	}
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_column_name(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || (p_num_args < 1)) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	int index = gdn_core_api->godot_variant_as_int(p_args[0]);
	const char *name = sqlite3_column_name(stmt_data->stmt, index);
	if (!name) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, name);
	return ret;
};

#ifdef SQLITE_ENABLE_COLUMN_METADATA
godot_variant GDCALLINGCONV gdn_sqlite3_stmt_column_database_name(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || (p_num_args < 1)) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	int index = gdn_core_api->godot_variant_as_int(p_args[0]);
	const char *name = sqlite3_column_database_name(stmt_data->stmt, index);
	if (!name) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, name);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_column_table_name(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || (p_num_args < 1)) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	int index = gdn_core_api->godot_variant_as_int(p_args[0]);
	const char *name = sqlite3_column_table_name(stmt_data->stmt, index);
	if (!name) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, name);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_column_origin_name(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || (p_num_args < 1)) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	int index = gdn_core_api->godot_variant_as_int(p_args[0]);
	const char *name = sqlite3_column_origin_name(stmt_data->stmt, index);
	if (!name) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, name);
	return ret;
};
#endif

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_column_decltype(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || (p_num_args < 1)) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	int index = gdn_core_api->godot_variant_as_int(p_args[0]);
	const char *name = sqlite3_column_decltype(stmt_data->stmt, index);
	if (!name) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, name);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_is_valid(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	gdn_core_api->godot_variant_new_bool(&ret, stmt_data->stmt);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_is_busy(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_bool(&ret, false);
		return ret;
	}
	gdn_core_api->godot_variant_new_bool(&ret, sqlite3_stmt_busy(stmt_data->stmt));
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_is_readonly(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_bool(&ret, false);
		return ret;
	}
	gdn_core_api->godot_variant_new_bool(&ret, sqlite3_stmt_readonly(stmt_data->stmt));
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_clear_bindings(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_int(&ret, SQLITE_ERROR);
		return ret;
	}
	gdn_core_api->godot_variant_new_int(&ret, sqlite3_clear_bindings(stmt_data->stmt));
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_bind_parameter_count(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_int(&ret, 0);
		return ret;
	}
	gdn_core_api->godot_variant_new_int(&ret, sqlite3_bind_parameter_count(stmt_data->stmt));
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_bind_parameter_index(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || p_num_args < 1) {
		gdn_core_api->godot_variant_new_int(&ret, -1);
		return ret;
	}
	godot_char_string name_buf = godot_variant_as_char_string(p_args[0]);
	const char *name = gdn_core_api->godot_char_string_get_data(&name_buf);
	gdn_core_api->godot_variant_new_int(&ret, sqlite3_bind_parameter_index(stmt_data->stmt, name));
	gdn_core_api->godot_char_string_destroy(&name_buf);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_bind_parameter_name(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || p_num_args < 1) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	const char *name = sqlite3_bind_parameter_name(stmt_data->stmt, gdn_core_api->godot_variant_as_int(p_args[0]));
	if (!name) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, name);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_bind_parameter_value(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || p_num_args < 2) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	int index = gdn_core_api->godot_variant_as_int(p_args[0]);
	int status = 0;
	switch (gdn_core_api->godot_variant_get_type(p_args[1])) {
		case GODOT_VARIANT_TYPE_NIL: {
			status = sqlite3_bind_null(stmt_data->stmt, index);
		} break;
		case GODOT_VARIANT_TYPE_BOOL:
		case GODOT_VARIANT_TYPE_INT: {
			status = sqlite3_bind_int(stmt_data->stmt, index, gdn_core_api->godot_variant_as_int(p_args[1]));
		} break;
		case GODOT_VARIANT_TYPE_REAL: {
			status = sqlite3_bind_double(stmt_data->stmt, index, gdn_core_api->godot_variant_as_real(p_args[1]));
		} break;
		case GODOT_VARIANT_TYPE_STRING: {
			godot_char_string name_buf = godot_variant_as_char_string(p_args[1]);
			const char *name = gdn_core_api->godot_char_string_get_data(&name_buf);
			status = sqlite3_bind_text(stmt_data->stmt, index, name, -1, SQLITE_TRANSIENT);
			gdn_core_api->godot_char_string_destroy(&name_buf);
		} break;
		case GODOT_VARIANT_TYPE_POOL_BYTE_ARRAY: {
			godot_pool_byte_array array = gdn_core_api->godot_variant_as_pool_byte_array(p_args[1]);
			godot_pool_byte_array_read_access *read_access = gdn_core_api->godot_pool_byte_array_read(&array);
			status = sqlite3_bind_blob(stmt_data->stmt, index, gdn_core_api->godot_pool_byte_array_read_access_ptr(read_access), gdn_core_api->godot_pool_byte_array_size(&array), SQLITE_TRANSIENT);
			gdn_core_api->godot_pool_byte_array_read_access_destroy(read_access);
			gdn_core_api->godot_pool_byte_array_destroy(&array);
		} break;
		default: {
			DEBUG_PRINT_ERROR("Unsupported data type. Nil, Bool, Int, Float, String or PoolByteArray expected!");
			status = SQLITE_ERROR;
		}
	};
	gdn_core_api->godot_variant_new_int(&ret, status);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_belongs_to(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt || p_num_args < 1) {
		gdn_core_api->godot_variant_new_bool(&ret, false);
		return ret;
	}
	godot_object *db_obj = gdn_core_api->godot_variant_as_object(p_args[0]);
	if (!db_obj) {
		gdn_core_api->godot_variant_new_bool(&ret, false);
		return ret;
	}
	if (gdn_nativescript_1_1_api) {
		gdn_type_tag *db_tag = (gdn_type_tag *)gdn_nativescript_1_1_api->godot_nativescript_get_type_tag(db_obj);
		if (!db_tag || (db_tag->tag != GDN_SQLITE3_TYPE)) {
			DEBUG_PRINT_ERROR("Type mismatch, SQLite expected!");
			gdn_core_api->godot_variant_new_bool(&ret, false);
			return ret;
		}
	}
	gdn_sqlite3_data *db_data = (gdn_sqlite3_data *)(gdn_nativescript_api->godot_nativescript_get_userdata(db_obj));
	if (!db_data) {
		gdn_core_api->godot_variant_new_bool(&ret, false);
		return ret;
	}
	gdn_core_api->godot_variant_new_bool(&ret, sqlite3_db_handle(stmt_data->stmt) == db_data->db);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_sql(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	const char *sql = sqlite3_sql(stmt_data->stmt);
	if (!sql) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, sql);
	return ret;
};

godot_variant GDCALLINGCONV gdn_sqlite3_stmt_expanded_sql(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {

	godot_variant ret;
	gdn_sqlite3_stmt_data *stmt_data = (gdn_sqlite3_stmt_data *)p_user_data;
	if (!stmt_data->stmt) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	const char *sql = sqlite3_expanded_sql(stmt_data->stmt);
	if (!sql) {
		gdn_core_api->godot_variant_new_nil(&ret);
		return ret;
	}
	godot_variant_chars_to_utf8(&ret, sql);
	return ret;
};

/*************************************************************************/
/*  GDNative API                                                         */
/*************************************************************************/

void register_nativescript_class(void *p_handle, const char *p_class_name, const char *p_base_class_name, const gdn_type_tag *p_tag, const char *p_doc, gdn_create_func p_create_func, gdn_destroy_func p_destroy_func) {

	godot_instance_create_func create_func = {
		.create_func = p_create_func,
		.method_data = NULL,
		.free_func = NULL
	};
	godot_instance_destroy_func destroy_func = {
		.destroy_func = p_destroy_func,
		.method_data = NULL,
		.free_func = NULL
	};
	gdn_nativescript_api->godot_nativescript_register_class(p_handle, p_class_name, p_base_class_name, create_func, destroy_func);
	gdn_nativescript_api->godot_nativescript_register_tool_class(p_handle, p_class_name, p_base_class_name, create_func, destroy_func);
	if (gdn_nativescript_1_1_api) {
		gdn_nativescript_1_1_api->godot_nativescript_set_type_tag(p_handle, p_class_name, (const void *)p_tag);
		godot_string documentation;
		gdn_core_api->godot_string_new(&documentation);
		gdn_core_api->godot_string_parse_utf8(&documentation, p_doc);
		gdn_nativescript_1_1_api->godot_nativescript_set_class_documentation(p_handle, p_class_name, documentation);
		gdn_core_api->godot_string_destroy(&documentation);
	}
};

void register_nativescript_method(void *p_handle, const char *p_class_name, const char *p_method_name, const char *p_doc, gdn_method p_method, unsigned int p_num_args, godot_method_arg *p_args) {

	godot_instance_method method = {
		.method = p_method,
		.method_data = NULL,
		.free_func = NULL
	};
	godot_method_attributes attr = {
		.rpc_type = GODOT_METHOD_RPC_MODE_DISABLED
	};
	gdn_nativescript_api->godot_nativescript_register_method(p_handle, p_class_name, p_method_name, attr, method);
	if (gdn_nativescript_1_1_api) {
		godot_string documentation;
		gdn_core_api->godot_string_new(&documentation);
		gdn_core_api->godot_string_parse_utf8(&documentation, p_doc);
		gdn_nativescript_1_1_api->godot_nativescript_set_method_documentation(p_handle, p_class_name, p_method_name, documentation);
		if (p_num_args > 0) gdn_nativescript_1_1_api->godot_nativescript_set_method_argument_information(p_handle, p_class_name, p_method_name, p_num_args, p_args);
		gdn_core_api->godot_string_destroy(&documentation);
	}
	for (unsigned int i = 0; i < p_num_args; i++) {
		gdn_core_api->godot_string_destroy(&p_args[i].name);
		gdn_core_api->godot_string_destroy(&p_args[i].hint_string);
	}
};

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options) {

	gdn_core_api = p_options->api_struct;
	for (unsigned int i = 0; i < gdn_core_api->num_extensions; i++) {
		if (gdn_core_api->extensions[i]->type == GDNATIVE_EXT_NATIVESCRIPT) {
			gdn_nativescript_api = (const godot_gdnative_ext_nativescript_api_struct *)gdn_core_api->extensions[i];
			const godot_gdnative_api_struct *next = gdn_nativescript_api->next;
			while (next) {
				if ((next->version.major == 1) && (next->version.minor == 1)) {
					gdn_nativescript_1_1_api = (const godot_gdnative_ext_nativescript_1_1_api_struct *)next;
				}
				next = next->next;
			}
		}
		if (gdn_core_api->extensions[i]->type == GDNATIVE_EXT_PLUGINSCRIPT) {
			gdn_pluginscript_api = (const godot_gdnative_ext_pluginscript_api_struct *)gdn_core_api->extensions[i];
		}
		if (gdn_core_api->extensions[i]->type == GDNATIVE_EXT_ARVR) {
			gdn_arvr_api = (const godot_gdnative_ext_arvr_api_struct *)gdn_core_api->extensions[i];
		}
	}
	if (!gdn_nativescript_1_1_api) {
		DEBUG_PRINT_WARNING("WARNING: NativeScript API 1.1 not available, type checks and documentation disabled!");
	}
	method_bind_object_callv = gdn_core_api->godot_method_bind_get_method("Object", "callv");
	method_bind_file_open = gdn_core_api->godot_method_bind_get_method("_File", "open");
	method_bind_file_close = gdn_core_api->godot_method_bind_get_method("_File", "close");
	method_bind_file_get_path = gdn_core_api->godot_method_bind_get_method("_File", "get_path");
	method_bind_file_get_path_absolute = gdn_core_api->godot_method_bind_get_method("_File", "get_path_absolute");
	method_bind_file_is_open = gdn_core_api->godot_method_bind_get_method("_File", "is_open");
	method_bind_file_seek = gdn_core_api->godot_method_bind_get_method("_File", "seek");
	method_bind_file_seek_end = gdn_core_api->godot_method_bind_get_method("_File", "seek_end");
	method_bind_file_get_position = gdn_core_api->godot_method_bind_get_method("_File", "get_position");
	method_bind_file_get_len = gdn_core_api->godot_method_bind_get_method("_File", "get_len");
	method_bind_file_eof_reached = gdn_core_api->godot_method_bind_get_method("_File", "eof_reached");
	method_bind_file_get_buffer = gdn_core_api->godot_method_bind_get_method("_File", "get_buffer");
	method_bind_file_get_error = gdn_core_api->godot_method_bind_get_method("_File", "get_error");
	method_bind_file_store_buffer = gdn_core_api->godot_method_bind_get_method("_File", "store_buffer");
	method_bind_dir_remove = gdn_core_api->godot_method_bind_get_method("_Directory", "remove");
	method_bind_dir_file_exists = gdn_core_api->godot_method_bind_get_method("_Directory", "file_exists");
	method_bind_dir_dir_exists = gdn_core_api->godot_method_bind_get_method("_Directory", "dir_exists");
	method_bind_os_delay_msec = gdn_core_api->godot_method_bind_get_method("_OS", "delay_msec");
	method_bind_os_get_unix_time = gdn_core_api->godot_method_bind_get_method("_OS", "get_unix_time");
};

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options) {

	method_bind_os_delay_msec = NULL;
	method_bind_os_get_unix_time = NULL;
	method_bind_dir_remove = NULL;
	method_bind_dir_file_exists = NULL;
	method_bind_dir_dir_exists = NULL;
	method_bind_file_open = NULL;
	method_bind_file_close = NULL;
	method_bind_file_get_path = NULL;
	method_bind_file_get_path_absolute = NULL;
	method_bind_file_is_open = NULL;
	method_bind_file_seek = NULL;
	method_bind_file_seek_end = NULL;
	method_bind_file_get_position = NULL;
	method_bind_file_get_len = NULL;
	method_bind_file_eof_reached = NULL;
	method_bind_file_get_buffer = NULL;
	method_bind_file_get_error = NULL;
	method_bind_file_store_buffer = NULL;
	method_bind_object_callv = NULL;
	gdn_core_api = NULL;
	gdn_nativescript_api = NULL;
	gdn_nativescript_1_1_api = NULL;
	gdn_pluginscript_api = NULL;
	gdn_arvr_api = NULL;
};

void GDN_EXPORT godot_nativescript_init(void *p_handle) {

	//SQLite
	register_nativescript_class(p_handle, "SQLite", "Reference", &gdn_sqlite3_tag, "Database connection handle", &gdn_sqlite3_init, &gdn_sqlite3_terminate);
	godot_method_arg gdn_sqlite3_open_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("filename"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("flags"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("vfsname"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "open", "", &gdn_sqlite3_open, 3, gdn_sqlite3_open_args);
	register_nativescript_method(p_handle, "SQLite", "close", "", &gdn_sqlite3_close, 0, NULL);
	register_nativescript_method(p_handle, "SQLite", "queue_close", "", &gdn_sqlite3_queue_close, 0, NULL);
	godot_method_arg gdn_sqlite3_prepare_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("statement_text"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("statement_object"),
				.type = GODOT_VARIANT_TYPE_OBJECT,
				.hint = GODOT_PROPERTY_HINT_RESOURCE_TYPE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("SQLiteStatement") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("statement_tail"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "prepare", "", &gdn_sqlite3_prepare, 3, gdn_sqlite3_prepare_args);
	godot_method_arg gdn_sqlite3_create_collation_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("name"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("callback_host_object"),
				.type = GODOT_VARIANT_TYPE_OBJECT,
				.hint = GODOT_PROPERTY_HINT_RESOURCE_TYPE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("Object") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("callback_name"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "create_collation", "", &gdn_sqlite3_create_collation, 3, gdn_sqlite3_create_collation_args);
	godot_method_arg gdn_sqlite3_destroy_collation_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("name"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "destroy_collation", "", &gdn_sqlite3_destroy_collation, 1, gdn_sqlite3_destroy_collation_args);
	godot_method_arg gdn_sqlite3_exec_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("sql_statement"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("callback_host_object"),
				.type = GODOT_VARIANT_TYPE_OBJECT,
				.hint = GODOT_PROPERTY_HINT_RESOURCE_TYPE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("Object") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("callback_name"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("error_msg"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "exec", "", &gdn_sqlite3_exec, 4, gdn_sqlite3_exec_args);
	register_nativescript_method(p_handle, "SQLite", "errcode", "", &gdn_sqlite3_errcode, 0, NULL);
	register_nativescript_method(p_handle, "SQLite", "errmsg", "", &gdn_sqlite3_errmsg, 0, NULL);
	godot_method_arg gdn_sqlite3_errstr_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("errorcode"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "errstr", "", &gdn_sqlite3_errstr, 1, gdn_sqlite3_errstr_args);
	register_nativescript_method(p_handle, "SQLite", "version", "", &gdn_sqlite3_version, 0, NULL);
	register_nativescript_method(p_handle, "SQLite", "sourceid", "", &gdn_sqlite3_sourceid, 0, NULL);
	register_nativescript_method(p_handle, "SQLite", "changes", "", &gdn_sqlite3_changes, 0, NULL);
	register_nativescript_method(p_handle, "SQLite", "total_changes", "", &gdn_sqlite3_total_changes, 0, NULL);
	register_nativescript_method(p_handle, "SQLite", "release_memory", "", &gdn_sqlite3_release_memory, 0, NULL);
	register_nativescript_method(p_handle, "SQLite", "get_last_insert_rowid", "", &gdn_sqlite3_get_last_insert_rowid, 0, NULL);
	godot_method_arg gdn_sqlite3_set_last_insert_rowid_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("last_insert_row"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "set_last_insert_rowid", "", &gdn_sqlite3_set_last_insert_rowid, 1, gdn_sqlite3_set_last_insert_rowid_args);
	godot_method_arg gdn_sqlite3_readonly_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("name"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "readonly", "", &gdn_sqlite3_readonly, 1, gdn_sqlite3_readonly_args);
	godot_method_arg gdn_sqlite3_config_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("verb"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("arg1"),
				.type = GODOT_VARIANT_TYPE_NIL,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("arg2"),
				.type = GODOT_VARIANT_TYPE_NIL,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "config", "", &gdn_sqlite3_config, 3, gdn_sqlite3_config_args);
	godot_method_arg gdn_sqlite3_filename_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("db_name"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLite", "filename", "", &gdn_sqlite3_filename, 1, gdn_sqlite3_filename_args);

	//SQLiteStatement
	register_nativescript_class(p_handle, "SQLiteStatement", "Reference", &gdn_sqlite3_stmt_tag, "Prepared statement object", &gdn_sqlite3_stmt_init, &gdn_sqlite3_stmt_terminate);
	register_nativescript_method(p_handle, "SQLiteStatement", "step", "", &gdn_sqlite3_stmt_step, 0, NULL);
	register_nativescript_method(p_handle, "SQLiteStatement", "finalize", "", &gdn_sqlite3_stmt_finalize, 0, NULL);
	register_nativescript_method(p_handle, "SQLiteStatement", "reset", "", &gdn_sqlite3_stmt_reset, 0, NULL);

	register_nativescript_method(p_handle, "SQLiteStatement", "data_count", "", &gdn_sqlite3_stmt_data_count, 0, NULL);
	register_nativescript_method(p_handle, "SQLiteStatement", "column_count", "", &gdn_sqlite3_stmt_column_count, 0, NULL);
	godot_method_arg gdn_sqlite3_stmt_column_value_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("index"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "column_value", "", &gdn_sqlite3_stmt_column_value, 1, gdn_sqlite3_stmt_column_value_args);
	godot_method_arg gdn_sqlite3_stmt_column_name_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("index"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "column_name", "", &gdn_sqlite3_stmt_column_name, 1, gdn_sqlite3_stmt_column_name_args);
#ifdef SQLITE_ENABLE_COLUMN_METADATA
	godot_method_arg gdn_sqlite3_stmt_column_database_name_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("index"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "column_database_name", "", &gdn_sqlite3_stmt_column_database_name, 1, gdn_sqlite3_stmt_column_database_name_args);
	godot_method_arg gdn_sqlite3_stmt_column_table_name_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("index"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "column_table_name", "", &gdn_sqlite3_stmt_column_table_name, 1, gdn_sqlite3_stmt_column_table_name_args);
	godot_method_arg gdn_sqlite3_stmt_column_origin_name_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("index"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "column_origin_name", "", &gdn_sqlite3_stmt_column_origin_name, 1, gdn_sqlite3_stmt_column_origin_name_args);
#endif
	godot_method_arg gdn_sqlite3_stmt_column_decltype_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("index"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "column_decltype", "", &gdn_sqlite3_stmt_column_decltype, 1, gdn_sqlite3_stmt_column_decltype_args);
	register_nativescript_method(p_handle, "SQLiteStatement", "is_valid", "", &gdn_sqlite3_stmt_is_valid, 0, NULL);
	register_nativescript_method(p_handle, "SQLiteStatement", "is_busy", "", &gdn_sqlite3_stmt_is_busy, 0, NULL);
	register_nativescript_method(p_handle, "SQLiteStatement", "is_readonly", "", &gdn_sqlite3_stmt_is_readonly, 0, NULL);
	register_nativescript_method(p_handle, "SQLiteStatement", "bind_parameter_count", "", &gdn_sqlite3_stmt_bind_parameter_count, 0, NULL);
	godot_method_arg gdn_sqlite3_stmt_bind_parameter_index_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("name"),
				.type = GODOT_VARIANT_TYPE_STRING,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "bind_parameter_index", "", &gdn_sqlite3_stmt_bind_parameter_index, 1, gdn_sqlite3_stmt_bind_parameter_index_args);
	godot_method_arg gdn_sqlite3_stmt_bind_parameter_name_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("index"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "bind_parameter_name", "", &gdn_sqlite3_stmt_bind_parameter_name, 1, gdn_sqlite3_stmt_bind_parameter_name_args);
	godot_method_arg gdn_sqlite3_stmt_bind_parameter_value_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("index"),
				.type = GODOT_VARIANT_TYPE_INT,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") },
		{ .name = gdn_core_api->godot_string_chars_to_utf8("data"),
				.type = GODOT_VARIANT_TYPE_NIL,
				.hint = GODOT_PROPERTY_HINT_NONE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "bind_parameter_value", "", &gdn_sqlite3_stmt_bind_parameter_value, 2, gdn_sqlite3_stmt_bind_parameter_value_args);
	register_nativescript_method(p_handle, "SQLiteStatement", "clear_bindings", "", &gdn_sqlite3_stmt_clear_bindings, 0, NULL);
	godot_method_arg gdn_sqlite3_stmt_belongs_to_args[] = {
		{ .name = gdn_core_api->godot_string_chars_to_utf8("db_object"),
				.type = GODOT_VARIANT_TYPE_OBJECT,
				.hint = GODOT_PROPERTY_HINT_RESOURCE_TYPE,
				.hint_string = gdn_core_api->godot_string_chars_to_utf8("SQLite") }
	};
	register_nativescript_method(p_handle, "SQLiteStatement", "belongs_to", "", &gdn_sqlite3_stmt_belongs_to, 1, gdn_sqlite3_stmt_belongs_to_args);
	register_nativescript_method(p_handle, "SQLiteStatement", "sql", "", &gdn_sqlite3_stmt_sql, 0, NULL);
	register_nativescript_method(p_handle, "SQLiteStatement", "expanded_sql", "", &gdn_sqlite3_stmt_expanded_sql, 0, NULL);

	//GodotVFS
	register_godot_vfs();
};
