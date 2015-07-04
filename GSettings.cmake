set(PKG_CONFIG_EXECUTABLE pkg-config)

#find the gsettings compiler name
execute_process (COMMAND ${PKG_CONFIG_EXECUTABLE} gio-2.0 --variable glib_compile_schemas  OUTPUT_VARIABLE _glib_comple_schemas OUTPUT_STRIP_TRAILING_WHITESPACE)

macro (compile_schemas SCHEMA_DIR)
	execute_process (COMMAND ${_glib_comple_schemas} ${SCHEMA_DIR})
endmacro()