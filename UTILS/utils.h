#ifndef _UTILS
#define _UTILS

int LoadImage(FILE * img);
int LoadBPB(FILE * img);
int PrintBootSectInfo();
int SetRootDir();
struct cluster FindClusterInfo(unsigned int cluster);

unsigned int setclus(struct directory * dir);
void nametofat(char * name);

#endif
