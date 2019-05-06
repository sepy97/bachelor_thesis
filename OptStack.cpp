#include <cstdio>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <thread>
#include <x86intrin.h>

#define INIT_PUSH 10000
#define MAX_THREAD_NUM 10
#define MAX_VOLUME 10000000

class FastRandom {
private:
	unsigned long long rnd;
public:
	FastRandom(unsigned long long seed) {
		rnd = seed;
	}
	unsigned long long rand() {
		rnd ^= rnd << 21;
		rnd ^= rnd >> 35;
		rnd ^= rnd << 4;
		return rnd;
	}
};

struct LockFreeStack
{
	int data;
	LockFreeStack* next;
	LockFreeStack(const int& data) : data(data), next(nullptr) {}
};

class OptStack
{
	std::atomic<LockFreeStack*> head;

public:
	void push (const int& data)
	{
		LockFreeStack* newStack = new LockFreeStack (data);
		newStack->next = head.load ();
		
		while (!head.compare_exchange_strong (newStack->next, newStack))
			;
	}
	
	LockFreeStack* pop ()
	{
		if (head == nullptr)
		{
			printf ("Stack if EMPTY!!! You can not pop from it! \n");
			return 0;
		}
		LockFreeStack* newStack = head.load ();
		LockFreeStack* poppedNode = new LockFreeStack (newStack->data);
		while (!head.compare_exchange_strong (newStack, newStack->next))
		{
			poppedNode->data = newStack->data;
		}
		
		return poppedNode;
	}
	
	void display ()
	{
		//нужна проверка this на empty!!!
		if (this->head == NULL)
		{
			printf ("Stack: EMPTY!\n");
			return;
		}
		LockFreeStack current = *this->head.load();
		printf ("Stack: ");
		while (true)
		{
			printf ("%d ", current.data);
			if (current.next == nullptr) break;
			current = *current.next;
		}
		printf ("\n");
	}
	
	bool isEmpty()
	{
		return (this->head == NULL);
		
	}
};

bool empty();//

void testStack (OptStack* toTest, int volume)
{
	for (int i = 0; i < volume; i++)
	{
		int pushOrPop = rand()%2;
		if (pushOrPop)
		{
			toTest->push (rand()%volume);
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
	OptStack toTest;
	srand (time (NULL));
	
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
	
	for (int i = 0; i < INIT_PUSH; i++)
	{
		toTest.push (rand() % INIT_PUSH);
	}
	
	uint64_t tick = __rdtsc ()/100000;
	
	for (int i = 0; i < maxThreads; i++)
	{
		thr[i] = std::thread (testStack, &toTest, MAX_VOLUME/maxThreads);
	}
	
	for (int i = 0; i < maxThreads; i++)
	{
		thr[i].join ();
	}
	
	uint64_t tick2 = __rdtsc ()/100000;
	printf ("%llu\n", tick2 - tick);
		
}