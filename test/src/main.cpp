#include <iostream>
#include <windows.h>
// Short program for test.
int main(int agrc, char** argv) { unsigned int i = 0; unsigned char c = '\x02'; for (;; Sleep(5000), i++) { std::cout << i << " " << &i << std::endl; } }