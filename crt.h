#ifndef ___CRT___
#define ___CRT___

#include <assert.h>
#include <vector>

struct CrtDigit {
	long _base;
	long _val;

	CrtDigit(long v, long b) : _base(b), _val(v) { assert(v < b); }

	int base() const { return _base; }
	int val() const { return _val; }
};

inline int mod(int a, int b) {
	while (a < 0)
		a += b;
	return a % b;
}

inline int crtDecode(const std::vector<CrtDigit> &d) {
	if (d.size() == 0)
		return 0;

	long ret = d[0].val();
	long product = d[0].base();
	for (unsigned int i = 1; i < d.size(); ++i) {
		ret += mod(ret - d[i].val(), d[i].base()) * product;
		product *= d[i].base();
	}

//	std::cout << "CRT decoding: " << std::endl;
//	for (unsigned int i = 0; i < d.size(); ++i) {
//		std::cout << d[i].val() << " mod " << d[i].base() << std::endl;
//	}
//	std::cout << "turned out to be: x = " << ret << std::endl << std::endl;

	return ret;
}

#endif
