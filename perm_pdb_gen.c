#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define N 7
#define MOVES 9
#define STATES 5040

static const uint8_t source[MOVES][N] = {
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

static const int fact[N] = {720,120,24,6,2,1,1};

static uint8_t dist[STATES];
static uint16_t queue[STATES];

static int rank_perm(const uint8_t p[N])
{
    int rank = 0;
    for (int i = 0; i < N; ++i) {
        int smaller = 0;
        for (int j = i + 1; j < N; ++j)
            if (p[j] < p[i])
                smaller++;
        rank += smaller * fact[i];
    }
    return rank;
}

static void unrank_perm(int rank, uint8_t p[N])
{
    uint8_t available[N] = {0,1,2,3,4,5,6};

    for (int i = 0; i < N; ++i) {
        int k = rank / fact[i];
        rank %= fact[i];

        p[i] = available[k];

        for (int j = k; j < N - i - 1; ++j)
            available[j] = available[j + 1];
    }
}

int main(void)
{
    memset(dist, 255, sizeof(dist));

    int head = 0, tail = 0, max_depth = 0;
    dist[0] = 0;
    queue[tail++] = 0;

    while (head < tail) {
        int state = queue[head++];

        uint8_t p[N];
        unrank_perm(state, p);

        if (rank_perm(p) != state) {
            fprintf(stderr, "Rank validation FAILED!\n");
            return 1;
        }

        for (int m = 0; m < MOVES; ++m) {
            uint8_t next_p[N];
            for (int i = 0; i < N; ++i)
                next_p[i] = p[source[m][i]];

            int next = rank_perm(next_p);

            if (dist[next] != 255)
                continue;

            dist[next] = (uint8_t)(dist[state] + 1);
            if (dist[next] > max_depth)
                max_depth = dist[next];

            if (tail >= STATES) {
                fprintf(stderr, "Queue overflow!\n");
                return 1;
            }

            queue[tail++] = (uint16_t)next;
        }
    }

    printf("=== Permutation PDB ===\n");
    printf("Table entries : %d\n", STATES);
    printf("Reachable     : %d\n", tail);
    printf("Max distance  : %d\n", max_depth);
    printf("Table bytes   : %zu\n", sizeof(dist));
    printf("Goal distance : %u\n", dist[0]);

    if (tail != STATES || dist[0] != 0) {
        fprintf(stderr, "PDB validation FAILED!\n");
        return 1;
    }

    FILE *fp = fopen("permutation_pdb.bin", "wb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    if (fwrite(dist, 1, sizeof(dist), fp) != sizeof(dist)) {
        perror("fwrite");
        fclose(fp);
        return 1;
    }

    if (fclose(fp) != 0) {
        perror("fclose");
        return 1;
    }

    puts("PDB generation PASSED!");
    return 0;
}
