#include <iostream>
#include <string>

namespace
{

bool is_palindrome(const std::string &s)
{
	const size_t mid_point = s.length() / 2;
	std::string::const_reverse_iterator b = s.rbegin();
	for (size_t i = 0; i < mid_point; ++i)
	{
		if (s[i] != *b)
			return false;

		++b;
	}
	return true;
}

std::string make_palindrome(const std::string &s)
{
	size_t e = s.length() - 1;
	const size_t mid_point = s.length() / 2;
	for (size_t i = 0; i < mid_point; ++i)
	{
		if (s[i] != s[e])
		{
			std::string tmp(s);
			tmp.insert(e+1, 1, s[i]);
			if (is_palindrome(tmp))
				return tmp;
			//else
			tmp = s;
			tmp.insert(i, 1, s[e]);
			if (is_palindrome(tmp))
				return tmp;

			return "NA";
		}

		--e;
	}

	// it is a palindrome
	std::string ret(s);
	const size_t m = (s.length() - 1) / 2;
	return ret.insert(m, 1, s[m]);
}

} // anon namespace

int main(int argc, char **argv)
{
	std::string line;
	while (getline(std::cin, line))
	{
		std::cout << make_palindrome(line) << std::endl;
	}

	return 0;
}

