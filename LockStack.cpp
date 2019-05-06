#include <iostream>
#include <thread>
#include <mutex>
#include <x86intrin.h>

#define INIT_PUSH 10000
#define MAX_THREAD_NUM 100
#define MAX_VOLUME 10000000

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

void testStack (LockStack* toTest, const int volume)
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
	srand (time(NULL));
	
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
	
	for (int i = 0; i < INIT_PUSH; i++)
	{
		toTest.push (rand() % INIT_PUSH);
	}
	
	uint64_t tick = __rdtsc ()/100000;
	
	for (int i = 0; i < maxThreads; i++)
	{
		//toTest.push (i);
		thr[i] = std::thread (testStack, &toTest, MAX_VOLUME/maxThreads); //testPush, &toTest, i);
	}
	
	for (int i = 0; i < maxThreads; i++)
	{
		thr[i].join ();
	}
	
	/*for (int i = 0; i < MAX_THREAD_NUM; i++)
	{
		printf ("%d\n", toTest.pop());
	}*/
	
	uint64_t tick2 = __rdtsc ()/100000;
	printf ("%llu\n", tick2 - tick);
	
	return 0;
}
