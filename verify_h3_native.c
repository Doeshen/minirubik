#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NP 5040
#define NO 729
#define FULL 3674160
#define NM 9
#define CORNER 147
#define DENSE_O 2187

static const unsigned char src[NM][7] = {
 {1,4,2,0,3,5,6},{4,3,2,1,0,5,6},{3,0,2,4,1,5,6},
 {0,1,2,4,5,6,3},{0,1,2,5,6,3,4},{0,1,2,6,3,4,5},
 {0,2,5,3,1,4,6},{0,5,4,3,2,1,6},{0,4,1,3,5,2,6}
};
static const unsigned char twist[NM][7] = {
 {1,2,0,2,1,0,0},{0,0,0,0,0,0,0},{1,2,0,2,1,0,0},
 {0,0,0,1,2,1,2},{0,0,0,0,0,0,0},{0,0,0,1,2,1,2},
 {0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}
};
static const int facts[7]={720,120,24,6,2,1,1};
static uint8_t pdbp[NP],pdbo[DENSE_O],pdbc[CORNER];
static uint16_t tr[NP][NM],otr[NO][NM],orient_to_dense[NO];
static int16_t dense_to_orient[DENSE_O];
static uint8_t perm[NP][7],ori[NO][7];
static uint8_t *full_dist;
static uint32_t *queue;

