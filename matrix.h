#ifndef ___MATRIX___
#define ___MATRIX___

#include <vector>
#include <ostream>
#include <stdexcept>
#include <functional>

#include <NTL/mat_ZZ_p.h>

#include <eq.h>

template<class Number>
class Matrix {
public:
	class InverseRuntimeError : public std::runtime_error {
	public:
		InverseRuntimeError(const std::string &s) : std::runtime_error(s) {}
	};
private:
	std::vector< std::vector<Number> > _data;
public:
	Matrix(int c = 0, int r = 0) { resize(c,r); }
	Matrix(const Matrix &m) : _data(m._data) {}

	void resize(int c, int r) {
		_data.resize(c);
		for (unsigned int i = 0; i < _data.size(); ++i)
			_data[i].resize(r);
	}

	Number &operator()(int c, int r) { return _data[c][r]; }
	const Number &operator()(int c, int r) const { return _data[c][r]; }

	Matrix<Number> T() const {
		Matrix<Number> ret(rows(), cols());
		for (unsigned int i_col = 0; i_col < cols(); ++i_col)
			for (unsigned int i_row = 0; i_row < rows(); ++i_row)
				ret(i_row, i_col) = (*this)(i_col, i_row);
		return ret;
	}

	Matrix<Number> operator*(const Matrix<Number> &m) {
		Matrix<Number> ret;
		mul(ret, *this, m);
		return ret;
//
//		for (unsigned int i_col = 0; i_col < m.cols(); ++i_col) {
//			for (unsigned int i_row = 0; i_row < rows(); ++i_row) {
//				ret(i_col, i_row) = 0;
//				for (unsigned int k = 0; k < cols(); ++k) {
//					ret(i_col, i_row) += m(i_col, k) * (*this)(k, i_row);
//				}
//			}
//		}
//		return ret;
	}

	std::vector<Number> operator*(const std::vector<Number> &v) {
		std::vector<Number> ret;
		mul(ret, *this, v);
		return ret;

//		assert(cols() == v.size());
//
//		std::vector<Number> ret(rows());
//
//		for (unsigned int i_row = 0; i_row < rows(); ++i_row) {
//			ret[i_row] = 0;
//			for (unsigned int i_col = 0; i_col < cols(); ++i_col) {
//				ret[i_row] += (*this)(i_col, i_row) * v[i_row];
//			}
//		}
//		return ret;
	}

	unsigned int cols() const { return _data.size(); }
	unsigned int rows() const { return _data[0].size(); }



	Matrix<Number> inverse_ntl() const {
		assert(cols() == rows());

		NTL::ZZ_pPush( NTL::conv<NTL::ZZ>(Number::global_p()) );
		NTL::ZZ_p::init( NTL::conv<NTL::ZZ>(Number::global_p()) );

		NTL::mat_ZZ_p S;
		S.SetDims(cols(), rows());

		for (unsigned int i_row = 0; i_row < rows(); i_row++) {
			for (unsigned int i_col = 0; i_col < rows(); i_col++) {
				S[i_row][i_col] = NTL::conv<NTL::ZZ_p>( (*this)(i_row, i_col).to_int() );
			}
		}

		NTL::mat_ZZ_p R;
		R.SetDims(cols(), rows());

		R = inv(S);

		Matrix<Number> ret(cols(), rows());
		for (unsigned int i_row = 0; i_row < rows(); i_row++) {
			for (unsigned int i_col = 0; i_col < rows(); i_col++) {
				ret(i_row,i_col).from_int( NTL::conv<int>(R[i_row][i_col]) );
			}
		}
		return ret;
	}

