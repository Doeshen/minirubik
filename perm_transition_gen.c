#include <stdint.h>
#include <stdio.h>

#define STATES 5040
#define MOVES 9

static const uint8_t src[9][7] = {
    {1,4,2,0,3,5,6},
    {4,3,2,1,0,5,6},
    {3,0,2,4,1,5,6},
    {0,1,2,4,5,6,3},
    {0,1,2,5,6,3,4},
    {0,1,2,6,3,4,5},
    {0,2,5,3,1,4,6},
    {0,5,4,3,2,1,6},
    {0,4,1,3,5,2,6}
};

static const int fact[7] = {
    720,120,24,6,2,1,1
};

static uint16_t trans[STATES][MOVES];

static void unrank(int x, uint8_t p[7])
{
    uint8_t a[7] = {0,1,2,3,4,5,6};

    for (int i = 0; i < 7; i++) {
        int k = x / fact[i];
        x %= fact[i];

        p[i] = a[k];

        for (int j = k; j < 6-i; j++)
            a[j] = a[j+1];
    }
}

static int rank(const uint8_t p[7])
{
    int r = 0;

    for (int i = 0; i < 7; i++) {
        int c = 0;

        for (int j = i+1; j < 7; j++)
            if (p[j] < p[i])
                c++;

        r += c * fact[i];
    }

    return r;
}

int main(void)
{
    for (int s = 0; s < STATES; s++) {
        uint8_t p[7];
        unrank(s, p);

        if (rank(p) != s) {
            puts("Unrank validation FAILED");
            return 1;
        }

        for (int m = 0; m < MOVES; m++) {
            uint8_t q[7];

            for (int i = 0; i < 7; i++)
                q[i] = p[src[m][i]];

            trans[s][m] = (uint16_t)rank(q);
        }
    }

    const int inv[9] = {2,1,0,5,4,3,8,7,6};

    for (int s = 0; s < STATES; s++) {
        for (int m = 0; m < MOVES; m++) {
            int next = trans[s][m];

            if (trans[next][inv[m]] != s) {
                puts("Inverse validation FAILED");
                return 1;
            }
        }
    }

    FILE *f = fopen("permutation_transition.bin", "wb");

    if (!f) {
        perror("fopen");
        return 1;
    }

    for (int s = 0; s < STATES; s++) {
        for (int m = 0; m < MOVES; m++) {
            int v = trans[s][m];

            fputc(v & 255, f);
            fputc(v >> 8, f);
        }
    }

    if (fclose(f) != 0) {
        perror("fclose");
        return 1;
    }

    puts("Permutation transition PASSED");
    puts("5040 states, 9 moves, 90720 bytes");

    return 0;
}
