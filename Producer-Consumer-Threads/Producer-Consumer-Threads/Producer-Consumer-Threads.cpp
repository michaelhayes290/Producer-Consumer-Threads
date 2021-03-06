// Producer-Consumer-Threads.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <random>
#include <thread>
#include <mutex>
#include <condition_variable>
#define MAX 20		/* how many numbers to produce */
#define N   5		/* how big the buffer should be */

using namespace std;

mutex m;					//the mutex
thread prod, cons;			//the threads
condition_variable_any cp;	//the condition variable
condition_variable_any cc;	//the condition variable
char buffer[N];				//buffer used between producer and consumer
int lastElement = 0;		//the last element in the buffer

//statements used by the thread waiting
bool bufferNotFull() { return lastElement != sizeof(buffer); }
bool bufferNotEmpty() { return lastElement != 0; }

static void *producer() /* produce data */
{
	for (int i = 1; i <= MAX; i++)
	{
		m.lock(); //lock the mutex
		cp.wait(m, bufferNotFull); //wait while the buffer is full
		
		//generate a random letter
		int r = rand() % 26;
		char randLetter = 'a' + r;

		//print out the lowercase random letter and put the CAPS version in the buffer
		cout << randLetter;
		cout << "\n";
		buffer[lastElement] = toupper(randLetter);
		lastElement++;

		m.unlock(); //unlock the mutex
		cc.notify_one(); //wake up consumer
	}
	return 0;
}
static void *consumer() /* consume data */
{
	for (int i = 1; i <= MAX; i++)
	{
		m.lock(); //lock the mutex
		cc.wait(m, bufferNotEmpty); //wait while the buffer is empty

		//take out the last element in the buffer and print it
		lastElement--;
		cout << buffer[lastElement];
		cout << "\n";
		buffer[lastElement] = ' ';

		m.unlock(); //unlock the mutex
		cp.notify_one(); //wake up producer
	}
	return 0;
}
int main(int argc, char **argv)
{
	//introductory statements
	cout << "The Producer-Consumer Problem\nCreated by Michael Hayes\n";
	cout << "Buffer Size: " << N << "\n";
	cout << "[p]roducer / [C]onsumer - " << MAX << " items\n";
	
	//create the producer thread
	thread prod(producer);
	//wait a little bit so the producer can start making things
	this_thread::sleep_for(chrono::microseconds(1));
	//create the consumer thread
	thread cons(consumer);

	system("pause");
}