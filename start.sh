# Run the program by removing build files, running CMake, making the
# executable, and running the executable.
rm -rf build/* && cd build/ && cmake ../ && make && ./main "$@" && cd ..;
