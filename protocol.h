#include "times.h"
#include "rational_reconstruction.h"
#include "packed_matrix.h"
#include "packed_vector.h"


template<class Plaintext, class Ciphertext>
void DataSource<Plaintext, Ciphertext>::encode_data() {

	Matrix<Plaintext> tempA(_X.cols(), _X.cols());
	std::vector<Plaintext> tempb(_X.cols());

	for (unsigned int col = 0; col < _X.cols(); ++col)
		for (unsigned int row = 0; row < _X.cols(); ++row)
			tempA(col, row).from_int(0);

	for (unsigned int col = 0; col < _X.cols(); ++col)
		tempb[col].from_int(0);


	PackedMatrixSet<Ciphertext> A_simd;
	PackedVector<Ciphertext> b_simd;

	Times::start_phase1_step2();

	for (unsigned int i = 0; i < _X.rows(); ++i) {

		for (unsigned int col = 0; col < _X.cols(); ++col)
			for (unsigned int row = 0; row < _X.cols(); ++row)
				tempA(col, row) += (_X(col,i).to_int() * _X(row,i).to_int());

		for (unsigned int col = 0; col < _X.cols(); ++col)
			tempb[col] += (_X(col,i).to_int() * _y[i].to_int());
	}

	A_simd.init_left_matrix(tempA);
	b_simd.init_vector(tempb);

	Times::end_phase1_step2();

	_communication_channel->send_A_and_b_to_server1(A_simd, b_simd);
}

template<class Plaintext, class Ciphertext>
void Server1<Plaintext, Ciphertext>::receive_fraction_of_A_and_b(const PackedMatrixSet<Ciphertext> &A, const PackedVector<Ciphertext> &b) {
	Times::start_phase1_step3();
	if (_A.cols() == 0) {
		_A = A;
		_b = b;
	} else {
		_A += A;
		_b += b;
	}
	Times::end_phase1_step3();
}


template<class Plaintext, class Ciphertext>
void Server1<Plaintext, Ciphertext>::mask(const Matrix<Ciphertext> &X, const std::vector<Ciphertext> &y) {
//	std::cout << "server1 masking:  A = " << std::endl << _A << std::endl;
//	std::cout << "server1 masking: A_simd = " << std::endl << _A_simd << std::endl;

//	std::cout << "server1 masking:  b = " << _b << std::endl;
//	std::cout << "server1 masking: b_simd = " << _b_simd << std::endl;

	_R.resize(_A.cols(), _A.rows());
	_r.resize(_b.size());

	PackedMatrix<Ciphertext> Aprime;
	PackedVector<Ciphertext> bprime;

	draw(_R);
	draw(_r);

	PackedMatrixSet<Plaintext> R_simd;
	R_simd.init_right_matrix(_R);

	PackedMatrixSet<Plaintext> r_simd;
	r_simd.init_right_vector(_r);

	PackedMatrix<Ciphertext> Ar_simd;

	Times::start_phase2_step1();
	mul(Aprime, _A, R_simd);

	mul(Ar_simd, _A, r_simd);
	add(bprime, Ar_simd, _b);

	Times::end_phase2_step1();

	_communication_channel->send_Aprime_and_bprime_to_server2(Aprime, bprime);
}

template<class Plaintext, class Ciphertext>
void Server2<Plaintext, Ciphertext>::solve() {
	Plaintext::set_global_simd_factor(1);

	Matrix<Plaintext> Aprime;
	std::vector<Plaintext> bprime;

	Times::start_phase2_step2a();
	_EncAprime.to_matrix(Aprime);
	_Encbprime.to_vector(bprime);
	Times::end_phase2_step2a();

	Times::start_phase2_step2b();
	Matrix< Plaintext > Aprimeinv = Aprime.inverse();

	std::vector<Plaintext> wprime;
	mul(wprime, Aprimeinv, bprime);

	Times::end_phase2_step2b();

	_communication_channel->send_w_to_server1(wprime);
}

template<class Plaintext, class Ciphertext>
void Server1<Plaintext, Ciphertext>::unmask(std::vector<Plaintext> &w) {
	std::vector<Plaintext> temp;

	Times::start_phase2_step3();
	mul(w, _R, _wprime);
	sub(w, _r);
	Times::end_phase2_step3();
}


//template<class Plaintext, class Ciphertext>
//void DataSource<Plaintext, Ciphertext>::decrypt(std::vector<Plaintext> &w) {
//	w.resize(_Encw.size());
//
//	Times::start_phase2_step3();
//	for (unsigned int i = 0; i < w.size(); ++i) {
//		w[i] = _Encw[i].to_int();
//	}
//	Times::end_phase2_step3();
//}





template<class Plaintext, class Ciphertext>
void Server1<Plaintext,Ciphertext>::linear_regression() {
	std::vector<Plaintext> w;

	mask(_X, _y);
	_communication_channel->server2_solve();
	unmask(w);

	_communication_channel->send_w_to_DataSource(w);


	Times::print(std::cout);
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

