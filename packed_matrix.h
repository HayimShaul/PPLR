#ifndef ___PACKED_MATRIX___
#define ___PACKED_MATRIX___

template<class Number>
class PackedMatrix {
private:
	std::vector<Number> _ctxt;
	
	enum MatrixType { NONE, RIGHT, LEFT };
	MatrixType _matrixType;

	unsigned int _cols, _rows;
public:
	PackedMatrix() : _matrixType(NONE), _cols(0), _rows(0) {}

	unsigned int cols() const { return _cols; }
	unsigned int rows() const { return _rows; }

	long to_int(int col, int row) const {
		int simd_factor = _ctxt[0].simd_factor();
		int i_item = row * _cols + col;
		int i_ctxt = i_item / simd_factor;
		std::vector<long> a = _ctxt[i_ctxt].to_vector();
		return a[i_item - i_ctxt * simd_factor];
	}

	void init_matrix(int rows, int cols) {
		_cols = cols;
		_rows = rows;

		Number c;
		unsigned int simd_factor = c.simd_factor();
		_ctxt.resize((cols * rows + simd_factor - 1) / simd_factor);

		std::vector<long> zero(simd_factor);
		for (unsigned int i = 0; i < simd_factor; ++i)
			zero[i] = 0;

		for (unsigned int i = 0; i < _ctxt.size(); ++i) {
			_ctxt[i].from_vector(zero);
		}
	}

	template<class In>
	void init_left_matrix(const Matrix<In> &m, int rot) {
		_cols = m.cols();
		_rows = m.rows();
		_matrixType = LEFT;

		Number temp;

		std::vector<long> serialMat(temp.simd_factor());
		_ctxt.resize((m.rows()*m.cols() + temp.simd_factor() - 1) / temp.simd_factor());

		unsigned int i_simd = 0;
		unsigned int i_ctxt = 0;
		for (unsigned int i_row = 0; i_row < m.rows(); ++i_row) {
			unsigned int row = (i_row + rot) % m.rows();
			for (unsigned int col = 0; col < m.rows(); ++col) {
				serialMat[i_simd] = m(row, col).to_int();
				++i_simd;
				if (i_simd == temp.simd_factor()) {
					i_simd = 0;
					_ctxt[i_ctxt].from_vector(serialMat);
					++i_ctxt;
				}
			}
		}

		if (i_simd != 0) {
			while (i_simd < temp.simd_factor()) {
				serialMat[i_simd] = 0;
				++i_simd;
			}
			_ctxt[i_ctxt].from_vector(serialMat);
			std::cout << "after from_vector: " << _ctxt[i_ctxt] << std::endl;
		}
	}

	template<class In>
	void init_right_matrix(const Matrix<In> &m, int rot) {
		_cols = m.cols();
		_rows = m.rows();
		_matrixType = RIGHT;

		Number temp;

		std::vector<long> serialMat(temp.simd_factor());
		_ctxt.resize((m.rows()*m.cols() + temp.simd_factor() - 1) / temp.simd_factor());

		unsigned int i_simd = 0;
		unsigned int i_ctxt = 0;
		for (unsigned int row = 0; row < m.rows(); ++row) {
			for (unsigned int i_col = 0; i_col < m.rows(); ++i_col) {
				unsigned int col = (i_col + rot) % m.cols();

				serialMat[i_simd] = m(row, col).to_int();
				++i_simd;
				if (i_simd == temp.simd_factor()) {
					i_simd = 0;
					_ctxt[i_ctxt].from_vector(serialMat);
					++i_ctxt;
				}
			}
		}

		if (i_simd != 0) {
			while (i_simd < temp.simd_factor()) {
				serialMat[i_simd] = 0;
				++i_simd;
			}
			_ctxt[i_ctxt].from_vector(serialMat);
		}
	}

	template<class Out, class In1, class In2>
	friend void add_mul(PackedMatrix<Out> &out, const PackedMatrix<In1> &left, const PackedMatrix<In2> &right);
};

template<class Number> 
class PackedMatrixSet {
private:
	std::vector<PackedMatrix<Number> > _mat;

	enum MatrixType { NONE, RIGHT, LEFT };
	MatrixType _matrixType;

	unsigned int _cols, _rows;
public:
	PackedMatrixSet() : _matrixType(NONE), _cols(0), _rows(0) {}

	unsigned int cols() const { return _cols; }
	unsigned int rows() const { return _rows; }


	template<class In>
	void init_left_matrix(const Matrix<In> &m) {
		_cols = m.cols();
		_rows = m.rows();
		_matrixType = LEFT;

		_mat.resize(_cols);
		for (unsigned int i = 0; i < _mat.size(); ++i) {
			_mat[i].init_left_matrix(m, i);
		}

		std::cout << "Packed left matrix:" << std::endl;
		for (unsigned int i = 0; i < m.cols(); ++i) {
			std::cout << "   Rotation " << i << ": " << std::endl << _mat[i] << std::endl;
		}
	}

	template<class In>
	void init_right_matrix(const Matrix<In> &m) {
		_cols = m.cols();
		_rows = m.rows();
		_matrixType = RIGHT;

		_mat.resize(_rows);
		for (unsigned int i = 0; i < _mat.size(); ++i) {
			_mat[i].init_right_matrix(m, i);
		}

		std::cout << "Packed left matrix:" << std::endl;
		for (unsigned int i = 0; i < m.cols(); ++i) {
			std::cout << "   Rotation " << i << ":" << std::endl << _mat[i] << std::endl;
		}
	}

	template<class Out, class In1, class In2>
	friend void mul(PackedMatrix<Out> &out, const PackedMatrixSet<In1> &left, const PackedMatrixSet<In2> &right);
};

template<class Out, class In1, class In2>
void add_mul(PackedMatrix<Out> &out, const PackedMatrix<In1> &left, const PackedMatrix<In2> &right) {
	assert (out.cols() == left.cols());
	assert (out.cols() == right.cols());
	assert(left._ctxt.size() == right._ctxt.size());

	for (unsigned int i = 0; i < left._ctxt.size(); ++i)
		out._ctxt[i] += left._ctxt[i] * right._ctxt[i];
}

template<class Out, class In1, class In2>
void mul(PackedMatrix<Out> &out, const PackedMatrixSet<In1> &left, const PackedMatrixSet<In2> &right) {
	assert(left.cols() == right.rows());
	assert(left._mat.size() == right._mat.size());

	out.init_matrix(left.rows(), right.cols());

	for (unsigned int i = 0; i < right._mat.size(); ++i) {
		add_mul(out, left._mat[i], right._mat[i]);
	}
}

template<class Number>
std::ostream &operator<<(std::ostream &out, const PackedMatrix<Number> &m) {
	for (unsigned int row = 0; row < m.rows(); ++row) {
		for (unsigned int col = 0; col < m.cols(); ++col) {
			out << m.to_int(row, col) << " ";
		}
		out << std::endl;
	}
	return out;
}

#endif
