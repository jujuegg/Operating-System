#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#define BUFFER_SIZE 100
#define true 1
#define false 0;

static int stop = false;

void handler( int signum )
{
	stop = true;
}

int main( int argc, char *argv[] )
{
	signal( SIGINT, handler);
	if(argc<2)
	{
	    printf( "Please type filename as argv[1]\n" );
	    exit( EXIT_FAILURE );
	}
	int fd = open( argv[1], O_RDWR | O_CREAT, 0666 );
	if( fd == -1 )
	{
	    printf( "open file failed\n" );
	    exit( EXIT_FAILURE );
	}
	char *map_ptr = ( char * )mmap( 0, getpagesize(), PROT_WRITE, MAP_SHARED, fd, 0 );
	if( map_ptr == MAP_FAILED )
	{
	    close( fd );
	    printf( "Error mmapping the file" );
	    exit( EXIT_FAILURE );
	}
	while ( true && !stop )
	{
	    struct stat fileinfo = {0};
	    fstat( fd, &fileinfo );
    
	    int current_size = ( int )fileinfo.st_size;
	    char buffer[BUFFER_SIZE];
	    printf( "type string to append into '%s':\n", argv[1] );
	  
	    fgets( buffer, sizeof( buffer ), stdin );
	    printf( "file updated\n" );
	    int text_len = strlen( buffer );
	    lseek( fd, current_size + text_len, SEEK_SET );
	    write( fd, "", 1 );
	    lseek( fd, 0, SEEK_SET );
	    if ( !stop )
		    sprintf( map_ptr + current_size, "%s", buffer );
	}
	if ( munmap( map_ptr, getpagesize() ) == -1 )
	{
	    close( fd );
	    printf( "Error un-mmapping the file\n" );
	    exit( EXIT_FAILURE );
	}
	close( fd );
	printf( "-----exit-----\n" );
	return 0;
}
