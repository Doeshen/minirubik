/* Generate two small read-only tables for RV32I early orientation pruning.
 * Input: orientation_pdb.bin (2187 bytes; same encoding as original solver)
 * Output: orientation_compact.bin (729 bytes)
 *         orientation_transition.bin (729*9*2 = 13122 bytes, LE uint16_t)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const uint8_t src[9][7] = {
    {1,4,2,0,3,5,6},{4,3,2,1,0,5,6},{3,0,2,4,1,5,6},
    {0,1,2,4,5,6,3},{0,1,2,5,6,3,4},{0,1,2,6,3,4,5},
    {0,2,5,3,1,4,6},{0,5,4,3,2,1,6},{0,4,1,3,5,2,6}
};
static const uint8_t twist[9][7] = {
    {1,2,0,2,1,0,0},{0,0,0,0,0,0,0},{1,2,0,2,1,0,0},
    {0,0,0,1,2,1,2},{0,0,0,0,0,0,0},{0,0,0,1,2,1,2},
    {0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0}
};
static unsigned short trans[729][9];
static uint8_t compact[729];
static uint8_t dense[2187];

static void fatal(const char *text) { fprintf(stderr, "ERROR: %s\n", text); exit(1); }
static void decode(unsigned index, uint8_t o[7]) {
    unsigned sum=0;
    for (int i=0;i<6;i++) { o[i]=(uint8_t)(index%3); index/=3; sum+=o[i]; }
    o[6]=(uint8_t)((3-sum%3)%3);
}
static unsigned encode(const uint8_t o[7], int n) {
    unsigned index=0;
    for (int i=n-1;i>=0;i--) index=3*index+o[i];
    return index;
}
int main(void) {
    FILE *f=fopen("orientation_pdb.bin", "rb");
    if (!f) { perror("orientation_pdb.bin"); return 1; }
    if (fread(dense,1,sizeof dense,f)!=sizeof dense || fgetc(f)!=EOF)
        fatal("orientation_pdb.bin must contain exactly 2187 bytes");
    if (fclose(f)) fatal("cannot close orientation_pdb.bin");
    unsigned maxdistance=0;
    for (unsigned state=0;state<729;state++) {
        uint8_t o[7]; decode(state,o);
        unsigned denseidx=encode(o,7);
        if (denseidx>=2187 || dense[denseidx]==255) fatal("missing valid orientation distance");
        compact[state]=dense[denseidx];
        if (compact[state]>maxdistance) maxdistance=compact[state];
        for (int m=0;m<9;m++) {
            uint8_t next[7]; unsigned sum=0;
            for(int j=0;j<7;j++) { next[j]=(uint8_t)((o[src[m][j]]+twist[m][j])%3); sum+=next[j]; }
            if (sum%3) fatal("orientation invariant broken");
            unsigned idx=encode(next,6);
            if (idx>=729) fatal("transition rank out of range");
            trans[state][m]=(uint16_t)idx;
        }
    }
    const int inv[9]={2,1,0,5,4,3,8,7,6};
    for (int s=0;s<729;s++) for (int m=0;m<9;m++) {
        unsigned next=trans[s][m];
        if (trans[next][inv[m]]!=s) fatal("inverse move identity mismatch");
        if (compact[s]>compact[next]+1 || compact[next]>compact[s]+1)
            fatal("orientation distance consistency failed");
    }
    if (compact[0]!=0 || maxdistance!=6) fatal("unexpected orientation PDB goal or maximum");
    f=fopen("orientation_compact.bin","wb");
    if(!f) { perror("orientation_compact.bin"); return 1; }
    if(fwrite(compact,1,sizeof compact,f)!=sizeof compact || fclose(f))
        fatal("orientation_compact.bin write failed");
    f=fopen("orientation_transition.bin","wb");
    if(!f) { perror("orientation_transition.bin"); return 1; }
    for(int s=0;s<729;s++) for(int m=0;m<9;m++) {
        unsigned v=trans[s][m];
        if (fputc((int)(v&255),f)==EOF || fputc((int)(v>>8),f)==EOF)
            fatal("orientation_transition.bin write failed");
    }
    if(fclose(f)) fatal("orientation_transition.bin close failed");
    puts("ORIENTATION FAST TABLES PASS: 729 states, 9 moves, all 6561 transitions checked");
    puts("orientation_compact.bin = 729 bytes");
    puts("orientation_transition.bin = 13122 bytes");
    return 0;
}
