//==============================================================================
// ldpc_encoder.cpp
//
// LDPC encoder implementation.
//------------------------------------------------------------------------------
// Copyright (c) 2019 Guangxi Liu
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.
//==============================================================================


#include "ldpc/ldpc_encoder.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

using namespace std;


//----------------------------------------------------------
// Right rotate vector
//
// Inout:
//     vec: vector to be rotated
//
// Input:
//     sh: right rotate shift number, negative number for zeros vector output
//----------------------------------------------------------
static void rotateVector(vector<int>& vec, int sh)
{
    if (sh < 0)
        fill(vec.begin(), vec.end(), 0);
    else
        rotate(vec.begin(), vec.begin() + sh, vec.end());
}


//----------------------------------------------------------
// LDPC encoder core
//
// Input:
//     dataIn: message data bits, value is 0 or 1
//     h: parity check matrix
//
// Return:
//     codeword data bits, value is 0 or 1
//----------------------------------------------------------
static vector<int> ldpcEncodeCore(const vector<int>& dataIn, const MatrixType& h)
{
    int k = h.n - h.m;
    if (dataIn.size() != static_cast<vector<int>::size_type>(k * h.z)) {
        cerr << "Error: Invalid input data size" << dataIn.size()
             << ", should be " << k * h.z << endl;
        exit(EXIT_FAILURE);
    }

    vector<int> x(h.m * h.z);
    vector<int> p(h.m * h.z);
    vector<int> t;

    for (int i = 0; i < h.m; i++) {
        for (int j = 0; j < k; j++) {
            t.assign(dataIn.begin() + j * h.z, dataIn.begin() + (j + 1) * h.z);
            rotateVector(t, h.base[i * h.n + j]);
            for (int ii = 0; ii < h.z; ii++)
                x[i * h.z + ii] = (x[i * h.z + ii] + t[ii]) % 2;
        }
    }

    for (int i = 0; i < h.m; i++) {
        for (int ii = 0; ii < h.z; ii++)
            p[ii] = (p[ii] + x[i * h.z + ii]) % 2;
    }

    t.assign(p.begin(), p.begin() + h.z);
    rotateVector(t, 1);
    for (int i = 1; i < h.m; i++) {
        for (int ii = 0; ii < h.z; ii++) {
            if (i == 1)
                p[i * h.z + ii] = (x[(i - 1) * h.z + ii] + t[ii]) % 2;
            else if (i == h.m / 2 + 1)
                p[i * h.z + ii] = (x[(i - 1) * h.z + ii] + p[ii] + p[(i - 1) * h.z + ii]) % 2;
            else
                p[i * h.z + ii] = (x[(i - 1) * h.z + ii] + p[(i - 1) * h.z + ii]) % 2;
        }
    }

    vector<int> cw(dataIn);
    cw.insert(cw.end(), p.begin(), p.end());
    return cw;
}


//----------------------------------------------------------
// LDPC encoder
//
// Input:
//     dataIn: message data bits, value is 0 or 1
//     mode: mode of codeword length and code rate
//
// Return:
//     codeword data bits, value is 0 or 1
//----------------------------------------------------------
vector<int> ldpcEncode(const vector<int>& dataIn, CodeMode mode)
{
    int idxHldpc = static_cast<int>(mode);
    return ldpcEncodeCore(dataIn, Hldpc[idxHldpc]);
}
