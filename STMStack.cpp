#include <iostream>
#include <thread>
#include <x86intrin.h>

#define INIT_PUSH 10000
#define MAX_THREAD_NUM 10
#define MAX_VOLUME 10000000

class STMStack {
public:
	STMStack() :head(NULL) {}
	void push(int d)
	{
		Node *pv = new Node;
		pv->d = 0;//d;
		__transaction_atomic
		{
			pv->d = d;
			pv->p = head;
			head = pv;
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
		
		__transaction_atomic
		{
			temp = head->d;
			pv = head;
			head = head->p;
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

void testPush (STMStack* toTest, int volume)
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
}

int main (int argc, char** argv)
{
	STMStack testStack;
	
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
		testStack.push (rand() % INIT_PUSH);
	}
	uint64_t tick = __rdtsc ()/100000;
	
	for (int i = 0; i < maxThreads; i++)
	{
		thr[i] = std::thread (testPush, &testStack, MAX_VOLUME/maxThreads);
	}
	
	for (int i = 0; i < maxThreads; i++)
	{
		thr[i].join ();
	}
	
	uint64_t tick2 = __rdtsc ()/100000;
	printf ("%llu\n", tick2 - tick);
	
}
