
add_library(dht20_lib dht20.c)

add_executable(dht20 main.c)

target_link_libraries(dht20 
PUBLIC dht20_lib
)

target_compile_options(dht20_lib PRIVATE -Werror -Wall -Wextra -Wpedantic)
target_compile_options(dht20 PRIVATE -Werror -Wall -Wextra -Wpedantic)