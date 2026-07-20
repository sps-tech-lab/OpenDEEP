include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)

FetchContent_MakeAvailable(Catch2)

add_executable(unit-tests
  test_ms5837.cpp
  support/fake_ms5837_bus.cpp
  ${CMAKE_SOURCE_DIR}/src/MS5837/MS5837.cpp
)

# The driver under test includes Pico SDK / BSP headers. On the host we shadow
# them with the stubs in tests/stubs and back the I2C API with the fake bus.
target_include_directories(unit-tests PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/stubs
  ${CMAKE_CURRENT_LIST_DIR}/support
  ${CMAKE_SOURCE_DIR}/src/MS5837
)

target_compile_features(unit-tests PRIVATE cxx_std_20)

target_link_libraries(unit-tests PRIVATE
  Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(unit-tests)
