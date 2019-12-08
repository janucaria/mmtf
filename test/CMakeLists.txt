enable_testing()

add_executable(janucaria-mmtf-test)

target_sources(janucaria-mmtf-test
  PRIVATE
    src/codec_header_test.cpp
    src/decode_header_test.cpp
    src/main.cpp
)

target_link_libraries(janucaria-mmtf-test
  PRIVATE
    janucaria::mmtf
    Catch2::Catch2
)

add_test(
  NAME janucaria-mmtf-test
  COMMAND janucaria-mmtf-test
)
  