#include <iostream>
#include <ctime>
#include <unistd.h>

#include "Scheme.h"

using namespace std;

// I think I fixed eflips...

int main(int argc, char* argv[])
{
    if (3 > argc or 4 < argc) {
        cout << "USAGE: ./flip filename pathlength (check)" << endl;
        return 1;
    }
    srand(static_cast<unsigned int>(time(0)) + getpid());
    string filename = argv[1];
    int pathlength = stoi(argv[2]);
    int check;
    if (argc>3) check = stoi(argv[3]);
    else check = 0;
    Scheme scheme;
    scheme.from_file(filename);
    scheme.update();
    scheme.random_walk(pathlength);
    scheme.write_to_file();
    if (check!=0) scheme.check();
    return 0;
}
