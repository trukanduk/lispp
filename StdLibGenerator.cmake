set (STDLIB_LISP_SOURCES_PATH ${CMAKE_SOURCE_DIR}/src/core/stdlib)
set (STDLIB_CPP_SOURCES_PATH ${CMAKE_BINARY_DIR}/stdlib)
file (MAKE_DIRECTORY ${STDLIB_CPP_SOURCES_PATH})
set (STDLIB_FILES_TEMPLATE "${STDLIB_LISP_SOURCES_PATH}/template.cpp.in")
include_directories("${CMAKE_BINARY_DIR}/stdlib")

macro (generate_stdlib_source module_name dest_variable)
  set (SOURCE_MODULE "${STDLIB_LISP_SOURCES_PATH}/${module_name}.lisp")
  set (DEST_CPP_FILE "${STDLIB_CPP_SOURCES_PATH}/${module_name}_stdlib_module.cpp")

  ADD_CUSTOM_COMMAND(
      OUTPUT ${DEST_CPP_FILE}
      COMMAND python stdlib_cpp_gen.py ${STDLIB_FILES_TEMPLATE} ${module_name} ${SOURCE_MODULE} ${DEST_CPP_FILE}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tools
      DEPENDS ${SOURCE_MODULE}
      COMMENT "Generation ${DEST_CPP_FILE} stdlib file for lispp project"
  )

  set(${dest_variable} ${${dest_variable}} ${DEST_CPP_FILE})
endmacro ()
