#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#define true 1
#define false 0;

static int stop = false;

void handler( int signum )
{
	stop = true;
}

int main( int argc, const char *argv[] )
{
    while(1)
    {
	int i ;
	signal(SIGINT, handler);
  
	if ( argc < 2 )
	{
	    printf("Please input filename\n");
	    exit(EXIT_FAILURE);
	}
  
	int fd = open(argv[1],O_RDONLY);
	if ( fd == -1 )
	{
		printf( "open file failed\n" );
	    exit(EXIT_FAILURE);
	}
    
    struct stat fileInfo = {0};
    
    if (fstat( fd, &fileInfo ) == -1)
    {
        printf("get file stat failed\n");
        exit(EXIT_FAILURE);
    }
    if (fileInfo.st_size == 0 )
    {
        printf("file is empty\n");
        exit( EXIT_FAILURE );
    }
    char *map = (char*)mmap(NULL, getpagesize(), PROT_READ, MAP_SHARED, fd, 0);
    if ( map == MAP_FAILED )
    {
        close(fd);
        printf("mmap failed\n");
        exit(EXIT_FAILURE);
    }
    
	printf("file:'%s':\n", argv[1]);
    	for (i=0;i<fileInfo.st_size;i++)
            printf("%c", map[i] );
	sleep(2);
    
    if (munmap( map, fileInfo.st_size ) == -1)
    {
        close(fd);
        printf("munmap failed\n");
        exit( EXIT_FAILURE );
    }
    close(fd);
    }
    return 0;
}
