#include "rational_reconstruction.h"


template<class Plaintext, class Ciphertext>
void DataSource<Plaintext, Ciphertext>::encode_data() {
	std::cout << "X (data source) = " << std::endl << _X << std::endl;

	Matrix<Ciphertext> EncX;
	EncX.resize(_X.cols(), _X.rows());
	for (unsigned int col = 0; col < _X.cols(); ++col) {
		for (unsigned int row = 0; row < _X.rows(); ++row) {
			EncX(col, row) = Ciphertext::static_from_int(_X(col,row).to_int());
		}
	}

	std::vector<Ciphertext> Ency;
	Ency.resize(_y.size());
	for (unsigned int i = 0; i < _y.size(); ++i) {
		Ency[i] = Ciphertext::static_from_int(_y[i].to_int());
	}

	std::cout << "encX (data source) = " << std::endl << EncX << std::endl;

	_communication_channel->send_X_and_y_to_server1(EncX, Ency);
}


template<class Plaintext, class Ciphertext>
void Server1<Plaintext, Ciphertext>::mask(const Matrix<Ciphertext> &X, const std::vector<Ciphertext> &y) {
	Matrix<Ciphertext> A;
	std::vector<Ciphertext> b;

	std::cout << "X (server1) = " << std::endl << X << std::endl;

	A = X.T() * X;
	b = X.T() * y;

	std::cout << "A (server1) = " << std::endl << A << std::endl;

	_R.resize(A.cols(), A.rows());
	draw(_R);

	std::cout << "R (server1) = " << std::endl << _R << std::endl;

	_r.resize(b.size());
	draw(_r);

	Matrix<Ciphertext> Aprime;
	mul(Aprime, _R, A);

	std::cout << "A' (server1) = " << std::endl << Aprime << std::endl;

	std::vector<Ciphertext> bprime;
	mul(bprime, _R, b);
	add(bprime, _r);

	_communication_channel->send_Aprime_and_bprime_to_server2(Aprime, bprime);
}

template<class Plaintext, class Ciphertext>
void Server2<Plaintext, Ciphertext>::solve() {
	Matrix< Plaintext > Aprime;
	std::vector<Plaintext> bprime;

	Aprime.resize(_EncAprime.cols(), _EncAprime.rows());
	for (unsigned int col = 0; col < _EncAprime.cols(); ++col) {
		for (unsigned int row = 0; row < _EncAprime.rows(); ++row) {
			Aprime(col, row) = _EncAprime(col, row).to_int();
		}
	}
	std::cout << "A' = " << std::endl << Aprime << std::endl;

	bprime.resize(_Encbprime.size());
	for (unsigned int i = 0; i < bprime.size(); ++i) {
		bprime[i] = _Encbprime[i].to_int();
	}

	Matrix< Plaintext > Aprimeinv = Aprime.inverse();

	Matrix<Ciphertext> EncAprimeinv;

	EncAprimeinv.resize(Aprime.cols(), Aprimeinv.rows());
	for (unsigned int col = 0; col < Aprimeinv.cols(); ++col) {
		for (unsigned int row = 0; row < Aprimeinv.rows(); ++row) {
			EncAprimeinv(col, row) = Ciphertext::static_from_int(Aprimeinv(col, row).to_int());
		}
	}

	std::cout << "A'^-1 = " << std::endl << Aprimeinv << std::endl;

	std::vector<Plaintext> wprime;
	mul(wprime, Aprimeinv, bprime);

	print((std::cout << "b' = "), bprime) << std::endl;
	print((std::cout << "w' = "), wprime) << std::endl;

	std::vector<Ciphertext> Encwprime;
	Encwprime.resize(wprime.size());
	for (unsigned int i = 0; i < wprime.size(); ++i) {
		Encwprime[i] = Ciphertext::static_from_int(wprime[i].to_int());
	}

	_communication_channel->send_wprime_and_Ainv_to_server1(Encwprime, EncAprimeinv);
}

template<class Plaintext, class Ciphertext>
void Server1<Plaintext, Ciphertext>::unmask(const Matrix<Ciphertext> &X, const std::vector<Ciphertext> y, std::vector<Ciphertext> &Encw) {
	std::vector<Ciphertext> temp;
	mul(temp, _Aprimeinv, _r);
	Encw = _wprime - temp;
}


template<class Plaintext, class Ciphertext>
void DataSource<Plaintext, Ciphertext>::decrypt(std::vector<Plaintext> &w) {
	w.resize(_Encw.size());
	for (unsigned int i = 0; i < w.size(); ++i) {
		w[i] = _Encw[i].to_int();
	}
}





template<class Plaintext, class Ciphertext>
void Server1<Plaintext,Ciphertext>::linear_regression() {
	std::vector<Ciphertext> Encw;

	mask(_X, _y);
	_communication_channel->server2_solve();
	unmask(_X, _y, Encw);

	_communication_channel->send_w_to_DataSource(Encw);
}



// Code taken from https://stackoverflow.com/questions/30495102/iterate-through-different-subset-of-size-k/30518940#30518940
template<typename BidiIter, typename CBidiIter, typename Compare = std::less<typename BidiIter::value_type> >
int next_comb(BidiIter first, BidiIter last, CBidiIter /* first_value */, CBidiIter last_value, Compare comp = Compare()) {
	// 1. Find the rightmost value which could be advanced, if any
	auto p = last;
	while (p != first && !comp(*(p - 1), *--last_value))
		--p;
	if (p == first)
		return false;

	// 2. Find the smallest value which is greater than the selected value
	for (--p; comp(*p, *(last_value - 1)); --last_value) {
	}

	// 3. Overwrite the suffix of the subset with the lexicographically smallest sequence starting with the new value
	while (p != last)
		*p++ = *last_value++;
	return true;
}


template<class Plaintext, class Ciphertext>
void Server1<Plaintext,Ciphertext>::exhaustive_sparse_linear_regression() {
//	int k = 10;
//
//	std::vector<int> values(X.size() + 1);
//	for (unsigned int i = 0; i < X.size(); ++i)
//		values[i] = i;
//
//		// Since that's sorted, the first subset is just the first k values */
//	std::vector<int> subset(values.cbegin(), values.cbegin() + k);
//
//		// Print each combination
//	do {
//		for (auto const& v : subset) std::cout << v << ' ';
//		std::cout << '\n';
//	} while (next_comb(subset.begin(),  subset.end(), values.cbegin(), values.cend()));
//
//
//
//
//
//	std::vector<Ciphertext> Encw;
//
//	mask(_X, _y);
//	_communication_channel->server2_solve();
//	unmask(_X, _y, Encw);
//
//	_communication_channel->send_w_to_DataSource(Encw);
}









//////////////////////////////////////

template<class Plaintext, class Ciphertext>
template<class Bits>
void Server1<Plaintext,Ciphertext>::toBits(Bits &bits, const Ciphertext &y) {
	Plaintext r;

	// draw r
	r.from_int(random());

	Ciphertext _y(y);
	_y -= r;

	_communication_channel->server2_toBits(bits, _y);

	bits += Bits( r.to_int() );
}

