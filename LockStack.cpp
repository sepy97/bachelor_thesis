#include <iostream>
#include <thread>
#include <mutex>
#include <x86intrin.h>

#define INIT_PUSH 1000000
#define MAX_THREAD_NUM 100
#define MAX_VOLUME 10000000

class FastRandom {
private:
	unsigned long long rnd;
public:
	FastRandom(unsigned long long seed) { //time + threadnum
		rnd = seed;
	}
	unsigned long long rand() {
		rnd ^= rnd << 21;
		rnd ^= rnd >> 35;
		rnd ^= rnd << 4;
		return rnd;
	}
};

class LockStack {
public:
	LockStack() :head(NULL) {}
	
	void push(int d)
	{
		Node *pv = new Node;
		
		m.lock ();
		
		pv->d = d;
		pv->p = head;
		head = pv;
		
		m.unlock ();
	}
	int pop()
	{
		if (head == NULL) return NULL;
		/*if (isEmpty())
		{
   			return -1;
		}*/
		m.lock ();
		
		int temp = head->d;
		Node *pv = head;
		head = head->p;
		
		m.unlock ();
		
		delete pv;
		return temp;
	}
	bool isEmpty()
	{
		return (this->head == NULL);
		
	}
	void display();

private:
	struct Node {
		int d;
		Node *p;
	};
	
	Node *head;
	std::mutex m;
};

void testPush (LockStack* toTest, int data)
{
	toTest->push (data);
}

void testStack (LockStack* toTest, const int volume, int threadNum)
{
	FastRandom* ran = new FastRandom (time(NULL) + threadNum);
	for (int i = 0; i < volume; i++)
	{
		int pushOrPop = i%2;//(int)time(NULL);///100000;
		//pushOrPop %= 2;
		if (pushOrPop)
		{
			toTest->push (ran->rand()%volume);
		}
		else
		{
			toTest->pop ();
		}
	}
}

int main (int argc, char** argv)
{
	LockStack toTest;
	int maxThreads = 0;
	
	if (argc > 1)
	{
		maxThreads = atoi(argv[1]);
	}
	else
	{
		printf ("no arguments :( \n");
		return 0;
	}
	
	std::thread thr[maxThreads];
	
	FastRandom* ran = new FastRandom (__rdtsc ()/1000000000);
	
	for (int i = 0; i < INIT_PUSH; i++)
	{
		toTest.push (ran->rand() % INIT_PUSH);
	}
	
	uint64_t tick = __rdtsc ()/100000;
	
	for (int i = 0; i < maxThreads; i++)
	{
		//toTest.push (i);
		thr[i] = std::thread (testStack, &toTest, MAX_VOLUME/maxThreads, i); //testPush, &toTest, i);
	}
	
	for (int i = 0; i < maxThreads; i++)
	{
		thr[i].join ();
	}
	
	uint64_t tick2 = __rdtsc ()/100000;
	printf ("%llu\n", tick2 - tick);
	
	return 0;
}
