#ifndef ___COMMUNICATION___
#define ___COMMUNICATION___

template<class Plaintext, class Ciphertext>
class Communication {
private:
	std::vector< DataSource<Plaintext, Ciphertext> > &_dataSource;
	Server1<Plaintext, Ciphertext> &_server1;
	Server2<Plaintext, Ciphertext> &_server2;

public:
	Communication(std::vector< DataSource<Plaintext, Ciphertext> > &d, Server1<Plaintext, Ciphertext> &s1, Server2<Plaintext, Ciphertext> &s2) : _dataSource(d), _server1(s1), _server2(s2) {
		for (unsigned int i = 0; i < _dataSource.size(); ++i)
			_dataSource[i].setCommunicationChannel(this);
		_server1.setCommunicationChannel(this);
		_server2.setCommunicationChannel(this);
	}

	void send_A_and_b_to_server1(const PackedMatrixSet<Ciphertext> &A, const PackedVector<Ciphertext> &b) { _server1.receive_fraction_of_A_and_b(A, b); }
	void send_X_and_y_to_server1(const Matrix<Ciphertext> &A, const std::vector<Ciphertext> &b) { _server1.receive_X_and_y_from_data_source(A, b); }
	void send_Aprime_and_bprime_to_server2(const PackedMatrix<Ciphertext> &A, const PackedVector<Ciphertext> &b) { _server2.receive_A_and_bfrom_server1(A, b); }
	void send_w_to_DataSource(const std::vector<Plaintext> &v) {
		for (unsigned int i = 0; i < _dataSource.size(); ++i)
			_dataSource[i].receive_w_from_server1(v);
	}
	void send_w_to_server1(const std::vector<Plaintext> &w) { _server1.receive_w_from_server2(w); }

	void server2_solve() { _server2.solve(); }

	template<class Bits>
	void server2_toBits(Bits &bits, const Ciphertext &y) { _server2.toBits(bits, y); }
};

#endif
