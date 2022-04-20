/*
 * main.cxx
 * 
 * C++ Driver Program to Call Python Talk-2-Text Script
 * 
 */

//g++ -Wall -o  "%e" "%f" -pthread $(python-config --includes --cflags --ldflags) >outfile.txt

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <Python.h>
#include <thread>
#include <chrono>
#include <sys/stat.h>
#include <ctime>
#include <pthread.h>
#include <unistd.h>

using namespace std;


int offline_mode()
{
	return system("sudo python3 talk2text_offline.py");
}

void * online_mode_try(void * arg)
{
	int ret = system("sudo python3 talk2text.py");
	
	pthread_exit(NULL);
	
}



void online_mode()
{
		
	pthread_t threadID;
	int error = pthread_create(&threadID,NULL, &online_mode_try, NULL); // running python code
	
	sleep(5);
	
	ifstream fin;
	fin.open("outfile.txt");
	ofstream temp;
	temp.open("temp.txt");
	
	string prev;
	getline(fin, prev);
	string current;
	
	while(1)
	{
		
		sleep(30); //Adjustable number of seconds
		getline(fin, current);
		if (current == prev)
		{
		system("sudo killall python3");
		pthread_cancel(threadID);

		if (system("ping -q -c 1 www.google.com"))
		{
			offline_mode();
		}
		//pthread_t threadID;
		sleep(1);
		remove("outfile.txt");
		rename("temp.txt", "outfile.txt");
		
		int error = pthread_create(&threadID,NULL, &online_mode_try, NULL); 
		
		}
		prev = current;
		current = "";
		
			
	}
	
}



int main(int argc, char **argv)
{
	
	volatile bool connected = (system("ping -q -c 1 www.google.com") == 0);

	
	if (connected)
	{

		online_mode();
	}
	else
	{

		offline_mode();
	}
	

	return 0;
}

