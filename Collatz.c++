// ----------------------------
// projects/collatz/Collatz.c++
// Copyright (C) 2015
// Glenn P. Downing
// ----------------------------

// --------
// includes
// --------

#include <cassert>  // assert
#include <iostream> // endl, istream, ostream
#include <sstream>  // istringstream
#include <string>   // getline, string
#include <utility>  // make_pair, pair
#include <vector>

#include "Collatz.h"

using namespace std;

namespace
{
	/// @return the length of the collatz cycle for 'n'
	/// @note: only works for n > 0 && n < 1000000
	int collatz_length(int n)
	{
		assert(n > 0);

		int c = 1;
		while (n > 1)
		{
			++c; // another round in the cycle

			if ((n & 1) == 0)
			{
				// even - divide by 2
				n >>= 1;
			}
			else
			{
				// odd -  n = 3n + 1
				n = (n << 1) + n;
				++n;
			}
		}

		assert(c >= 1);
		return c;
	}

	class CollatzCache
	{
	public:
		CollatzCache(int cacheSize = 64)
		: oddResults_(cacheSize, 0)
		{
		}

		int lookup(int n)
		{
			assert(n > 0);

			// handle even cases
			if ((n & 1) == 0)
				return 1 + lookup(n >> 1);

			// handle cached case
			const int cache_index = n >> 1;
			if (cache_index < (int)oddResults_.size())
			{
				if (oddResults_[cache_index] == 0)
					oddResults_[cache_index] = calculate_value(n);

				return oddResults_[cache_index];
			}

			// uncached case
			return calculate_value(n);
		}

	private:
		int calculate_value(int n)
		{
			// TODO: use cache in intermediate cases
			return collatz_length(n);
		}

		std::vector<int> oddResults_; ///< results for the first few odd values
	};
}

// ------------
// collatz_read
// ------------

pair<int, int> collatz_read (const string& s) {
    istringstream sin(s);
    int i;
    int j;
    sin >> i >> j;
    return make_pair(i, j);}

/// @return the longest cycle length between i and j
int collatz_eval(int i, int j)
{
	const int start = std::min(i, j);
	const int end = std::max(i, j);

	int num = 0;
	for (int n = start; n <= end; ++n)
	{
		const int tmp = collatz_length(n);
		if (tmp > num)
		{
			num = tmp;
		}
	}

	return num;
}

// -------------
// collatz_print
// -------------

void collatz_print (ostream& w, int i, int j, int v) {
    w << i << " " << j << " " << v << endl;}

// -------------
// collatz_solve
// -------------

void collatz_solve (istream& r, ostream& w) {
    string s;
    while (getline(r, s)) {
        const pair<int, int> p = collatz_read(s);
        const int            i = p.first;
        const int            j = p.second;
        const int            v = collatz_eval(i, j);
        collatz_print(w, i, j, v);}}
