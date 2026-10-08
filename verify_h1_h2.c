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
static void verify(void){
 uint8_t perm_min[NP],orient_min[DENSE_O],corner_min[CORNER];
 memset(perm_min,255,sizeof perm_min);
 memset(orient_min,255,sizeof orient_min);
 memset(corner_min,255,sizeof corner_min);
 unsigned int maxhp=0,maxho=0,maxhc=0,maxh=0;
 unsigned int full_checked=0;
 for(int p=0;p<NP;p++){
  for(int o=0;o<NO;o++){
   uint8_t d=full_dist[(uint32_t)p*NO+o];
   if(d==255)die("unvisited state");
   int index=orient_to_dense[o];
   uint8_t hp=pdbp[p],ho=pdbo[index];
   if(hp==255||ho==255)die("PDB contains sentinel in reachable state");
   if(hp>d||ho>d){fprintf(stderr,"PDB overestimates p=%d o=%d d=%u hp=%u ho=%u\n",p,o,d,hp,ho);die("H1 violated");}
   if(d<perm_min[p])perm_min[p]=d;
   if(d<orient_min[index])orient_min[index]=d;
   unsigned int sum=0,bad=0,any_two=0;
   for(int position=0;position<7;position++){
    int cubie=perm[p][position],orientation=ori[o][position];
    int k=cubie*21+position*3+orientation;
    uint8_t cd=pdbc[k];
    if(cd==255||cd>d)die("corner PDB invalid/overestimate");
    if(d<corner_min[k])corner_min[k]=d;
    sum+=cd;
    bad+=(orientation!=0);
    any_two+=(cd==2);
   }
   unsigned int hc=(sum+3)/4;
   if(hc<(bad+3)/4)hc=(bad+3)/4;
   if(any_two && hc<2)hc=2;
   unsigned int h=hp>ho?hp:ho;
   if(h<hc)h=hc;
   if(h>d){fprintf(stderr,"Heuristic overestimates p=%d o=%d d=%u h=%u\n",p,o,d,h);die("H1 violated");}
   if(hp>maxhp)maxhp=hp;
   if(ho>maxho)maxho=ho;
   if(hc>maxhc)maxhc=hc;
   if(h>maxh)maxh=h;
   full_checked++;
  }
 }
 for(int i=0;i<NP;i++)if(perm_min[i]!=pdbp[i])die("permutation PDB does not equal projected full-BFS minimum");
 for(int i=0;i<DENSE_O;i++){
  if(dense_to_orient[i]>=0){if(orient_min[i]!=pdbo[i])die("orientation PDB does not equal projected full-BFS minimum");}
  else if(pdbo[i]!=255)die("unreachable orientation table entry should be 255");
 }
 for(int i=0;i<CORNER;i++)if(corner_min[i]!=pdbc[i])die("corner PDB does not equal projected full-BFS minimum");
 if(pdbp[0]!=0||pdbo[0]!=0)die("PDB solved distances not zero");
 for(int c=0;c<7;c++)if(pdbc[c*21+c*3]!=0)die("corner PDB solved distance not zero");
 printf("H1 PASS: admissibility checked for %u complete cube states\n",full_checked);
 printf("H2 PASS: all 5040 permutation + 729 valid orientation + 147 corner entries equal full-BFS projected minima\n");
 printf("H2 PASS: all %d permutation transitions independently verified\n",NP*NM);
 printf("Observed heuristic maxima: perm=%u orient=%u corner=%u combined=%u\n",maxhp,maxho,maxhc,maxh);
 puts("H3 NOT TESTED: full-domain assembly solver optimality is a separate test");
 puts("H4: not applicable to the four uncompressed files checked here");
}
int main(void){setup();puts("All four .bin files loaded and transition encodings validated");bfs();verify();free(full_dist);free(queue);puts("VERIFICATION PASSED");return 0;}
