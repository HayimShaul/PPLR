#ifndef ___LINEAR_REGRESSION___
#define ___LINEAR_REGRESSION___

template<class Num>
void encode(Num &n, float f) {
	n.from_int(int(f));
	return;

	int best_num = 0;
	int best_denom = 1;
	for (int num = 0; num < 6; ++num) {
		for (int denom = 1; denom < 6; ++denom) {
			if (abs((float)num/denom - f) < abs((float)best_num/best_denom)) {
				best_num = num;
				best_denom = denom;
			}
		}
	}

	n = best_num + power(best_denom, n.get_ring_size() - 2);
}

std::vector<int> linearRegression(const Matrix<float> &X, const std::vector<float> &y) {
	std::vector< DataSource<Plaintext, Ciphertext> > dataSource(DATA_SOURCE_NUMBER);

	int rowsPerSource = (X.rows() + DATA_SOURCE_NUMBER - 1) / DATA_SOURCE_NUMBER;

	for (int i_dataSource = 0; i_dataSource < DATA_SOURCE_NUMBER; ++i_dataSource) {
		Matrix<Plaintext> encX;
		std::vector<Plaintext> ency;

		unsigned int start = i_dataSource * rowsPerSource;
		unsigned int end = std::min(X.rows() - 1, start + rowsPerSource);

		encX.resize(X.cols(), end - start);
		ency.resize(end - start);

		for (unsigned int j = start; j < end; ++j) {
			encode(ency[j - start], y[j]);
			for (unsigned int i = 0; i < X.cols(); ++i) {
				encode(encX(i, j-start), X(i,j));
			}
		}

		dataSource[i_dataSource].set_data(encX, ency);
	}


	Server1<Plaintext, Ciphertext> server1;
	Server2<Plaintext, Ciphertext> server2;

	Communication<Plaintext, Ciphertext> communication(dataSource, server1, server2);

	for (unsigned int i = 0; i < dataSource.size(); ++i)
		dataSource[i].encode_data();

	server1.linear_regression();

//	server1.mask();
//	server2.solve();
//	server1.unmask();

//	std::vector<Plaintext> dec;
//	for (unsigned int i = 0; i < dataSource.size(); ++i) {
//		dataSource[i].decrypt(dec);
//	}

	std::vector<int> ret(dataSource[0].w().size());
	for (unsigned int i = 0; i < ret.size(); ++i)
		ret[i] = dataSource[0].w()[i].to_int();

	return ret;
}

//std::vector<int> linearRegression(const Matrix<float> &X, const std::vector<float> &y) {
//	std::vector< DataSource<Plaintext, Ciphertext> > dataSource(DATA_SOURCE_NUMBER);
//
//	int rowsPerSource = (X.rows() + DATA_SOURCE_NUMBER - 1) / DATA_SOURCE_NUMBER;
//
//	for (int i_dataSource = 0; i_dataSource < DATA_SOURCE_NUMBER; ++i_dataSource) {
//		Matrix<Plaintext> encX;
//		std::vector<Plaintext> ency;
//
//		unsigned int start = i_dataSource * rowsPerSource;
//		unsigned int end = std::min(X.rows() - 1, start + rowsPerSource);
//
//		encX.resize(X.cols(), end - start);
//		ency.resize(end - start);
//
//		for (unsigned int j = start; j < end; ++j) {
//			encode(ency[j - start], y[j]);
//			for (unsigned int i = 0; i < X.cols(); ++i) {
//				encode(encX(i, j-start), X(i,j));
//			}
//		}
//
//		dataSource[i_dataSource].set_data(encX, ency);
//	}
//
//
//	Server1<Plaintext, Ciphertext> server1;
//	Server2<Plaintext, Ciphertext> server2;
//
//	Communication<Plaintext, Ciphertext> communication(dataSource, server1, server2);
//
//	for (int i_dataSource = 0; i_dataSource < DATA_SOURCE_NUMBER; ++i_dataSource)
//		dataSource[i_dataSource].encode_data();
//
//	server1.linear_regression();
//
////	server1.mask();
////	server2.solve();
////	server1.unmask();
//
//	std::vector<Plaintext> dec;
//	for (unsigned int i = 0; i < dataSource.size(); ++i)
//		dataSource[i].decrypt(dec);
//
//	std::vector<int> ret(dec.size());
//	for (unsigned int i = 0; i < ret.size(); ++i)
//		ret[i] = dec[i].to_int();
//
//	return ret;
//}

template<class Plaintext, class Ciphertext>
void test_simd() {
	Matrix<Plaintext> L;
	Matrix<Plaintext> R;

	L.resize(3, 3);
	R.resize(3, 3);

	draw(R);
	draw(L);

	std::cout << "Left = " << std::endl << L << std::endl;
	std::cout << "Right = " << std::endl << R << std::endl;

	PackedMatrixSet<Ciphertext> Left;
	PackedMatrixSet<Plaintext> Right;

	Left.init_left_matrix(L);
	Right.init_right_matrix(R);

	PackedMatrix<Ciphertext> Res;
	mul(Res, Left, Right);

	Matrix<Ciphertext> Res_correct;
	mul(Res_correct, L, R);

	std::cout << "Correct Matrix is:" << std::endl << Res_correct << std::endl;
	std::cout << "SIMD Matrix is:" << std::endl << Res << std::endl;

}

#endif
