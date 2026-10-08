/* Generate 3-corner combined position+orientation heuristic (7P3 * 3^3).
 * Usage: ./gen_corner3 [cubie0 cubie1 cubie2]   defaults 0 1 2
 * Output: corner3_pdb.bin (5670 bytes), corner3_positions.bin (10080 bytes)
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define NS 5670
static const uint8_t src[9][7]={{1,4,2,0,3,5,6},{4,3,2,1,0,5,6},{3,0,2,4,1,5,6},{0,1,2,4,5,6,3},{0,1,2,5,6,3,4},{0,1,2,6,3,4,5},{0,2,5,3,1,4,6},{0,5,4,3,2,1,6},{0,4,1,3,5,2,6}};
static const uint8_t twist[9][7]={{1,2,0,2,1,0,0},{0,0,0,0,0,0,0},{1,2,0,2,1,0,0},{0,0,0,1,2,1,2},{0,0,0,0,0,0,0},{0,0,0,1,2,1,2},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};
static const int fact[7]={720,120,24,6,2,1,1};
static uint8_t dist[NS];static uint16_t queue[NS];static uint8_t dest[9][7];
static void fail(const char *msg){fprintf(stderr,"FAIL %s\n",msg);exit(1);}
static unsigned posrank(const uint8_t p[3]){
 return (unsigned)p[0]*30u+(unsigned)(p[1]-(p[1]>p[0]))*5u+
     (unsigned)(p[2]-(p[2]>p[0])-(p[2]>p[1]));
}
static unsigned rank(const uint8_t p[3],const uint8_t o[3]){
 return posrank(p)*27u+o[0]*9u+o[1]*3u+o[2];
}
static void unpackpos(unsigned x,uint8_t p[3]){
 unsigned a=x/30;x%=30;unsigned b=x/5;x%=5;
 p[0]=(uint8_t)a;
 p[1]=(uint8_t)(b>=a?b+1:b);
 unsigned c=x;
 for(unsigned j=0;j<7;j++)if(j!=p[0]&&j!=p[1]){
  if(c==0){p[2]=(uint8_t)j;return;}c--;
 }
 fail("invalid abstract position rank");
}
static void perm_unrank(unsigned rank,uint8_t p[7]){
 uint8_t avail[7]={0,1,2,3,4,5,6};
 for(int i=0;i<7;i++){
  int k=rank/fact[i];rank%=fact[i];p[i]=avail[k];
  for(int j=k;j<6-i;j++)avail[j]=avail[j+1];
 }
}
int main(int argc,char **argv){
 uint8_t wanted[3]={0,1,2};
 if(argc!=1&&argc!=4)fail("Usage: gen_corner3 [cubie0 cubie1 cubie2]");
 if(argc==4)for(int j=0;j<3;j++){int x=atoi(argv[j+1]);if(x<0||x>=7)fail("invalid corner number");wanted[j]=(uint8_t)x;}
 if(wanted[0]==wanted[1]||wanted[0]==wanted[2]||wanted[1]==wanted[2])fail("duplicate corners");
 for(int m=0;m<9;m++)for(int old=0;old<7;old++){
  int found=0;for(int pos=0;pos<7;pos++)if(src[m][pos]==old){dest[m][old]=(uint8_t)pos;found++;}
  if(found!=1)fail("move source invalid");
 }
 memset(dist,255,sizeof dist);
 uint8_t ori0[3]={0};
 unsigned goal=rank(wanted,ori0);
 unsigned head=0,tail=0;dist[goal]=0;queue[tail++]=(uint16_t)goal;
 while(head<tail){
  unsigned x=queue[head++];uint8_t p[3],o[3];
  unpackpos(x/27,p);unsigned rem=x%27;o[0]=(uint8_t)(rem/9);o[1]=(uint8_t)(rem/3%3);o[2]=(uint8_t)(rem%3);
  if(rank(p,o)!=x)fail("rank decode mismatch");
  for(int m=0;m<9;m++){
   uint8_t q[3],w[3];
   for(int i=0;i<3;i++){q[i]=dest[m][p[i]];w[i]=(uint8_t)((o[i]+twist[m][q[i]])%3);}
   unsigned y=rank(q,w);if(y>=NS)fail("abstract rank out of range");
   if(dist[y]==255){dist[y]=dist[x]+1;if(tail>=NS)fail("queue overflow");queue[tail++]=(uint16_t)y;}
  }
 }
 if(tail!=NS||dist[goal]!=0)fail("BFS incomplete");
 int inv[9]={2,1,0,5,4,3,8,7,6};unsigned maxd=0;
 for(unsigned x=0;x<NS;x++){
  if(dist[x]>maxd)maxd=dist[x];
  uint8_t p[3],o[3];unpackpos(x/27,p);unsigned rem=x%27;o[0]=rem/9;o[1]=(rem/3)%3;o[2]=rem%3;
  for(int m=0;m<9;m++){
   uint8_t q[3],w[3],z[3],v[3];for(int i=0;i<3;i++){
    q[i]=dest[m][p[i]];w[i]=(o[i]+twist[m][q[i]])%3;
    z[i]=dest[inv[m]][q[i]];v[i]=(w[i]+twist[inv[m]][z[i]])%3;
   }
   unsigned y=rank(q,w);
   if(rank(z,v)!=x||dist[x]>dist[y]+1||dist[y]>dist[x]+1)fail("inverse/distance consistency");
  }
 }
 FILE *f=fopen("corner3_pdb.bin","wb");if(!f){perror("corner3_pdb.bin");return 1;}
 if (fwrite(dist,1,sizeof dist,f)!=sizeof dist) fail("write PDB");
 if (fclose(f)) fail("close PDB");
 f=fopen("corner3_positions.bin","wb");if(!f){perror("corner3_positions.bin");return 1;}
 for(unsigned r=0;r<5040;r++){
  uint8_t p[7],positions[3]={0};perm_unrank(r,p);
  for(int j=0;j<3;j++){
   int found=0;for(int i=0;i<7;i++)if(p[i]==wanted[j]){positions[j]=(uint8_t)i;found++;}
   if(found!=1)fail("missing cubie in permutation");
  }
  unsigned pack=positions[0]|(positions[1]<<3)|(positions[2]<<6);
  if(fputc(pack&255,f)==EOF||fputc(pack>>8,f)==EOF)fail("write positions");
 }
 if(fclose(f))fail("close positions");
 printf("CORNER3 TABLES PASS: corners %d,%d,%d, abstract states=%u, maximum distance=%u\n",wanted[0],wanted[1],wanted[2],tail,maxd);
 puts("corner3_pdb.bin = 5670 bytes; corner3_positions.bin = 10080 bytes");
 return 0;
}
