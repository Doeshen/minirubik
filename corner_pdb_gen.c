#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define C 7
#define M 9
#define STATES 147

static const uint8_t src[M][C] = {
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

static const uint8_t twist[M][C] = {
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

int main(void)
{
    memset(dist, 255, sizeof(dist));

    int max_depth = 0;

    for (int cubie = 0; cubie < C; cubie++) {
        int queue[21];
        int head = 0, tail = 0;

        int goal = cubie * 3;
        dist[cubie * 21 + goal] = 0;
        queue[tail++] = goal;

        while (head < tail) {
            int state = queue[head++];
            int pos = state / 3;
            int ori = state % 3;

            for (int m = 0; m < M; m++) {
                int next_pos = -1;

                for (int i = 0; i < C; i++) {
                    if (src[m][i] == pos) {
                        next_pos = i;
                        break;
                    }
                }

                if (next_pos < 0) {
                    puts("Invalid transition");
                    return 1;
                }

                int next_ori =
                    (ori + twist[m][next_pos]) % 3;

                int next_state = next_pos * 3 + next_ori;
                int index = cubie * 21 + next_state;

                if (dist[index] != 255)
                    continue;

                if (tail >= 21) {
                    puts("Queue overflow");
                    return 1;
                }

                dist[index] =
                    (uint8_t)(dist[cubie * 21 + state] + 1);

                if (dist[index] > max_depth)
                    max_depth = dist[index];

                queue[tail++] = next_state;
            }
        }

        if (tail != 21) {
            printf("Cubie %d incomplete: %d\n", cubie, tail);
            return 1;
        }
    }

    for (int cubie = 0; cubie < C; cubie++) {
        for (int pos = 0; pos < C; pos++) {
            for (int ori = 0; ori < 3; ori++) {
                int old_index = cubie * 21 + pos * 3 + ori;

                for (int m = 0; m < M; m++) {
                    int next_pos = -1;

                    for (int i = 0; i < C; i++)
                        if (src[m][i] == pos)
                            next_pos = i;

                    int next_ori =
                        (ori + twist[m][next_pos]) % 3;

                    int new_index =
                        cubie * 21 + next_pos * 3 + next_ori;

                    int d0 = dist[old_index];
                    int d1 = dist[new_index];

                    if (d0 > d1 + 1 || d1 > d0 + 1) {
                        puts("Distance validation FAILED");
                        return 1;
                    }
                }
            }
        }
    }

    FILE *f = fopen("corner_pdb.bin", "wb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    if (fwrite(dist, 1, sizeof(dist), f) != sizeof(dist)) {
        perror("fwrite");
        fclose(f);
        return 1;
    }

    if (fclose(f) != 0) {
        perror("fclose");
        return 1;
    }

    printf("States       : %d\n", STATES);
    printf("Max distance : %d\n", max_depth);
    printf("Table bytes  : %zu\n", sizeof(dist));
    puts("Corner PDB PASSED");

    return 0;
}
