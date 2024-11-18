#pragma once

#include "prakcommon.hpp"

#include <cstring>
#include <iomanip>
#include <ostream>
#include <vector>

namespace prak {

struct dimension_error : public std::logic_error { 
	explicit dimension_error(const char *what): std::logic_error(what) {} 
};

/// A class to represent a matrix. Works as an extension to std::vector, so all methods of std::vector are available on this class
/// BIG TODO: replave `throw` with `std::optional` return type
template <typename T>
struct matrix : public std::vector<T> {
	size_t rows, cols;
	size_t feq_precision = 10;
	const T &SUBSCR_OPRTR(size_t row, size_t col) const {
		return this->at(row * cols + col);
	}
	T &SUBSCR_OPRTR(size_t row, size_t col) {
		return this->at(row * cols + col);
	}
	matrix(void) = default;
	matrix(struct matrix<T> &&) = default;
	matrix(const struct matrix<T> &) = default;
	struct matrix<T> &operator=(const struct matrix<T> &) = default;
	explicit matrix(size_t rows, size_t cols, T def)
		: rows{rows}, cols{cols}, std::vector<T>(rows * cols, def) {}
	explicit matrix(size_t rows, size_t cols, T *data = nullptr)
		: rows{rows}, cols{cols}, std::vector<T>(rows * cols) {
		if (data != nullptr) std::memcpy(this->data(), data, rows * cols);
	}
	explicit matrix (size_t rows, size_t cols, std::vector<T> &&data) noexcept(false)
		: rows{rows}, cols{cols}, std::vector<T>(std::move(data)) {
		if (this->size() != rows * cols) throw std::out_of_range("data.size() != rows*cols");
	}
	/// add 2 matrixes
	struct matrix<T> operator+(const struct matrix<T> &other) noexcept(false) {
		if (other.rows != rows || other.cols != cols) 
			throw dimension_error("matrix add: dimensions of 'other' do not match dimensions of 'this'");
		struct matrix<T> ret(rows, cols);
		for (size_t i = 0; i < this->size(); ++i) {
			ret.data()[i] = this->data()[i] + other.data()[i];
		}
		return ret;
	}
	/// perform matrix multiplication
	struct matrix<T> operator*(const struct matrix<T> &other) noexcept(false) {
		if (cols != other.rows) 
			throw dimension_error("Matrix mul: 'other.rows' does not match 'this.cols'");
		struct matrix<T> ret(rows, other.cols, T{});
		for (size_t i = 0; i < rows; ++i) {
			for (size_t j = 0; j < other.cols; ++j) {
				for (size_t k = 0; k < cols; ++k) 
					ret.SUBSCR_OPRTR(i, j) += SUBSCR_OPRTR(i, k) * other.SUBSCR_OPRTR(k, j);
			}
		}
		return ret;
	}
	/// multiply matrix by a vector [other.size() x 1]
	std::enable_if<std::is_arithmetic_v<T>, std::vector<T>>
	operator*(const std::vector<T> &other) noexcept(false) {
		if (cols != other.size()) throw dimension_error("Matrix mul: 'other.rows' does not match 'this.cols'");
		std::vector<T> ret(rows, T{});
		for (size_t row = 0; row < rows; ++rows) {
			for (size_t k = 0; k < cols; ++k)
				ret[row] += SUBSCR_OPRTR(row, k) * other[k];
		}
		return ret;
	}
	/// multiply matrix by a scalar
	template <typename Arg>
	std::enable_if_t<std::is_arithmetic_v<Arg>, struct matrix<T>> 
	operator*(const Arg other) const {
		struct matrix<T> ret(rows, cols);
		for (size_t i = 0; i < this->size(); ++i)
			ret.data()[i] = this->data()[i] * other;
		return ret;
	}
	/// Tries to find an inverse of the matrix by using gaussian elimination
	/// TODO: handle the case where the inverse does not exist
	std::enable_if_t<std::is_floating_point_v<T>, std::optional<struct matrix<T>>>
	inv(void) const noexcept(false) {
		if (rows != cols) throw dimension_error("Matrix must be square to be inverted");
		struct matrix<T> gaussian(rows, cols * 2, T{});
		for (size_t row = 0; row < rows; ++row) {
			std::memcpy(gaussian.data() + (row * cols * 2 ), this->data() + row * cols, cols * sizeof (T));
			gaussian.data()[row * cols * 2 + cols + row] = 1;
		}
		for (size_t row = 0; row < rows; ++row) {
			T &fst_row_v = gaussian.SUBSCR_OPRTR(row, row);
			bool row_found = true;
			if (fequal<T>(fst_row_v, 0, feq_precision)) row_found = gaussian.find_suitable_row(row);
			if (!row_found) return std::nullopt;
			gaussian.mul_row(row, 1/gaussian.SUBSCR_OPRTR(row, row));
			for (size_t pr_row = 0; pr_row < rows; ++pr_row) {
				if (row == pr_row) continue;
				T& row_n_v = gaussian.SUBSCR_OPRTR(pr_row, row);
				if (fequal<T>(row_n_v, 0, feq_precision)) continue;
				gaussian.muladd_row(row, pr_row, -row_n_v/fst_row_v);
			}
		}
		struct matrix<T> ret(rows, cols);
		for (size_t row = 0; row < rows; ++row) {
			std::memcpy(ret.data() + row * cols, gaussian.data() + row * 2*cols + cols, cols * sizeof (T));
		}
		return ret;

	}
	struct matrix<T> tr(void) const {
		struct matrix<T> ret(cols, rows);
		for (size_t row = 0; row < rows; ++row)
			for (size_t col = 0; col < cols; ++col)
				ret.SUBSCR_OPRTR(col, row) = SUBSCR_OPRTR(row, col);
		return ret;
	}
	/// swaps rows `r1`, `r2`
	void swap_rows(size_t r1, size_t r2) {
		T* tmpbuf = new T[cols];
		size_t rowsize = cols * sizeof (T);
		std::memcpy(tmpbuf, this->data() + cols * r1, rowsize);
		std::memcpy(this->data() + cols * r1, this->data() + cols * r2, rowsize);
		std::memcpy(this->data() + cols * r2, tmpbuf, rowsize);
		delete[] tmpbuf;
	}
	/// multiplies `r_src` row by number `mul` and adds it ro row `r_dest`
	void muladd_row(size_t r_src, size_t r_dest, T mul) {
		for (size_t col = 0; col < cols; ++col) {
			size_t r_src_ofs = cols * r_src + col;
			size_t r_dest_ofs = cols * r_dest + col;
			this->data()[r_dest_ofs] += this->data()[r_src_ofs] * mul;
		}
	}
	/// multiplies a row by a number
	void mul_row(size_t r1, T mul) {
		for (size_t ind = r1 * cols; ind < (r1 + 1) * cols; ++ind)
			this->data()[ind] *= mul;
	}
	/// finds a row that contains non-zero number at index [row = i > `row`, column=`row`].
	/// will only search for the rows below `row` and swap them
	/// returns whether the swap occured (whether the row was found)
	bool find_suitable_row(size_t row) {
		for (size_t i = row + 1; i < rows; ++i) {
			if (SUBSCR_OPRTR(i, row) != 0) { 
				swap_rows(i, row);
				return true;
			}
		}
		return false;
	}
	
	static struct matrix<T> identity(size_t rank)  {
		struct matrix<T> ret(rank, rank);
		for (size_t i = 0; i < rank * rank; i += (rank + 1)) {
			ret.data()[i] = 1;
		}
		return ret;
	}

	std::ostream &print(std::ostream &out = std::cout, size_t width = 10) const {
		for (size_t row = 0; row < rows; ++row) {
			for (size_t col = 0; col < cols; ++col)
				out << "|" << std::setw(width) << SUBSCR_OPRTR(row, col); 
			std::cout << "|\n";
		}
		return out;
	}
};

template<typename T>
void operator/(const int x, const struct matrix<T> &m) {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

template <typename T>
std::ostream &operator<<(std::ostream &out, const struct matrix<T> &m) {
	return m.print(out);
}

} // namespace prak
