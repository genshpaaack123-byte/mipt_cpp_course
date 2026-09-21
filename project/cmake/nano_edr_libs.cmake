# Подключение готовых библиотек границ: os и edr.
#
# Библиотеки поставляются собранными; всё, что о них нужно знать, — в
# include/os.h, include/edr.h и SPEC.md.

if(WIN32)
    set(NANO_EDR_PLATFORM win-x64)
elseif(APPLE)
    message(FATAL_ERROR
        "На macOS проект не собирается: готовых os и edr под неё нет. "
        "Работа на macOS идёт в Linux-контейнере, он описан каталогом "
        ".devcontainer в репозитории курса — SETUP.md, раздел "
        "«macOS: работа в контейнере».")
elseif(UNIX)
    set(NANO_EDR_PLATFORM linux-x64)
else()
    message(FATAL_ERROR
        "Готовых библиотек для этой системы (${CMAKE_SYSTEM_NAME}) в комплекте нет: "
        "собираются win-x64 и linux-x64. Напишите преподавателю — сборка "
        "под вашу платформу это один прогон скрипта, а не переделка задания.")
endif()

set(NANO_EDR_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(NANO_EDR_LIB_DIR "${NANO_EDR_ROOT}/lib/${NANO_EDR_PLATFORM}")

if(WIN32)
    set(NANO_EDR_OS_BIN  "${NANO_EDR_LIB_DIR}/os.dll")
    set(NANO_EDR_OS_LINK "${NANO_EDR_LIB_DIR}/os.lib")
    set(NANO_EDR_EDR_BIN  "${NANO_EDR_LIB_DIR}/edr.dll")
    set(NANO_EDR_EDR_LINK "${NANO_EDR_LIB_DIR}/edr.lib")
else()
    set(NANO_EDR_OS_BIN  "${NANO_EDR_LIB_DIR}/libos.so")
    set(NANO_EDR_EDR_BIN "${NANO_EDR_LIB_DIR}/libedr.so")
endif()

foreach(f "${NANO_EDR_OS_BIN}" "${NANO_EDR_EDR_BIN}")
    if(NOT EXISTS "${f}")
        message(FATAL_ERROR
            "Не найдена библиотека: ${f}"
            "\nКомплект не несёт границ под ${NANO_EDR_PLATFORM}. "
            "Обновите клон (git pull); если каталога "
            "lib/${NANO_EDR_PLATFORM} нет и после этого — напишите "
            "преподавателю.")
    endif()
endforeach()

add_library(os SHARED IMPORTED GLOBAL)
set_target_properties(os PROPERTIES IMPORTED_LOCATION "${NANO_EDR_OS_BIN}")
target_include_directories(os INTERFACE "${NANO_EDR_ROOT}/include")
target_compile_definitions(os INTERFACE OS_USE_SHARED)

add_library(edr SHARED IMPORTED GLOBAL)
set_target_properties(edr PROPERTIES IMPORTED_LOCATION "${NANO_EDR_EDR_BIN}")
target_include_directories(edr INTERFACE "${NANO_EDR_ROOT}/include")
target_compile_definitions(edr INTERFACE EDR_USE_SHARED)

if(WIN32)
    set_target_properties(os  PROPERTIES IMPORTED_IMPLIB "${NANO_EDR_OS_LINK}")
    set_target_properties(edr PROPERTIES IMPORTED_IMPLIB "${NANO_EDR_EDR_LINK}")
endif()

# ASan runtime на Windows нужен не только при MSVC, но и при clang++ с MSVC ABI.
# В Visual Studio clang++.exe обычно лежит в VC/Tools/Llvm, а DLL ASan —
# в VC/Tools/MSVC/<version>/bin/Hostx64/x64, поэтому ищем в обоих местах.
if(NANO_EDR_SANITIZE AND WIN32)
    get_filename_component(NANO_EDR_COMPILER_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB NANO_EDR_ASAN_RUNTIME
         "${NANO_EDR_COMPILER_BIN}/clang_rt.asan_dynamic-*.dll")

    if(NOT NANO_EDR_ASAN_RUNTIME)
        file(GLOB NANO_EDR_MSVC_TOOLSETS
             "${NANO_EDR_COMPILER_BIN}/../../../MSVC/*")
        foreach(toolset IN LISTS NANO_EDR_MSVC_TOOLSETS)
            file(GLOB NANO_EDR_ASAN_CANDIDATES
                 "${toolset}/bin/Hostx64/x64/clang_rt.asan_dynamic-*.dll"
                 "${toolset}/bin/Hostx64/x86/clang_rt.asan_dynamic-*.dll"
                 "${toolset}/bin/Hostx86/x86/clang_rt.asan_dynamic-*.dll")
            list(APPEND NANO_EDR_ASAN_RUNTIME ${NANO_EDR_ASAN_CANDIDATES})
        endforeach()
    endif()

    if(NOT NANO_EDR_ASAN_RUNTIME)
        message(FATAL_ERROR
            "Не найдена библиотека ASan в каталоге компилятора или MSVC toolset.\n"
            "Проверьте установку компонента C++ AddressSanitizer в Visual Studio.")
    endif()
endif()

function(nano_edr_copy_runtime target)
    if(WIN32)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:os>" "$<TARGET_FILE:edr>"
                    ${NANO_EDR_ASAN_RUNTIME}
                    "$<TARGET_FILE_DIR:${target}>"
            COMMENT "Кладу os.dll, edr.dll и ASan runtime рядом с ${target}")
    else()
        set_target_properties(${target} PROPERTIES
            BUILD_RPATH "${NANO_EDR_LIB_DIR}")
    endif()
endfunction()
