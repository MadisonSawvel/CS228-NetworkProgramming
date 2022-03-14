#include <iostream>     
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>

using namespace std;

Class Buffer {
	public:
	int get() {return data};
	int put(int p);
	bool isEmpty() {return head == tail}
	bool isFull() {return head =(tail+1)%maxSize}	
}

Buffer buffer;
condition_variable p, c;
mutex mut;

bool isPrime(int maybe)
{
	if (maybe % 2 == 0)
		return false;
	for(int i = 3; i < maybe; i+=2)
		if (maybe % i == 0)
			return false;
	return true;
}
void makePrimes(int head, int tail)
{
	for(int i = head; i < tail; i++)
		if (isPrime(i))
		    buffer.put(i); //is buffer the array? do only primes go into the ring?
}
void print()
{
	while (1)
	        cout << "Prime " << buffer.get() << endl; 
}
void produce(){ //puts data into ring buffer 
	while(true){
		lock(mut); //make lock
		long data = proData(); //get data(primes)
		while(isFull){
			p.wait(lock,[] {return size=max;});
		}
		//insert
		//c.notify
	}
}
long proData(){ //finds the primes between the two numbers
	long start = 100000001;
	long stop = 100001001;
	int primes = makePrimes(start, stop);

}
void consume(){ //takes the data out of ring buffer
	while(true){
		while(buffer.isEmpty()){
			c.wait(lock,[]{return size=max;})
		}
		//fetch
		//print
		//p.notify
	}
}

int main (int argc, char *argv[])
{
	int N;
	cout << "How many threads to run?";
	cin >> N;
	int ringThing[N]; //n is how many threads 1,5,10
}

/*
Finish the code above.  It should start N examples of makePrimes.  It should start 1 example of print.
N comes from the command line as the first argument.
Basically, this means to finish main() to start and join threads, and to make Buffer class that
is a thread safe ring buffer that you wrote yourself.

You should not have to change any of my code above.

Atomics of any sort are forbidden.  Use must use mutexes, locks, notify, and wait.

Time this with 1 thread, 5 threads, and 10 threads.  Part of your homework is the timing data.

The program should search for primes from 100,000,001 to 100,001,001.

Hint:  You cannot call exit(0) when killing a program with running threads.  But you can call _exit(0).

write buffer ring first, then thread. Print frequenty to track

*/