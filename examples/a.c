#include <stdio.h>

int transform(int x) {
    return x * 3;
}

int main() {
    int n = 10;
    int total = 0;

    while (n > 0) {
        if (n % 2 == 0) {
            total += transform(n);
        } else {
            total -= n;
        }

        n--;
    }

    return total;
}
