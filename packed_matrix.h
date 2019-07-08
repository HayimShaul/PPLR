#ifndef ___PACKED_MATRIX___
#define ___PACKED_MATRIX___

template<class Number>
class PackedVector;

template<class Number>
class PackedMatrix {
private:
	std::vector<Number> _ctxt;
	
	enum MatrixType { NONE, RIGHT, LEFT };
	MatrixType _matrixType;

	unsigned int _cols, _rows;
public:
	PackedMatrix() : _matrixType(NONE), _cols(0), _rows(0) {}

	PackedMatrix<Number> &operator=(const PackedMatrix<Number> &m) {
		_matrixType = m._matrixType;
		_cols = m._cols;
		_rows = m._rows;

		_ctxt.resize(m._ctxt.size());
		for (unsigned int i = 0; i < _ctxt.size(); ++i) {
			_ctxt[i] = m._ctxt[i];
		}
		return *this;
	}

	unsigned int cols() const { return _cols; }
	unsigned int rows() const { return _rows; }

	long to_int(int col, int row) const {
		int simd_factor = _ctxt[0].simd_factor();
		int i_item = col * _rows + row;
		int i_ctxt = i_item / simd_factor;
		std::vector<long> a = _ctxt[i_ctxt].to_vector();
		return a[i_item - i_ctxt * simd_factor];
	}

	template<class Out>
	void to_matrix(Matrix<Out> &o) {
		o.resize(_cols, _rows);
		int simd_factor = _ctxt[0].simd_factor();
		int i_ctxt = -1;
		int i_slot = simd_factor;
		std::vector<long> data;

		for (unsigned int i_col = 0; i_col < _cols; ++i_col) {
			for (unsigned int i_row = 0; i_row < _rows; ++i_row) {
				if (i_slot == simd_factor) {
					i_slot = 0;
					++i_ctxt ;
					data = _ctxt[i_ctxt].to_vector();
				}

				o(i_col, i_row).from_int(data[i_slot]);
				++i_slot;
			}
		}
	}

	void init_matrix(int cols, int rows) {
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
		for (unsigned int i_col = 0; i_col < m.cols(); ++i_col) {
			for (unsigned int row = 0; row < m.rows(); ++row) {
				unsigned int col = (i_col + row + rot) % m.cols();
				serialMat[i_simd] = m(col, row).to_int();
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
//			std::cout << "after from_vector: " << _ctxt[i_ctxt] << std::endl;
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
		for (unsigned int col = 0; col < m.cols(); ++col) {
			for (unsigned int i_row = 0; i_row < m.rows(); ++i_row) {
				unsigned int row = (i_row + col + rot) % m.rows();

				serialMat[i_simd] = m(col, row).to_int();
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

	void operator+=(const PackedMatrix<Number> &m) {
		assert(_matrixType == m._matrixType);
		assert(_cols == m._cols);
		assert(_rows == m._rows);
		assert(_ctxt.size() == m._ctxt.size());

		for (unsigned int i = 0; i < _ctxt.size(); ++i) {
			_ctxt[i] += m._ctxt[i];
		}
	}

	template<class Out, class In1, class In2>
	friend void add_mul(PackedMatrix<Out> &out, const PackedMatrix<In1> &left, const PackedMatrix<In2> &right);

	template<class N>
	friend void add(PackedVector<N> &o, const PackedMatrix<N> &a, const PackedVector<N> &b);
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

	PackedMatrixSet<Number> &operator=(const PackedMatrixSet<Number> &m) {
		_matrixType = m._matrixType;
		_cols = m._cols;
		_rows = m._rows;

		_mat.resize(m._mat.size());
		for (unsigned int i = 0; i < _mat.size(); ++i) {
			_mat[i] = m._mat[i];
		}
		return *this;
	}

	unsigned int cols() const { return _cols; }
	unsigned int rows() const { return _rows; }

	const std::vector<PackedMatrix<Number> > &mat() const { return _mat; }

	template<class In>
	void init_left_matrix(const Matrix<In> &m) {
		_cols = m.cols();
		_rows = m.rows();
		_matrixType = LEFT;

		_mat.resize(_cols);
		for (unsigned int i = 0; i < _mat.size(); ++i) {
			_mat[i].init_left_matrix(m, i);
		}

//		std::cout << "Packed left matrix:" << std::endl;
//		for (unsigned int i = 0; i < m.cols(); ++i) {
//			std::cout << "   Rotation " << i << ": " << std::endl << _mat[i] << std::endl;
//		}
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

//		std::cout << "Packed right matrix:" << std::endl;
//		for (unsigned int i = 0; i < m.cols(); ++i) {
//			std::cout << "   Rotation " << i << ":" << std::endl << _mat[i] << std::endl;
//		}
	}

	template<class In>
	void init_right_vector(const std::vector<In> &v) {
		Matrix<In> m(v.size(), v.size());

		for (unsigned row = 0; row < v.size(); ++row) {
			m(0, row) = v[row];
			for (unsigned col = 1; col < v.size(); ++col) {
				m(col, row).from_int(0);
			}
		}

		init_right_matrix(m);
	}

	void operator+=(const PackedMatrixSet<Number> &m) {
		assert(_matrixType == m._matrixType);
		assert(_cols == m._cols);
		assert(_rows == m._rows);
		assert(_mat.size() == m._mat.size());

		for (unsigned int i = 0; i < _mat.size(); ++i) {
			_mat[i] += m._mat[i];
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

	for (unsigned int i = 0; i < left._ctxt.size(); ++i) {
//		Out temp = left._ctxt[i] * right._ctxt[i];
//		std::cout << " mul = " << temp << std::endl;
		out._ctxt[i] += left._ctxt[i] * right._ctxt[i];
//		std::cout << " after adding = " << out._ctxt[i] << std::endl;
	}

//	std::cout << " after adding = " << std::endl << out << std::endl;
}

template<class Out, class In1, class In2>
void mul(PackedMatrix<Out> &out, const PackedMatrixSet<In1> &left, const PackedMatrixSet<In2> &right) {
	assert(left.cols() == right.rows());
	assert(left._mat.size() == right._mat.size());

	out.init_matrix(left.cols(), right.rows());

	for (unsigned int i = 0; i < right._mat.size(); ++i) {
//		std::cout << "   Adding  rotation " << i << std::endl;
		add_mul(out, left._mat[i], right._mat[i]);
	}
}

template<class Number>
std::ostream &operator<<(std::ostream &out, const PackedMatrix<Number> &m) {
	for (unsigned int row = 0; row < m.rows(); ++row) {
		for (unsigned int col = 0; col < m.cols(); ++col) {
			out << m.to_int(col, row) << " ";
		}
		out << std::endl;
	}
	return out;
}

template<class Number>
std::ostream &operator<<(std::ostream &out, const PackedMatrixSet<Number> &m) {
	for (unsigned int i = 0; i < m.mat().size(); ++i) {
		out << "  rotation " << i << std::endl;
		out << m.mat()[i];
		out << std::endl;
	}
	return out;
}


#endif
