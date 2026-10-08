#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CUBIES 7
#define MOVES 9
#define STATES 2187

static const uint8_t move_source[MOVES][CUBIES] = {
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

static const uint8_t move_twist[MOVES][CUBIES] = {
    {1,2,0,2,1,0,0},
    {0,0,0,0,0,0,0},
    {1,2,0,2,1,0,0},
    {0,0,0,1,2,1,2},
    {0,0,0,0,0,0,0},
    {0,0,0,1,2,1,2},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0}
};

static uint8_t dist[STATES];
static uint16_t queue[STATES];

static void decode(int index, uint8_t o[CUBIES])
{
    for (int i = 0; i < CUBIES; ++i) {
        o[i] = (uint8_t)(index % 3);
        index /= 3;
    }
}

static int encode(const uint8_t o[CUBIES])
{
    int index = 0;

    for (int i = CUBIES - 1; i >= 0; --i)
        index = index * 3 + o[i];

    return index;
}

int main(void)
{
    memset(dist, 255, sizeof(dist));

    int head = 0;
    int tail = 0;
    int max_depth = 0;

    dist[0] = 0;
    queue[tail++] = 0;

    while (head < tail) {
        int state = queue[head++];

        uint8_t old_o[CUBIES];
        decode(state, old_o);

        for (int m = 0; m < MOVES; ++m) {
            uint8_t next_o[CUBIES];

            for (int i = 0; i < CUBIES; ++i) {
                int src = move_source[m][i];

                next_o[i] =
                    (uint8_t)((old_o[src] + move_twist[m][i]) % 3);
            }

            int next = encode(next_o);

            if (dist[next] != 255)
                continue;

            dist[next] = (uint8_t)(dist[state] + 1);

            if (dist[next] > max_depth)
                max_depth = dist[next];

            if (tail >= STATES) {
                fprintf(stderr, "Queue overflow\n");
                return 1;
            }

            queue[tail++] = (uint16_t)next;
        }
    }

    FILE *fp = fopen("orientation_pdb.bin", "wb");

    if (!fp) {
        perror("orientation_pdb.bin");
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

    printf("=== Orientation PDB ===\n");
    printf("Table entries : %d\n", STATES);
    printf("Reachable     : %d\n", tail);
    printf("Max distance  : %d\n", max_depth);
    printf("Table bytes   : %zu\n", sizeof(dist));
    printf("Goal distance : %u\n", dist[0]);

    if (tail != 729 || dist[0] != 0) {
        fprintf(stderr, "PDB validation failed!\n");
        return 1;
    }

    printf("PDB generation PASSED!\n");
    return 0;
}
