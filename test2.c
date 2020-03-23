#include <stdio.h>

int t = 0;
int getAndIncrement() { return t++; }

int main(int argc, char const *argv[]) {
    printf("Gotten: %d\nAcctual: %d\n", getAndIncrement(), t);
    return 0;
}
