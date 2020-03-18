#include <stdio.h>

void foo(char str[]) { str = "1234"; }

int main(int argc, char const *argv[]) {
    int test[20];
    printf("string começa em: %p\n", &test);
    int a = 10;
    int b = 20;
    printf("int começa em %p\n", &a);
    int byte_offset = (void *)test - (void *)(&a);
    printf("Byte offset: %d\t Char offset: %ld\n", byte_offset,
           byte_offset / sizeof(int));
    printf("anres a: %d\n", a);
    test[-byte_offset / sizeof(int)] = 99;
    for (int i = -10000; i < 10000; i++) {
        test[i] = 0;
    }

    printf("depois a: %d\n", a);
}
