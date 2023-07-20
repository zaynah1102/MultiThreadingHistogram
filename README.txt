*** Computing a Histogram Using Multi-threads in C++11 ***

This project is a VS 2022 Win64 Console C++11 program that uses the built-in threading library to compute a histogram of the contents of any (text or binary) file.
The code consists of two solutions to demonstrate the use of locks to prevent race conditions in situations where they can occur.
Global Histogram: a global histogram is updated by all threads at the same time, a must be used to prevent race conditions
Local Histogram: elminiates the possibility of race conditions because each thread updates its own local histogram.
			The number of occurrences of each base two number (0 - 255) are added together to create a final histogram with the totals.
Thread usage is maximized in both solutions.

Running the project:
- Test files are included in CS420Lab02AnsariZaynah\x64\Debug
- the following command can be used to run the project on the command line: >.\CS420Lab02AnsariZaynah.exe fileName