
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	if(argc<2)
	{
		printf(2,"llegal command or arguments");
		exit(-1);
	}
 // printf(2,"%s",argv[1]);
  procinfo(atoi(argv[1]));
  exit(0);
}
