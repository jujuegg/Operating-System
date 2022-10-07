CC:=gcc
target:=read
target2:=write

all:
	$(CC) -c $(target).c
	$(CC) -o $(target) $(target).o
	$(CC) -c $(target2).c
	$(CC) -o $(target2) $(target2).o
dep:
	
clean:
	rm -rf $(target)
	rm -rf $(target).o
	rm -rf $(target2)
	rm -rf $(target2).o

