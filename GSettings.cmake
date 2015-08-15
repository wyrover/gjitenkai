#find the gsettings compiler name
execute_process (
COMMAND ${PKG_CONFIG_EXECUTABLE} gio-2.0 --variable glib_compile_schemas  
OUTPUT_VARIABLE GLIB_COMPLE_SCHEMAS OUTPUT_STRIP_TRAILING_WHITESPACE)

macro (compile_schemas SCHEMA_DIR TARGET_DIR)
	execute_process (COMMAND ${GLIB_COMPLE_SCHEMAS} ${SCHEMA_DIR} --targetdir=${TARGET_DIR})
endmacro()