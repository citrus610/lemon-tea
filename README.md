# Lemon Tea
Guildline versus tetris bot

## How to build
### Notices
Lemon Tea makes use of standard library header `<bit>` available in C++20, so make sure your compiler support C++20.
### Install SFML
SFML are used to create the tetris client. Download the latest version of SFML here: https://www.sfml-dev.org/download.php
### Build the tetris client
- Clone the repository.
- In [`client/CMakeLists.txt`](client/CMakeLists.txt), set your SFML directory `set(SFML_DIR "<insert-your-sfml-dir-here>/lib/cmake/SFML")`.
- `cd` to your repository
- `mkdir build`, `cd build` and `cmake ..` to build. The binary can be found in the `build` folder.
- Remember to copy the [`client/res`](client/res) folder to the `build` directory, the tetris client needs those files.
