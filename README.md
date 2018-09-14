# Image Segmentation

This project consists on a program written in C++ which purpose is to segment the pixels of an image, that is, classify them as foreground or background pixels. I implemented Dinic's Algorithm to solve this problem.

## Getting Started

You will need the g++ compiler installed in your machine.

### Compiling

Download the .zip and compile the 'main.cpp' file using the following command:

```
$ g++ -o segmentation main.cpp
```
which should create the executable file 'segmentation'.

## Running

Make sure the executable file is in the same directory as the input files contained in 'tests'.
The program should be executed in the following way:

```
$ ./segmentation < t01.in > test01.myout
```

Afterwards you can compare your output with the foressen output using:

```
$ diff t01.out test01.myout
```

## Author

* Francisco Matos

## Evaluation

19/20
