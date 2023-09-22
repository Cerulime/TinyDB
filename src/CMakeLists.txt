add_library(Runtime Runtime.cpp)
add_library(REPL REPL.cpp)
add_library(HashTable HashTable.cpp)

target_link_libraries(REPL Runtime)