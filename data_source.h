#ifndef ___DATA_SOURCE___
#define ___DATA_SOURCE___

template<typename Plaintext, typename Ciphertext>
class Communication;

const int DATA_SOURCE_NUMBER = 10;

template<class Plaintext, class Ciphertext>
class DataSource {
private:
	Matrix<Plaintext> _X;
	std::vector<Plaintext> _y;

	std::vector<Plaintext> _w;

	Communication<Plaintext, Ciphertext> *_communication_channel;
public:
	DataSource() {}

	void set_data(const Matrix<Plaintext> &m, std::vector<Plaintext> &v) { _X = m; _y = v; }

	void setCommunicationChannel(Communication<Plaintext, Ciphertext> *c) { _communication_channel = c; }

	void receive_w_from_server1(const std::vector<Plaintext> &v) { _w = v; }

	void encode_data();

	const std::vector<Plaintext> &w() const { return _w; }

//	void decrypt(std::vector<Plaintext> &w);
};

#endif
