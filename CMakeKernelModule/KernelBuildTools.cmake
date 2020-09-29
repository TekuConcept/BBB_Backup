#
# Created by TekuConcept on February 21, 2020
#

SET(TARGET_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}")
STRING(TOLOWER "${TARGET_SYSTEM_NAME}" TARGET_SYSTEM_NAME)
IF (NOT "${TARGET_SYSTEM_NAME}" STREQUAL "linux")
    MESSAGE(FETAL_ERROR
        " Cannot build kernel modules for target system ${TARGET_SYSTEM_NAME}")
ENDIF ()



EXECUTE_PROCESS(
    COMMAND bash -c "uname -r"
    OUTPUT_VARIABLE KVERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
SET(KMODULE_BUILD "/lib/module/${KVERSION}/build"
    CACHE STRING "Linux Build Directory")
SET(KMODULE_ARCH "${CMAKE_SYSTEM_PROCESSOR}"
    CACHE STRING "Target Architecture")
SET(KMODULE_CROSS_COMPILE "${CROSS_COMPILER_PREFIX}"
    CACHE STRING "Target Toolchain Prefix")
SET(KMODULE_CFLAGS "")
SET(KMODULE_AFLAGS "")
SET(KMODULE_LDFLAGS "")



MACRO(WRITE_KBUILD target_name target_sources)
    SET(COMMON_INCLUDES "")
    GET_PROPERTY(includes
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY INCLUDE_DIRECTORIES)
    FOREACH(inc ${includes})
        STRING(APPEND COMMON_INCLUDES "-I${inc} ")
    ENDFOREACH()
    SET(target_build_dir "${CMAKE_CURRENT_BINARY_DIR}/${target_name}")
    SET(data "obj-m := ${target_name}.o\n")
    STRING(APPEND data "${target_name}-y := ${target_sources}\n")
    STRING(APPEND data "EXTRA_CFLAGS := ${KMODULE_CFLAGS} ${COMMON_INCLUDES}\n")
    STRING(APPEND data "EXTRA_AFLAGS := ${KMODULE_AFLAGS} ${COMMON_INCLUDES}\n")
    STRING(APPEND data "EXTRA_LDFLAGS := ${KMODULE_LDFLAGS}\n")
    FILE(WRITE ${target_build_dir}/Kbuild ${data})
ENDMACRO()



MACRO(SET_KTARGET_FLAGS target_name flag_prefix target_flags)
    SET(MODULE_NAME ${target_name})
    SET(target_build_dir "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}")
    IF(NOT EXISTS ${target_build_dir})
        MESSAGE(FETAL_ERROR "Target ${MODULE_NAME} does not exist")
    ENDIF()
    SET(MODULE_FLAGS ${target_flags})
    SET(LOCAL_FLAGS "")
    FOREACH(flag ${MODULE_FLAGS})
        STRING(APPEND LOCAL_FLAGS "${flag} ")
    ENDFOREACH()
    FILE(APPEND ${target_build_dir}/Kbuild
        "EXTRA_${flag_prefix}FLAGS += ${LOCAL_FLAGS}\n")
ENDMACRO()



FUNCTION(TARGET_KINCLUDE_DIRECTORIES)
    IF (NOT ${ARGC} GREATER_EQUAL 2)
        MESSAGE(FETAL_ERROR " Incorrect number of arguments")
    ENDIF ()
    SET(MODULE_NAME ${ARGV0})
    SET(target_build_dir "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}")
    IF(NOT EXISTS ${target_build_dir})
        MESSAGE(FETAL_ERROR "Target ${MODULE_NAME} does not exist")
    ENDIF()
    SET(MODULE_INCLUDES ${ARGV})
    LIST(REMOVE_AT MODULE_INCLUDES 0)
    SET(LOCAL_INCLUDES "")
    FOREACH(inc ${MODULE_INCLUDES})
        IF (NOT IS_ABSOLUTE ${inc})
            SET(inc "${CMAKE_CURRENT_SOURCE_DIR}/${inc}")
        ENDIF ()
        STRING(APPEND LOCAL_INCLUDES "-I${inc} ")
    ENDFOREACH()
    FILE(APPEND ${target_build_dir}/Kbuild
        "EXTRA_CFLAGS += ${LOCAL_INCLUDES}\nEXTRA_AFLAGS += ${LOCAL_INCLUDES}\n")
ENDFUNCTION()



FUNCTION(TARGET_KMODULE_CFLAGS target)
    IF (NOT ${ARGC} GREATER_EQUAL 2)
        MESSAGE(FETAL_ERROR " Incorrect number of arguments")
    ENDIF ()
    SET_KTARGET_FLAGS(${target} "C" ${ARGN})
ENDFUNCTION()



FUNCTION(TARGET_KMODULE_AFLAGS target)
    IF (NOT ${ARGC} GREATER_EQUAL 2)
        MESSAGE(FETAL_ERROR " Incorrect number of arguments")
    ENDIF ()
    SET_KTARGET_FLAGS(${target} "A" ${ARGN})
ENDFUNCTION()



FUNCTION(TARGET_KMODULE_LDFLAGS target)
    IF (NOT ${ARGC} GREATER_EQUAL 2)
        MESSAGE(FETAL_ERROR " Incorrect number of arguments")
    ENDIF ()
    SET_KTARGET_FLAGS(${target} "LD" ${ARGN})
ENDFUNCTION()



FUNCTION(TARGET_KMODULE_SYMVERS target_name)
    IF (NOT ${ARGC} GREATER_EQUAL 2)
        MESSAGE(FETAL_ERROR " Incorrect number of arguments")
    ENDIF ()

    SET(MODULE_NAME ${target_name})
    SET(target_build_dir "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}")
    IF(NOT EXISTS ${target_build_dir})
        MESSAGE(FETAL_ERROR "Target ${MODULE_NAME} does not exist")
    ENDIF()
    SET(MODULE_SYMVERS ${ARGN})
    SET(LOCAL_SYMVERS "")
    FOREACH(symver ${MODULE_SYMVERS})
        IF (TARGET ${symver})
            GET_PROPERTY(symver_location TARGET ${symver} PROPERTY LOCATION)
            STRING(APPEND LOCAL_SYMVERS "${symver_location}/Module.symvers ")
        ELSE ()
            STRING(APPEND LOCAL_SYMVERS "${symver} ")
        ENDIF ()
    ENDFOREACH()

    FILE(APPEND ${target_build_dir}/Kbuild
        "KBUILD_EXTRA_SYMBOLS += ${LOCAL_SYMVERS}\n")
ENDFUNCTION()



FUNCTION(ADD_KMODULE)
    IF (NOT ${ARGC} GREATER_EQUAL 2)
        MESSAGE(FETAL_ERROR " Incorrect number of arguments")
    ENDIF ()

    SET(MODULE_NAME ${ARGV0})
    SET(MODULE_SOURCE_FILES ${ARGV})
    SET(MODULE_SOURCES "")
    SET(MODULE_BYPRODUCTS "")
    LIST(REMOVE_AT MODULE_SOURCE_FILES 0)
    SET(MODULE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}")
    EXECUTE_PROCESS(COMMAND bash -c "mkdir -p ${MODULE_DIR}" VERBATIM)

    FOREACH(in_f ${MODULE_SOURCE_FILES})
        IF(NOT IS_ABSOLUTE ${in_f})
            SET(in_f "${CMAKE_CURRENT_SOURCE_DIR}/${in_f}")
        ENDIF()
        FILE(RELATIVE_PATH out_f ${CMAKE_CURRENT_SOURCE_DIR} ${in_f})
        GET_FILENAME_COMPONENT(tmp_n ${out_f} NAME_WE)   # get file name
        GET_FILENAME_COMPONENT(tmp_d ${out_f} DIRECTORY) # get rel-dir
        SET(next_bin "${tmp_d}/${tmp_n}.o")
        SET(tmp_d "${MODULE_DIR}/${tmp_d}")
        SET(out_f "${MODULE_DIR}/${out_f}")
        STRING(APPEND MODULE_SOURCES "${next_bin} ")
        LIST(APPEND MODULE_BYPRODUCTS
            "${MODULE_DIR}/${next_bin}"
            "${MODULE_DIR}/${next_bin}.cmd"
        )
        EXECUTE_PROCESS(COMMAND bash -c "mkdir -p ${tmp_d}" VERBATIM)
        EXECUTE_PROCESS(COMMAND bash -c "ln -sf ${in_f} ${out_f}" VERBATIM)
    ENDFOREACH()

    WRITE_KBUILD(${MODULE_NAME} ${MODULE_SOURCES})
    SET(KBUILD $(MAKE) V=1 -C ${KMODULE_BUILD} M=${MODULE_DIR}
        ARCH=${KMODULE_ARCH} CROSS_COMPILE=${KMODULE_CROSS_COMPILE})
    ADD_CUSTOM_COMMAND(
        COMMAND ${KBUILD}
        COMMENT "Building ${MODULE_NAME}.ko" VERBATIM
        OUTPUT ${MODULE_DIR}/${MODULE_NAME}.ko
        DEPENDS ${MODULE_SOURCE_FILES}
        WORKING_DIRECTORY ${MODULE_DIR}
        BYPRODUCTS
        ${MODULE_BYPRODUCTS}
        ".${MODULE_NAME}.ko.cmd"
        ".${MODULE_NAME}.mod.o.cmd"
        ".${MODULE_NAME}.o.cmd"
        ".built-in.o.cmd"
        "${MODULE_NAME}.mod.c"
        "${MODULE_NAME}.mod.o"
        "${MODULE_NAME}.o"
        "built-in.o"
        "Module.symvers"
        "modules.order"
    )
    ADD_CUSTOM_TARGET(
        ${MODULE_NAME} ALL
        DEPENDS ${MODULE_DIR}/${MODULE_NAME}.ko
    )
    SET_TARGET_PROPERTIES(
        ${MODULE_NAME}
        PROPERTIES
        LOCATION    ${MODULE_DIR}
        OUTPUT_NAME ${MODULE_NAME}
    )
ENDFUNCTION()
