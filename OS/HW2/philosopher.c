#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<unistd.h>
#define N 5 //number of pholisophers
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

typedef pthread_mutex_t semaphore;
int state[N];	//H or T or E
semaphore mutex;	//mutex lock
semaphore S[N];		//want to eat

void philosopher(void *P);
void take_forks(int i);
void put_forks(int i);
void test(int i);
void think();
void eat();
void print_state();
int* P_init();	//philosopher's number

int main()
{
	srand(time(NULL));

	int *P=P_init();
	pthread_t thread[N];

	pthread_mutex_init(&mutex,NULL);	//initial mutex
	for(int i=0 ; i<N ; i++)	//thread name,attribute,function pointer,function's par
		pthread_create(&thread[i],NULL,(void*)&philosopher,(void*)&P[i]);
	for(int i=0 ; i<N ; i++)
		pthread_join(thread[i],NULL);	//wait thread until finish

	return 0;
}
void philosopher(void *P)	//*void means unknown parameter's type
{
	int i=*(int*)P;

	while(1)
	{
		think();
		take_forks(i);
		eat();
		put_forks(i);	
	}
}
void take_forks(int i)
{
	pthread_mutex_lock(&mutex);		//enter C.S.
	state[i]=HUNGRY;
	print_state();
	test(i);	//test if can eat
	pthread_mutex_unlock(&mutex);	//leave C.S.
	pthread_mutex_lock(&S[i]);	//if can't eat sleep here until someone wake you up
}
void put_forks(int i)
{
	pthread_mutex_lock(&mutex);		//enter C.S.
	state[i]=THINKING;	//finish eating
	print_state();
	test(LEFT);	//tell left and right they can take his fork
	test(RIGHT);
	pthread_mutex_unlock(&mutex);	//leave C.S.
}
void test(int i)
{	//if hungry and left and right are not eating 
	if(state[i]==HUNGRY && state[LEFT]!=EATING && state[RIGHT]!=EATING)
	{
		state[i]=EATING;	
		print_state();
		pthread_mutex_unlock(&S[i]);	//ready to eat -> let take_forks finish and eat
	}
}
void think()
{
	int time=(rand()%5)+1;	//sleep 1~5 secs
	sleep(time);
}
void eat()
{
	int time=(rand()%5)+1;	//eat 1~5 secs
	sleep(time);
}
void print_state()
{
	printf("current states: \n");
	for(int i=0 ; i<N ; i++)
		printf("%d ",i);
	printf("\n");
	for(int i=0 ; i<N ; i++)
	{
		if(state[i]==THINKING)
			printf("T ");
		else if(state[i]==HUNGRY)
			printf("H ");
		else if(state[i]==EATING)
			printf("E ");
	}
	printf("\n");
}
int* P_init()
{
	int *P=malloc(N*sizeof(int));
	for(int i=0 ; i<N ; i++)
		P[i]=i;
	
	return P;
}
