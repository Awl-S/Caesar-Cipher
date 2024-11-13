cmake_minimum_required(VERSION 3.22.1)
project(Caesar-Cipher C CXX)

# Установка стандартов C и C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=address,undefined -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O2 -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")

# Проверка компилятора
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Используется компилятор Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "Используется компилятор GCC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "Используется компилятор MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
endif()

# Источники для C и C++
file(GLOB C_SOURCES "src/*.c")
file(GLOB CPP_SOURCES "src/*.cpp")

# Создание целей для C и C++
if(C_SOURCES)
    add_executable(Caesar-Cipher_C ${C_SOURCES})
    target_include_directories(Caesar-Cipher_C PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

if(CPP_SOURCES)
    add_executable(Caesar-Cipher_CPP ${CPP_SOURCES})
    target_include_directories(Caesar-Cipher_CPP PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

# Проверка наличия libmicrohttpd
find_path(MICROHTTPD_INCLUDE_DIR microhttpd.h)
find_library(MICROHTTPD_LIBRARY NAMES microhttpd)

if (NOT MICROHTTPD_INCLUDE_DIR OR NOT MICROHTTPD_LIBRARY)
    if(WIN32)
        message(FATAL_ERROR "libmicrohttpd может быть недоступна на Windows. Убедитесь, что вы собрали её вручную.")
    else()
        message(FATAL_ERROR "libmicrohttpd не найдена. Установите libmicrohttpd-dev или укажите путь вручную.")
    endif()
else()
    message(STATUS "libmicrohttpd найдена: ${MICROHTTPD_LIBRARY}")
    message(STATUS "Include директория: ${MICROHTTPD_INCLUDE_DIR}")
endif()

if(TARGET Caesar-Cipher_C)
    target_include_directories(Caesar-Cipher_C PRIVATE ${MICROHTTPD_INCLUDE_DIR})
    target_link_libraries(Caesar-Cipher_C ${MICROHTTPD_LIBRARY})
endif()

# Включаем CTest для тестирования
enable_testing()
if(TARGET Caesar-Cipher_C OR TARGET Caesar-Cipher_CPP)
    if(TARGET Caesar-Cipher_C)
        add_test(NAME Caesar-Cipher_C_Test COMMAND Caesar-Cipher_C)
    endif()
    if(TARGET Caesar-Cipher_CPP)
        add_test(NAME Caesar-Cipher_CPP_Test COMMAND Caesar-Cipher_CPP)
    endif()
else()
    message(WARNING "Ни одна цель не создана, тесты недоступны.")
endif()

# Опция для санитайзеров
option(ENABLE_SANITIZERS "Включить AddressSanitizer и UndefinedBehaviorSanitizer" OFF)
if(ENABLE_SANITIZERS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
endif()

# Покрытие кода с использованием lcov и gcovr
find_program(LCOV lcov)
find_program(GCOVR gcovr)

if (LCOV AND GCOVR)
    add_custom_target(coverage
        COMMAND ${LCOV} --capture --directory . --output-file coverage.info
        COMMAND ${LCOV} --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
        COMMAND ${LCOV} --list coverage.info
        COMMAND ${GCOVR} --root . --html --html-details -o coverage.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating code coverage report"
        VERBATIM
    )
    message(STATUS "Цель 'coverage' доступна")
else()
    message(WARNING "Инструменты 'lcov' и/или 'gcovr' не найдены, цель 'coverage' недоступна.")
endif()

# Статический анализ с cppcheck
find_program(CPPCHECK cppcheck)
if (CPPCHECK)
    add_custom_target(static_analysis
        COMMAND ${CPPCHECK} --enable=all --inconclusive --quiet ${CMAKE_SOURCE_DIR}
        COMMENT "Running static analysis with cppcheck"
        VERBATIM
    )
    message(STATUS "Цель 'static_analysis' доступна")
else()
    message(WARNING "Инструмент 'cppcheck' не найден, цель 'static_analysis' недоступна.")
endif()

# Форматирование кода с clang-format
find_program(CLANG_FORMAT clang-format)
if (CLANG_FORMAT)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${C_SOURCES} ${CPP_SOURCES}
        COMMENT "Formatting source code with clang-format"
        VERBATIM
    )
    message(STATUS "Цель 'format' доступна")
else()
    message(WARNING "Инструмент 'clang-format' не найден, цель 'format' недоступна.")
endif()

# Проверка памяти с Valgrind
find_program(VALGRIND valgrind)
if (VALGRIND)
    if(TARGET Caesar-Cipher_C)
        add_custom_target(memory_check_c
            COMMAND ${VALGRIND} --leak-check=full --track-origins=yes ./Caesar-Cipher_C
            COMMENT "Running memory check on C executable with Valgrind"
            VERBATIM
        )
    endif()

    if(TARGET Caesar-Cipher_CPP)
        add_custom_target(memory_check_cpp
            COMMAND ${VALGRIND} --leak-check=full --track-origins=yes ./Caesar-Cipher_CPP
            COMMENT "Running memory check on C++ executable with Valgrind"
            VERBATIM
        )
    endif()

    message(STATUS "Цели 'memory_check_c' и 'memory_check_cpp' доступны")
else()
    message(WARNING "Инструмент 'valgrind' не найден, цели 'memory_check_c' и 'memory_check_cpp' недоступны.")
endif()