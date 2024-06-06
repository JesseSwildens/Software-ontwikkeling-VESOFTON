# Example project
For the usage of the api layer, is this project an example of how to implement applications based on the library created in the main build of this project.

## Usage
To build this project, you'd first have to build the main project in the ```app``` folder. To build that project look in the readme for the usage or using the doxygen page usage on the navigation bar.
When building of the app project is successful get in the build folder the ```libSoftwareOntwikkelingLib.a``` file and copy into the ```Lib``` Folder in this project and rename the file to ```graphics_lib.a``` Or any other name when changing the line:
```target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/graphics_lib.a)```
to the desired name on the end of the line:
```target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/desired filename.a)```

### CLI
After the copy has been completed you can build the project by running the command following commands:
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=DEBUG
make all -j$(nproc)
```
This generates example_project.elf which can be flashed using your debugging server of choice to flash.

### Visual Studio Code
Open the folder as the root folder. Then using the CMake tools & CMake extension open the CMakeLists.txt in the root of the folder.
Then using the compiler of choice (That can build Cross-compiled projects for ARM Cortex M4 processors) build the project. Using an extension like Cortex-Debug flashing the generated example_project.elf file should result in a working software package that runs on the microcontroller.