	Matrix<Number> inverse() const {
		Matrix<Number> ret(cols(), rows());
		Matrix<Number> copy(cols(), rows());

		for (unsigned int i_col = 0; i_col < cols(); ++i_col) {
			for (unsigned int i_row = 0; i_row < rows(); ++i_row) {
				ret(i_col, i_row) = Number::static_from_int( ((i_col == i_row) ? 1 : 0) );
				copy(i_col, i_row) = (*this)(i_col, i_row);
			}
		}

//		std::cout << "copy = " << copy << std::endl;
//		std::cout << "ret = " << ret << std::endl;

		std::function<void(int,int,int)> add_to_row = [&ret, &copy](int to, int from, int factor) {
			for (unsigned int i_col = 0; i_col < ret.cols(); ++i_col) {
				ret(i_col, to) += ret(i_col, from) * factor;
				copy(i_col, to) += copy(i_col, from) * factor;
			}
		};

		for (unsigned int i_row = 0; i_row < rows(); ++i_row) {
			unsigned int copy_from = i_row + 1;
			while ((copy(i_row, i_row).to_int() == 0) && (copy_from < rows())) {
//				std::cout << "has 0 on the diagonal. addin line " << copy_from << " to line copy to" << std::endl;

				add_to_row(i_row, copy_from, 1);
				++copy_from;

//				std::cout << "copy = " << copy << std::endl;
//				std::cout << "ret = " << ret << std::endl;
			}
			if (i_row >= rows())
				throw InverseRuntimeError(std::string("Matrix is singular"));
			if (copy(i_row, i_row).to_int() == 0)
				throw InverseRuntimeError(std::string("Matrix is singular"));

			Number inv = power(copy(i_row, i_row), copy(i_row, i_row).p() - 2);

//			std::cout << "normalizing row " << i_row << std::endl;
			for (unsigned int i_col = 0; i_col < cols(); ++i_col) {
				ret(i_col, i_row) *= inv;
				copy(i_col, i_row) *= inv;
			}
//			std::cout << "copy = " << copy << std::endl;
//			std::cout << "ret = " << ret << std::endl;

			for (unsigned int eliminate = i_row + 1; eliminate < rows(); ++eliminate) {
//				std::cout << "eliminating column " << i_row << " on row " << eliminate << " = " << copy(i_row, eliminate).to_int() << std::endl;

				add_to_row(eliminate, i_row, -copy(i_row, eliminate).to_int());

//				std::cout << "copy = " << copy << std::endl;
//				std::cout << "ret = " << ret << std::endl;
			}
		}

		for (unsigned int i_row = rows() - 1; i_row > 0; --i_row) {
			for (int eliminate = i_row - 1; eliminate >= 0; --eliminate) {
//				std::cout << "back eliminating column " << i_row << " on row " << eliminate << " = " << copy(i_row, eliminate).to_int() << std::endl;

				add_to_row(eliminate, i_row, -copy(i_row, eliminate).to_int());

//				std::cout << "copy = " << copy << std::endl;
//				std::cout << "ret = " << ret << std::endl;
			}
		}

		return ret;
	}

};

template<class Out, class In1, class In2>
void mul(Matrix<Out> &out, const Matrix<In1> &in1, const Matrix<In2> &in2) {
	assert(in1.cols() == in2.rows());
	assert((void*)&out != (void*)&in1);
	assert((void*)&out != (void*)&in2);
	out.resize(in2.cols(), in1.rows());

	for (unsigned int i_col = 0; i_col < in2.cols(); ++i_col) {
		for (unsigned int i_row = 0; i_row < in1.rows(); ++i_row) {
			out(i_col, i_row) = 0;
			for (unsigned int k = 0; k < in1.cols(); ++k) {
				out(i_col, i_row) += in2(i_col, k) * in1(k, i_row);
			}
		}
	}
}

template<class Out, class In1, class In2>
void mul(std::vector<Out> &out, const Matrix<In1> &m, const std::vector<In2> &v) {
	assert(m.cols() == v.size());
	assert((void*)&out != (void*)&v);

	out.resize(m.rows());

	for (unsigned int i_row = 0; i_row < m.rows(); ++i_row) {
		out[i_row] = 0;
		for (unsigned int i_col = 0; i_col < m.cols(); ++i_col) {
//			out[i_row] +=  m(i_col, i_row) * v[i_col];
			out[i_row] +=  v[i_col] * m(i_col, i_row);
		}
	}
}



template<class Number>
void draw(Matrix<Number> &m) {
	for (unsigned int i = 0; i < m.cols(); ++i)
		for (unsigned int j = 0; j < m.cols(); ++j) {
			int r = random();
			m(i,j).from_int(r);
		}
}





template<class Number>
std::ostream &operator<<(std::ostream &s, const Matrix<Number> &m) {
	for (unsigned int i_row = 0; i_row < m.rows(); ++i_row) {
		for (unsigned int i_col = 0; i_col < m.cols(); ++i_col) {
			s << m(i_col, i_row).to_int() << " ";
		}
		s << std::endl;
	}
	return s;
}

#endif
