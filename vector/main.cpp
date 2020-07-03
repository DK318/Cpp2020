#include "vector.h"
#include <cstdlib>
#include <iostream>

int main() {
	vector<size_t> a;
	std::cout << (a.data() == nullptr) << '\n';
	vector<size_t> b;
	b = a;
	std::cout << b.data() << '\n';
	a = b;
	std::cout << (a.data() == nullptr) << '\n';
	return 0;
}