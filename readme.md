# Requirement:
- opencv
- cmake
- g++
- openni2

# How to build:
```
$ cp CMakeLists.template CMakeLists.txt
```
Edit OpenNI2's directory in CMakeLists.txt
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

# How to run:
```
$ cd build
$ ./Setup
$ ./Client
```
