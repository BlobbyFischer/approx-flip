#include <iostream>
#include <ctime>
#include <unistd.h>

#include "Scheme.h"

using namespace std;

//THINGS AREN'T WORKING YET, EITHER REDUCTIONS OR FLIPS AREN'T KEEPING TENSORS THE SAME BEFORE AND AFTER...

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned int>(time(0)) + getpid());
    string filename = argv[1];
    int pathlength = stoi(argv[2]);
    Scheme scheme;
    scheme.from_file(filename);
    scheme.random_walk(pathlength);
    scheme.write_to_file();
    //scheme.check();
    return 0;
}
