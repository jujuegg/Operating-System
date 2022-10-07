#include<iostream>
#include<cstdlib>
#include<time.h>
#include<pthread.h>
#include<unistd.h>
#define N 5 //number of pholisophers
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2
using namespace std;

typedef pthread_mutex_t semaphore;
class Monitor
{
	private:
		int state[N];	//H or T or E
		semaphore mutex;	//mutex lock
		pthread_cond_t S[N];		//condition variable
	public:
		Monitor();
		void take_forks(int i);
		void put_forks(int i);
		void test(int i);
		void print_state();
};

void *philosopher(void *P);
void think();
void eat();
int* P_init();	//philosopher's number

Monitor M;

int main()
{
	srand(time(NULL));

	int *P=P_init();
	pthread_t thread[N];

	for(int i=0 ; i<N ; i++)	//thread name,attribute,function pointer,function's par
		pthread_create(&thread[i],NULL,philosopher,(void*)&P[i]);
	for(int i=0 ; i<N ; i++)
		pthread_join(thread[i],NULL);	//wait thread until finish

	return 0;
}

void *philosopher(void *P)	//*void means unknown parameter's type
{
	int i=*(int*)P;

	while(1)
	{
		think();
		M.take_forks(i);
		eat();
		M.put_forks(i);	
	}
}
Monitor::Monitor()
{
	pthread_mutex_init(&mutex,NULL);	//initial mutex
	for(int i=0 ; i<N ; i++)
	{
		pthread_cond_init(&S[i],NULL);
		state[i]=THINKING;
	}
}
void Monitor::take_forks(int i)
{
	pthread_mutex_lock(&mutex);		//enter C.S.
	state[i]=HUNGRY;
	print_state();
	test(i);	//test if can eat
	pthread_mutex_unlock(&mutex);	//leave C.S.
}
void Monitor::put_forks(int i)
{
	pthread_mutex_lock(&mutex);		//enter C.S.
	state[i]=THINKING;	//finish eating
	print_state();
	pthread_cond_signal(&S[LEFT]);	//wake condition variable
	pthread_cond_signal(&S[RIGHT]);
	pthread_mutex_unlock(&mutex);	//leave C.S.
}
void Monitor::test(int i)
{
	while(!(state[i]==HUNGRY && state[LEFT]!=EATING && state[RIGHT]!=EATING))
		pthread_cond_wait(&S[i],&mutex);//stuck here and release mutex
	state[i]=EATING;	
	print_state();
}
void Monitor::print_state()
{
	cout<<"current states: \n";
	for(int i=0 ; i<N ; i++)
		cout<<i<<" ";
	cout<<"\n";
	for(int i=0 ; i<N ; i++)
	{
		if(state[i]==THINKING)
			cout<<"T ";
		else if(state[i]==HUNGRY)
			cout<<"H ";
		else if(state[i]==EATING)
			cout<<"E ";
	}
	cout<<"\n";
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
int* P_init()
{
	int *P=new int[N];
	for(int i=0 ; i<N ; i++)
		P[i]=i;
	
	return P;
}
