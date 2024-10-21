#include <stdio.h>
signed int globalVar;
void funcK(float* param) {
    const char* localVar = param;
    printf("Type is signed int\n"); // "signed int" in string should not be replaced
}
int main() {
    signed int myVar = 10;
    printf("The value of myVar is %d\n", myVar);
    return 0;
}