#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct Game Game;
struct Game{
//	char* name;
	char name[200];
	int appid;
	int owners;
	int p2weeks;
};
