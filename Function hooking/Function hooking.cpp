#include <cstdio>

int ReturnsAValue() {
    return 0;
}

int main()
{
    printf("%s", "Press any key to continue.");
    getchar();
    printf("ReturnsAValue returned %d", ReturnsAValue());
}