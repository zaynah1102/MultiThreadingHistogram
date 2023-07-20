#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <algorithm>
#include <mutex>
using namespace std;

void fileToMemoryTransfer(char* fileName, char** data, size_t& numOfBytes) { // starter code
	streampos begin, end;
	ifstream inFile(fileName, ios::in | ios::binary | ios::ate);
	if (!inFile)
	{
		cerr << "Cannot open " << fileName << endl;
		inFile.close();
		exit(1);
	}
	size_t size = inFile.tellg();
	char* buffer = new  char[size];
	inFile.seekg(0, ios::beg);
	inFile.read(buffer, size);
	inFile.close();
	*data = buffer;
	numOfBytes = size;

}

int main(int argc, char** argv)
{

	const unsigned int THREAD_COUNT = thread::hardware_concurrency();
	char* buffer[1];
	size_t nbytes;
	vector<thread> workers;											//vector of worker threads

	fileToMemoryTransfer(argv[1], buffer, nbytes);					// read input

	const unsigned int CHAR_COUNT = 256;

	int chunk = nbytes / THREAD_COUNT;								// number of bytes divided amongst threads
	int extra = nbytes % THREAD_COUNT;								// leftover bytes
	int start = 0;													// start initialized as 0 because first thread goes through bytes in buffer starting at index 0 to bytes per thread minus 1 (since indexing starts at 0)
	int end = chunk;												// section that first thread goes through ends at number of bytes per thread (not included since indexing starts from 0)


	// global histogram
	vector<int> globalH(CHAR_COUNT, 0);								// global histogram vector with 256 elements initialized to 0
	mutex m;														// mutex to lock

	for (int t = 0; t < THREAD_COUNT; t++)
	{
		if (t == THREAD_COUNT - 1)									// last thread processes any extra rows
			end += extra;

		workers.push_back(thread([&m, &globalH](int start, int end, const char* buffer)
			{
				unsigned char index;
				for (int i = start; i < end; i++)
				{
					lock_guard<mutex> lg(m);						// mutex to avoid race conditions (all threads are updating the same indices on a global histogram)
					index = buffer[i];
					globalH[index]++;						// increment histogram using the ascii value as the index
				}
			}, start, end, *buffer)
		);

		start = end;												// starting point in buffer for next thread is end of last thread
		end = start + chunk;										// end of next thread is the new start plus bytes per thread

	}

	for_each(workers.begin(), workers.end(), [](thread& t)			// wait for all threads to finish
		{
			t.join();
		}
	);

	cout << "Run with one global histogram\n";
	for (int i = 0; i < CHAR_COUNT; i++)							// print results
	{
		cout << i << ": " << globalH.at(i) << endl;
	}


	// local histogram
	workers.clear();												// clear the workers vector
	const unsigned int TOTAL_BUCKETS = CHAR_COUNT * THREAD_COUNT;	// total "buckets" in vector containing all local histograms
	vector<unsigned int> h(TOTAL_BUCKETS, 0);						// vector of local histograms created, all buckets initialized to 0

	start = 0;														// reinitialize start to 0 for use with local histograms
	end = chunk;													// reinitialize end to bytes per thread

	for (int t = 0; t < THREAD_COUNT; t++)
	{
		if (t == THREAD_COUNT - 1)									// last thread processes any extra rows
			end += extra;

		int offset = CHAR_COUNT * t;								// offset calculated so that the correct index is incremented depending on which thread is running, offset for first thread will be 0 (t = 0)

		workers.push_back(thread([&h](int start, int end, const char* buffer, int offset)
			{
				unsigned char index;
				for (unsigned int i = start; i < end; i++)
				{
					index = buffer[i];
					h[index + offset]++;							// increment index of h at ascii value of the character at the position i + the offset calculated based on which thread it is
				}
			}, start, end, *buffer, offset)
		);


		start = end;												// starting point in buffer for next thread is end of last thread
		end = start + chunk;										// end of next thread is the new start plus bytes per thread

	}

	for_each(workers.begin(), workers.end(), [](thread& t)			// wait for all threads to finish
		{
			t.join();
		}
	);

	for (unsigned i = 0; i < CHAR_COUNT; i++)						// add up totals of local histograms
	{
		unsigned sum = 0;
		for (unsigned j = 0; j < THREAD_COUNT; j++)
		{
			sum += h[(j * CHAR_COUNT) + i];
		}
		h[i] = sum;
	}

	cout << "Run with local histograms\n";
	for (unsigned i = 0; i < CHAR_COUNT; i++)						// print results
	{
		cout << i << ": " << h[i] << endl;
	}


	return 0;
}