static void die(const char *msg){fprintf(stderr,"FAIL: %s\n",msg);exit(1);}
static void load_exact(const char *filename,void *buf,size_t n){
 FILE *f=fopen(filename,"rb");
 if(!f){perror(filename);exit(1);}
 if(fread(buf,1,n,f)!=n || fgetc(f)!=EOF){fprintf(stderr,"FAIL: %s size mismatch\n",filename);exit(1);}
 if(fclose(f))die("close failed");
}
static int rankp(const uint8_t p[7]){
 int rank=0;
 for(int i=0;i<7;i++){
  int smaller=0;
  for(int j=i+1;j<7;j++)smaller+=(p[j]<p[i]);
  rank+=smaller*facts[i];
 }
 return rank;
}
static void unrankp(int x,uint8_t p[7]){
 uint8_t available[7]={0,1,2,3,4,5,6};
 for(int i=0;i<7;i++){
  int a=x/facts[i];x%=facts[i];
  p[i]=available[a];
  for(int j=a;j<6-i;j++)available[j]=available[j+1];
 }
}
static int ranko_full(const uint8_t o[7]){
 int r=0,factor=1;
 for(int i=0;i<7;i++){r+=o[i]*factor;factor*=3;}
 return r;
}
static void setup(void){
 load_exact("permutation_pdb.bin",pdbp,sizeof pdbp);
 load_exact("orientation_pdb.bin",pdbo,sizeof pdbo);
 load_exact("corner_pdb.bin",pdbc,sizeof pdbc);
 unsigned char bytes[NP*NM*2];
 load_exact("permutation_transition.bin",bytes,sizeof bytes);
 for(int p=0;p<NP;p++){
  unrankp(p,perm[p]);
  if(rankp(perm[p])!=p)die("permutation unrank/rank mismatch");
  for(int m=0;m<NM;m++){
   int k=2*(p*NM+m);
   int next=bytes[k]+256*bytes[k+1];
   if(next<0||next>=NP)die("permutation transition out of bounds");
   tr[p][m]=(uint16_t)next;
   uint8_t q[7];
   for(int i=0;i<7;i++)q[i]=perm[p][src[m][i]];
   if(rankp(q)!=next)die("permutation transition incorrect");
  }
 }
 for(int i=0;i<DENSE_O;i++)dense_to_orient[i]=-1;
 for(int x=0;x<NO;x++){
  int v=x,sum=0;
  for(int i=0;i<6;i++){ori[x][i]=v%3;sum+=ori[x][i];v/=3;}
  ori[x][6]=(3-sum%3)%3;
  int full=ranko_full(ori[x]);
  if(full<0||full>=DENSE_O||dense_to_orient[full]!=-1)die("orientation encoding collision");
  dense_to_orient[full]=(int16_t)x;
  orient_to_dense[x]=(uint16_t)full;
 }
 for(int x=0;x<NO;x++){
  for(int m=0;m<NM;m++){
   uint8_t q[7];
   for(int i=0;i<7;i++)q[i]=(ori[x][src[m][i]]+twist[m][i])%3;
   int full=ranko_full(q);
   if(full<0||full>=DENSE_O||dense_to_orient[full]<0)die("orientation transition violates invariant");
   otr[x][m]=(uint16_t)dense_to_orient[full];
  }
 }
}
static void bfs(void){
 full_dist=malloc(FULL);
 queue=malloc((size_t)FULL*sizeof(*queue));
 if(!full_dist||!queue)die("out of memory");
 memset(full_dist,255,FULL);
 uint32_t head=0,tail=1;
 queue[0]=0;full_dist[0]=0;
 unsigned int hist[16]={0};hist[0]++;
 while(head<tail){
  uint32_t state=queue[head++];
  unsigned int p=state/NO,o=state%NO;
  for(int m=0;m<NM;m++){
   uint32_t next=(uint32_t)tr[p][m]*NO+otr[o][m];
   if(full_dist[next]==255){
    unsigned int d=full_dist[state]+1;
    if(d>=16)die("BFS exceeds histogram range");
    if(tail>=FULL)die("BFS queue overflow");
    full_dist[next]=(uint8_t)d;
    queue[tail++]=next;
    hist[d]++;
   }
  }
 }
 printf("BFS visited: %u / %d\n",tail,FULL);
 if(tail!=FULL)die("full-state BFS incomplete");
 int diameter=0;
 for(int d=0;d<16;d++)if(hist[d])diameter=d;
 printf("BFS diameter: %d\n",diameter);
 for(int d=0;d<=diameter;d++)printf("Distance %d: %u states\n",d,hist[d]);
 if(diameter!=11)die("incorrect full-state diameter");
 printf("Distance-11 states: %u\n",hist[11]);
}
/* H3: native model of the assembly's IDA* search, not an RV32I execution. */
static uint64_t nodes;
static uint8_t path[14];
static int foundlen;
static int heuristic_state(int p,int o){
 int h=pdbp[p]>pdbo[orient_to_dense[o]]?pdbp[p]:pdbo[orient_to_dense[o]];
 int sum=0,bad=0,two=0;
 for(int i=0;i<7;i++){
  int c=pdbc[perm[p][i]*21+i*3+ori[o][i]];
  sum+=c;bad+=(ori[o][i]!=0);two+=(c==2);
 }
 int k=(sum+3)/4; if((bad+3)/4>k)k=(bad+3)/4;
 if(two&&k<2)k=2;
 return h>k?h:k;
}
static int ida_model(int p,int o,int g,int bound,int prevface){
 ++nodes;
 if(p==0&&o==0){foundlen=g;return 1;}
 if(g==14)return 0;
 for(int m=0;m<9;m++){
  if(m/3==prevface)continue;
  int np=tr[p][m];
  if(g+1+pdbp[np]>bound)continue;
  int no=otr[o][m];
  int h=heuristic_state(np,no);
  if(g+1+h>bound)continue;
  path[g]=(uint8_t)m;
  if(ida_model(np,no,g+1,bound,m/3))return 1;
 }
 return 0;
}
static int solve_model(int p,int o){
 int lower=heuristic_state(p,o);
 for(int bound=lower;bound<=14;bound++){
  if(ida_model(p,o,0,bound,-1))return foundlen;
 }
 return -1;
}
static void verify_path(int p,int o,int d){
 for(int i=0;i<d;i++){
  int m=path[i];
  p=tr[p][m];o=otr[o][m];
 }
 if(p||o)die("H3 path replay did not reach goal");
}
static void verify_h3(int argc,char**argv){
 int mode=0;int maxcount=100;
 if(argc>=2){
  if(!strcmp(argv[1],"--all")){mode=1;maxcount=FULL;}
  else if(!strcmp(argv[1],"--distance11")){mode=2;maxcount=(argc>=3)?atoi(argv[2]):30;}
  else if(!strcmp(argv[1],"--quick")){mode=0;maxcount=(argc>=3)?atoi(argv[2]):100;}
  else {fprintf(stderr,"Usage: %s [--quick [N] | --distance11 [N] | --all]\n",argv[0]);exit(2);}
 }
 if(maxcount<=0)die("sample count must be positive");
 unsigned count[12]={0},tot=0;uint64_t totalnodes=0;
 for(uint32_t state=0;state<FULL;state++){
  int d=full_dist[state];
  if(mode==2&&d!=11)continue;
  if(mode==0&&count[d]>=(unsigned)maxcount)continue;
  if(mode==2&&tot>=(unsigned)maxcount)break;
  int p=state/NO,o=state%NO;
  nodes=0;
  int got=solve_model(p,o);
  if(got!=d){fprintf(stderr,"H3 FAIL state=%u p=%d o=%d expected=%d got=%d nodes=%llu\n",state,p,o,d,got,(unsigned long long)nodes);exit(1);}
  verify_path(p,o,got);
  count[d]++;tot++;totalnodes+=nodes;
  if((tot%100)==0){printf("H3 progress: %u cases, latest depth=%d total nodes=%llu\n",tot,d,(unsigned long long)totalnodes);fflush(stdout);}
  if(mode==0){int complete=1;for(int j=0;j<12;j++)if(count[j]<(unsigned)maxcount)complete=0;if(complete)break;}
 }
 for(int d=0;d<12;d++)if(count[d])printf("H3 distance %d: %u cases PASS\n",d,count[d]);
 printf("H3 NATIVE IDA MODEL PASS: %u states; expanded %llu nodes\n",tot,(unsigned long long)totalnodes);
 puts("NOTE: This checks a native C model; it does not establish assembly H3 correctness.");
}
static void export_distance11(void){
    FILE *f = fopen("distance11_states.txt", "w");
    if (!f) {
        perror("distance11_states.txt");
        exit(1);
    }

    unsigned count = 0;

    for (uint32_t state = 0; state < FULL; state++) {
        if (full_dist[state] != 11)
            continue;

        int p = state / NO;
        int o = state % NO;

        for (int i = 0; i < 7; i++)
            fputc('1' + perm[p][i], f);

        for (int i = 0; i < 7; i++)
            fputc('1' + ori[o][i], f);

        fputc('\n', f);
        count++;
    }

    if (fclose(f) != 0)
        die("distance11 output close failed");

    if (count != 2644)
        die("distance11 count mismatch");

    printf("EXPORT PASS: %u distance-11 states written\n", count);
}

int main(int argc,char **argv){
 setup();puts("PDBs/transitions loaded");
    bfs();
    if (argc == 2 && !strcmp(argv[1], "--export-distance11"))
        export_distance11();
    else
        verify_h3(argc, argv);

 free(full_dist);free(queue);return 0;
}
