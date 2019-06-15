#include <cstdlib>
#include <atomic>
#include <thread>
#include <cstdio>
#include <immintrin.h>
#include <x86intrin.h>

#define INIT_PUSH 10000
#define MAX_THREAD_NUM 10
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

class RTMStack {
public:
	RTMStack() :head(NULL) {}
//    ~RTMStack();
	void push(int d)
	{
		Node *pv = new Node;
		pv->d = 0;
		unsigned status = _xbegin ();
		/*while (status != _XBEGIN_STARTED)
		{
			status = _xbegin ();
		}*/
		if (status == _XBEGIN_STARTED)
		{
			pv->d = d;
			pv->p = head;
			head = pv;
			_xend ();
		}
		
	}
	int pop()
	{
		if (head == NULL) return NULL;
		/*if (isEmpty())
		{
   			return -1;
		}*/
		
		int temp = -1;
		Node *pv = NULL;
		unsigned status = _xbegin ();
		/*while (status != _XBEGIN_STARTED)
		{
			status = _xbegin ();
		}*/
		if (status == _XBEGIN_STARTED)
		{
			temp = head->d;
			pv = head;
			head = head->p;
			_xend();
		}
		
		if (pv != NULL)	delete pv;
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
};

void testPush (RTMStack* toTest, int volume, int threadNum)
{
	FastRandom* ran = new FastRandom (time(NULL) + threadNum);
	for (int i = 0; i < volume; i++)
	{
		int pushOrPop = ran->rand()%2;
		if (pushOrPop)
		{
			toTest->push (ran->rand()%volume);
		}
		else
		{
			toTest->pop ();
		}
	}
	//toTest->push (data);
}

int main (int argc, char** argv)
{
	RTMStack testStack;
	
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
		testStack.push (ran->rand() % INIT_PUSH);
	}
	
	uint64_t tick = __rdtsc ()/100000;
	
	for (int i = 0; i < maxThreads; i++)
	{
		thr[i] = std::thread (testPush, &testStack, MAX_VOLUME/maxThreads, i);
	}
	
	for (int i = 0; i < maxThreads; i++)
	{
		thr[i].join ();
	}
	
	uint64_t tick2 = __rdtsc ()/100000;
	printf ("%d, %llu, \n", maxThreads, tick2 - tick);

}
