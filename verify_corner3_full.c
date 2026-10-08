/* Check the new 3-corner abstraction against the exact BFS distance
 * for every one of the 3,674,160 reachable cube states.
 * Run after gen_orientation_fast and gen_corner3 0 1 2.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NP 5040
#define NO 729
#define FULL 3674160u
#define N3 5670
static uint16_t ptr[NP][9],otr[NO][9];
static uint8_t *dist;
static uint32_t *queue;
static uint8_t corner_pdb[N3];
static uint8_t pos_map[NP][2];
static void die(const char *message){fprintf(stderr,"FAIL: %s\n",message);exit(1);}
static void read_exact(const char *file,void *data,size_t size){
 FILE *f=fopen(file,"rb");if(!f){perror(file);exit(1);}
 if(fread(data,1,size,f)!=size||fgetc(f)!=EOF)die("invalid table size");
 if(fclose(f)!=0)die("close failed");
}
static void load_u16(const char *file,uint16_t *buf,unsigned n){
 FILE *f=fopen(file,"rb");if(!f){perror(file);exit(1);}
 for(unsigned i=0;i<n;i++){int lo=fgetc(f),hi=fgetc(f);if(lo<0||hi<0)die("short transition table");buf[i]=(uint16_t)(lo|hi<<8);}
 if(fgetc(f)!=EOF)die("transition table has trailing data");
 if(fclose(f)!=0)die("close failed");
}
static void orientation(unsigned rank,uint8_t o[7]){
 unsigned sum=0;
 for(int i=0;i<6;i++){o[i]=(uint8_t)(rank%3);sum+=o[i];rank/=3;}
 o[6]=(uint8_t)((3-sum%3)%3);
}
static unsigned position_rank(unsigned p0,unsigned p1,unsigned p2){
 return p0*30u+(p1-(p1>p0))*5u+p2-(p2>p0)-(p2>p1);
}
static void verify_positions(void){
 int facts[7]={720,120,24,6,2,1,1};
 for(int r=0;r<NP;r++){
  uint8_t p[7],pool[7]={0,1,2,3,4,5,6};int x=r;
  for(int i=0;i<7;i++){
   int k=x/facts[i];x%=facts[i];p[i]=pool[k];
   for(int j=k;j<6-i;j++)pool[j]=pool[j+1];
  }
  unsigned pos[3];
  for(int i=0;i<3;i++){
   unsigned found=0;
   for(int j=0;j<7;j++)if(p[j]==i){pos[i]=(unsigned)j;found++;}
   if(found!=1)die("permutation contains duplicate or missing cubie");
  }
  unsigned packed=(unsigned)pos_map[r][0]+256u*pos_map[r][1];
  if(packed!=(pos[0]|pos[1]<<3|pos[2]<<6))die("packed positions disagree with permutation");
 }
}
int main(void){
 load_u16("permutation_transition.bin",&ptr[0][0],NP*9);
 load_u16("orientation_transition.bin",&otr[0][0],NO*9);
 read_exact("corner3_pdb.bin",corner_pdb,sizeof corner_pdb);
 read_exact("corner3_positions.bin",pos_map,sizeof pos_map);
 verify_positions();
 dist=malloc(FULL);queue=malloc((size_t)FULL*sizeof(*queue));if(!dist||!queue)die("out of memory");
 memset(dist,255,FULL);dist[0]=0;queue[0]=0;unsigned head=0,tail=1;
 while(head<tail){
  uint32_t s=queue[head++];unsigned pr=s/NO,orr=s%NO;
  for(int m=0;m<9;m++){
   unsigned q=(unsigned)ptr[pr][m]*NO+otr[orr][m];
   if(q>=FULL)die("transition index invalid");
   if(dist[q]==255){
    dist[q]=dist[s]+1;
    if(tail>=FULL)die("BFS queue overflow");
    queue[tail++]=q;
   }
  }
 }
 if(tail!=FULL)die("BFS did not visit the whole cube state space");
 uint8_t projected_min[N3];memset(projected_min,255,sizeof projected_min);
 unsigned depth11=0,excess=0;
 for(unsigned pr=0;pr<NP;pr++){
  unsigned packed=(unsigned)pos_map[pr][0]+256u*pos_map[pr][1];
  unsigned p0=packed&7u,p1=(packed>>3)&7u,p2=(packed>>6)&7u;
  if(p0>=7||p1>=7||p2>=7||p0==p1||p0==p2||p1==p2)die("bad position triple");
  unsigned first=position_rank(p0,p1,p2)*27u;
  for(unsigned orr=0;orr<NO;orr++){
   uint8_t o[7];orientation(orr,o);
   unsigned idx=first+o[p0]*9u+o[p1]*3u+o[p2];
   unsigned fullidx=pr*NO+orr;
   if(idx>=N3)die("pattern index invalid");
   uint8_t h=corner_pdb[idx],d=dist[fullidx];
   if(h==255||h>d){fprintf(stderr,"FAIL state=%u pattern=%u h=%u d=%u\n",fullidx,idx,h,d);exit(1);}
   if(d<projected_min[idx])projected_min[idx]=d;
   if(d==11)depth11++;
   if(h>0)excess++;
  }
 }
 for(int i=0;i<N3;i++)if(projected_min[i]!=corner_pdb[i])die("3-corner PDB differs from projected exact full BFS minima");
 if(depth11!=2644)die("distance-11 count mismatch");
 printf("CORNER3 H1 PASS: %u full states checked; no heuristic overestimation\n",FULL);
 printf("CORNER3 H2 PASS: all %d abstract entries equal projected full BFS minima\n",N3);
 printf("FULL BFS diameter 11, distance-11 states %u, nonzero pattern heuristic states %u\n",depth11,excess);
 free(dist);free(queue);return 0;
}
