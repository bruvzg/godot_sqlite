project(
    'libgodot_sqlite3', 'c',
    meson_version : '>= 0.45'
)
library_version = '0.1.0'

godot_sqlite3_sources = [
    'src/godot_sqlite3.c',
    'src/sqlite3/sqlite3.c'
]

c_args = [
    '-DSQLITE_THREADSAFE=' + (get_option('threadsafe') ? '1' : '0'),
    '-DSQLITE_OMIT_LOAD_EXTENSION=' + (get_option('dynamic-extensions') ? '0' : '1'),
    '-DSQLITE_ENABLE_COLUMN_METADATA=' + (get_option('column-metadata') ? '1' : '0'),
    '-DSQLITE_TEMP_STORE=3'
]

link_args = []

if host_machine.system() == 'linux'
    link_args += '-lpthread'
    if get_option('static-runtime')
        link_args += [
            '-static-libgcc'
        ]
    endif
    if get_option('dynamic-extensions')
        link_args += '-ldl'
    endif
elif host_machine.system() == 'windows'
    if meson.get_compiler('c').get_id() == 'msvc'
        cpp_args += '/EHsc'
        if get_option('static-runtime')
            cpp_args += '/MT'
        else
            cpp_args += '/MD'
        endif
    else
        if get_option('static-runtime')
            link_args += [
                '-static-libgcc'
            ]
        endif
    endif
endif

shared_library('godot_sqlite3' + '.' + target_machine.system() + '.' + target_machine.cpu_family(), godot_sqlite3_sources, link_args : link_args, c_args : c_args, include_directories : include_directories('src', 'src/sqlite3', 'src/godot_headers'))
