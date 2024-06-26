# App
This project is the main project which contains the source code for the library and example code that build software to use the UART to test the functionality in the API layer.

# Prerequisites
To build this project some software from other parties are required. Before attempting to build this project verify that the following software is installed:
- CMake (Version 3.20 and higher)
- Ninja or Unix Makefile
- ARM Toolchain or arm-none-eabi-gcc software suite

For unit testing the following software is required:
- CppCheck (Version 2.14 or newer)
- Catch2 (Is installed in the Catch2 submodule folder but is required to be pulled from the repository)
- GCC (x86 compiler)
- CMake (Version 3.20 and higher)
- Ninja or Unix Makefile

# Usage
To build this project there are multiple methods to get cmake to generate all the files needed. For each specific machine instructions can be different so these are common desktop machines and Visual Studio Code.

## CLI (MCU Firmware)
If the perquisites are installed running the following command from the root folder of this repository should build this project:
```
mkdir app/build
cd app/build
cmake .. -DCMAKE_BUILD_TYPE=DEBUG
make all -j$(nproc)
```

After which a file named `libSoftwareOntwikkelingLib.a` is build which can be copied to other projects to be used to statically link into other applications.
For testing the API layer `SoftwareOntwikkelingGroep8.elf` gets build which can be flashed using most common GDB servers, to test using the UART.
See the docs page for the supported commands.

## CLI (Unit test)
When trying to run the unit tests only the listed items under unit testing are required. Just like in the MCU Firmware a build folder with cmake needs to be made.
But by adding a flag to the cmake command some files get exchanged for mock files as these tests are run on the host PC so at the time of writing the most common architecture is x86.
Therefore peripherals like for the NVIC or DMA aren't possible to be written to and are exchanged for other function calls.</br>
To build and run the unit test run the following commands from the root folder:
```
mkdir app/build-unit-test
cd app/build-unit-test
cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_UNIT_TEST=TRUE
make all -j$(nproc)
ctest
```
This should generate a xml report with the name `results_{test_case_name}.xml` for each test in the build folder.