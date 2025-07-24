#ifndef RANK1TENSOR_H
#define RANK1TENSOR_H

#include <bitset>

using namespace std;

const int N = 64; // total number of unknowns

#ifndef MAX_ORDER
#define MAX_ORDER 4
#endif // MAX_ORDER // so the schemes are only up to O(e^MAX_ORDER)

class Rank1Tensor
{
public:
    int coeff;
    bitset<N> a[MAX_ORDER];
    bitset<N> b[MAX_ORDER];
    bitset<N> c[MAX_ORDER];

    Rank1Tensor();
    virtual ~Rank1Tensor();

    bool update();
};

#endif // RANK1TENSOR_H
