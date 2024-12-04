#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <algorithm>
#include <optional>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include <cmath>


#include "prakcommon.hpp"
#include "prakmath.hpp"

namespace prak {

/// truncate a string to n characters, or return in unmodified
inline std::string truncate_or(const std::string &str, size_t n) {
	if (str.size() >= n) return str.substr(0, n);
	return str;
}


/// Unused for now.
/// TODO: Remove
template <typename T> 
struct opt_value {
	enum struct t: unsigned char {
		val	= 0,
		up	= 1 << 0, 
		left	= 1 << 1, 
		down	= 1 << 2, 
		right	= 1 << 3,
	} tag;
	T val;

	explicit opt_value(const T &v): tag{t::val}, val{v} {}
	explicit opt_value(const opt_value::t t): tag{t} {}
	opt_value &operator=(const T &v) {
		val = v;
		tag = t::val;
		return *this;
	}
	opt_value &operator=(const opt_value::t t) {
		tag = t;
		return *this;
	}

};


/// Class that can store, print, and apply operations to a table used by lab works
template <typename dtype>
class table {
	std::vector<dtype> data;

	using function_type = function_t<dtype>;
	using stringvec = std::vector<std::string>;

	size_t index(const std::string &str) const noexcept(false) {
		auto ret = std::distance(names.cbegin(), std::find(names.cbegin(), names.cend(), str));
		if (ret == columns) throw std::out_of_range("Column " + str + " does not exist");
		return ret;
	}

	FILE* open_gnuplot() {
#if defined(_POSIX_VERSION) || defined(__unix)
		return popen("gnuplot", "w");
#else
#warning Not implemented for non-unix operating systems
		return NULL;
#endif
	}

public:
	size_t rows = 0, columns = 0;
	friend class iterator;
	struct iterator {
		table *parent;
		size_t columns;
		size_t col_index = 0, data_index = 0;
		using iterator_category = std::forward_iterator_tag;
		using value_type = dtype;
		using difference_type = ptrdiff_t;
		using pointer = dtype *;
		using reference = dtype &;
		iterator(table *new_parent, const std::string &column, size_t row_idx = 0) 
		 	: parent{new_parent}, columns(new_parent->columns) 
		{
			col_index = parent->index(column);
			data_index = col_index + row_idx * new_parent->columns;
		}
		iterator &operator++() { data_index += columns; return *this; }
		iterator operator++(int) { iterator ret = *this; ++(*this); return ret; }
		iterator &operator+(int x) { data_index += columns * x; return *this; }
		bool operator==(iterator other) { return data_index == other.data_index && parent == other.parent && col_index == other.col_index; }
		bool operator!=(iterator other) { return data_index != other.data_index || parent != other.parent || col_index != other.col_index; }
		value_type &operator*() { return parent->data[data_index]; };
	};
	/// Optional rownames: names of rows
	std::vector<std::string> opt_rownames;
	/// Mandatory columnnames: names of columns
	std::vector<std::string> names;
	/// width used for printing, defaults to 8
	size_t column_width = 12;

	/// default constructor
	table() = default;

	explicit table(const std::vector<std::string> &columns, size_t rows, const dtype &deflt) 
		: rows{rows}, columns{columns.size()} 
	{
		names = columns;
		data = std::vector<dtype>(rows * columns.size(), deflt);
	}

	/// Data: array of ararys, not freed automatically.
	explicit table(const std::vector<std::string> &columns, dtype **data, size_t rows /*size_t columns = strings.size()*/)
		: rows{rows}, columns{columns.size()} 
	{
		names = columns;

		data = std::vector<dtype>(rows * columns.size());
		for (size_t i = 0; i < rows; ++i)
			for (size_t j = 0; j < columns.size(); ++j)
				data[i * columns.size() + j] = data[i][j];
	}
	
	/// Strings: names for columns
	/// `new_data` format: { { entry1_a, entry2_a, ...} { entry1_b, entry2_b, ... }, ... }
	/// where `a`, `b`, ... are columns
	explicit table(std::vector<std::string> &&strings, std::vector<std::vector<dtype>> &&new_data) 
		: rows{new_data.size() ? new_data[0].size() : 0}, columns{strings.size()} 
	{
		names = strings;
		data = std::vector<dtype>(new_data.size() * rows);
		auto dit = data.begin();
		for (size_t j = 0; j < rows; ++j) {
			for (size_t i = 0; i < new_data.size(); ++i) {
				*dit = new_data[i][j];
				++dit;
			}
		}
	}

	explicit table(const std::string &file) {
		read(file);
	}

	iterator begin(std::string column) { return iterator(this, column); }
	iterator end(std::string column) { return iterator(this, column, rows); }

	dtype & SUBSCR_OPRTR (const std::string &column, size_t row) noexcept(false) {
		size_t i = index(column);
		return data.at(names.size() * row + index(column));
	}

	dtype & SUBSCR_OPRTR (size_t column, size_t row) {
		return data.at(names.size() * row + column);
	}

	/// prints a table. defaults to using std::cout, but any std::ostream can be passed in it.
	void print(std::ostream &stream = std::cout) const {
		stream << "columns: " << columns << ", rows: " << rows << std::endl;
		bool print_names = opt_rownames.size() == data.size() / columns;
		size_t topchars = (column_width + 1) * (columns + print_names) + 1;

		std::string rowsep(column_width, '_');
		for (size_t i = 0; i < columns + print_names - 1; ++i) {
			rowsep.append(1, '|');
			rowsep.append(column_width, '_');
		}

		stream << std::left << std::string(topchars, '_') << std::endl;
		if (print_names) stream << '|' << std::setw(column_width) << ' ';

		for (const auto &s : names)
			stream << '|' << std::setw(column_width) << truncate_or(s, column_width-1);

		for (size_t i = 0; auto x : data) {
			if (i % columns == 0) {
				stream << '|' << std::endl;
				stream << '|' << rowsep << '|' << std::endl;
				if (print_names) 
					stream << '|' << std::setw(column_width) << truncate_or(opt_rownames[i / columns], column_width);
			}
			stream << '|' << std::setw(column_width) << x;
			++i;
		}
		stream << '|' << std::endl << '|' << rowsep << '|' << std::endl;
	}

	/// Returns whether the amount of names is correct
	/// If it is incorrect, names won't be displayed during printing
	bool set_rownames(std::vector<std::string> &&names) {
		opt_rownames = names;
		return opt_rownames.size() == data.size() / names.size();
	}

	/// apply a function to several columns and store result in another column
	/// function must accept std::vector or arguments
	table &apply(function_type function, stringvec args, std::optional<std::string> result) {
		size_t result_index = result.has_value() ? index(*result) : 0;
		for (size_t i = 0; i < rows; ++i) {
			std::vector<dtype> v(args.size());
			for (size_t j = 0; j < args.size(); ++j) 
				v[j] = SUBSCR_OPRTR(args[j], i);
			if (result.has_value()) data[columns * i + result_index] = function(v);
			else (void)function(v);
		}
		return *this;
	}

	table &apply_n(function_type function, std::vector<iterator> cols, size_t n, std::optional<std::string> result) {
		size_t result_index = result.has_value() ? index(*result) : 0;
		for (size_t i = 0; i < n; ++i) {
			std::vector<dtype> v(cols.size());
			for (size_t j = 0; j < cols.size(); ++j) 
				v[j] = *cols[j]++;
			if (result.has_value()) data[columns * i + result_index] = function(v);
			else (void)function(v);
		}
		return *this;
	}

	table &apply_function_n(
		function_type function, 
		std::vector<iterator> cols, 
		std::vector<iterator> sigma_cols, 
		size_t n, 
		const std::string &resval, 
		const std::string &ressigma) 
	{
		if (cols.size() != sigma_cols.size()) 
			throw dimension_error("cols.size() is not equal to sigma_cols.size()");
		size_t	val_index = index(resval),
			sgm_index = index(ressigma);
		for (size_t i = 0; i < n; ++i) {
			std::vector<dtype> v(cols.size());
			std::vector<dtype> s(cols.size());
			for (size_t j = 0; j < cols.size(); ++j) {
				v[j] = *cols[j]++;
				s[j] = *sigma_cols[j]++;
			}
			data[columns * i + val_index] = function(v);
			data[columns * i + sgm_index] = sigma(function, v, s);
		}
		return *this;
	}
	/// adds a column with name `name` and data `column_data`
	table &add_column(const std::string &name, std::vector<dtype> column_data) {
		if (column_data.size() == 0) column_data = std::vector<dtype>(rows, dtype{});
		std::vector<dtype> data_new(rows * (++columns));

		for (size_t row = 0; row < rows; ++row) {
			for (size_t column = 0; column < names.size(); ++column) // columns variable is incremented already
				data_new[row * columns + column] = data[row * names.size() + column];
			data_new[(row + 1) * columns - 1] = column_data[row];
		}
		data = std::move(data_new);
		names.push_back(name);
		return *this;
	}
	table &add_column(const std::string &name, dtype dflt = dtype{}) {
		return add_column(name, std::vector<dtype>(rows, dflt));
	}

	/// Deletes a column from a table.
	table &delete_col(const std::string &colname) {
		std::vector<dtype> data_new(rows * (--columns));
		size_t idx = index(colname);
		for (size_t column = 0; column < names.size(); ++column) {
			if (column == idx) continue;
			size_t _col = column - (column > idx);
			for (size_t row = 0; row < rows; ++row)
				data_new[row * columns + _col] = data[row * names.size() + column];
		}
		data = std::move(data_new);
		names.erase(names.begin() + idx);
		return *this;
	}

	/// Deletes several columns 
	table &delete_cols(const stringvec &cols) noexcept(false) {
		size_t columns_new = columns - cols.size();
		std::vector<dtype> data_new(rows * columns_new);
		std::set<size_t> idxs;
		for (const std::string &col : cols)
			idxs.insert(index(col));

		size_t skipped = 0;
		for (size_t column = 0; column < columns; ++column) {
			if (idxs.count(column) != 0) {
				++skipped;
				continue;
			}
			size_t _col = column - skipped;
			for (size_t row = 0; row < rows; ++row)
				data_new[row * columns_new + _col] = data[row * columns + column];
		}
		stringvec names_new = stringvec(columns_new);
		for (size_t i = 0; const std::string &name : names)
			if (idxs.count(index(name)) == 0) names_new[i++] = name;
		names = std::move(names_new);
		data = std::move(data_new);
		columns = columns_new;
		return *this;
	}


	/// Appends a row to the table. if name is set, appends it to `opt_rownames`
	void add_row(std::vector<dtype> values, std::optional<std::string> name = std::nullopt) {
		if (values.size() == 0) values = std::vector<dtype>(columns, dtype{});
		data.resize(columns * (++rows));
		std::copy_n(values.cbegin(), columns, data.end() - columns);
		if (name.has_value()) opt_rownames.push_back(*name);
	}


	friend std::ostream& operator<<(std::ostream &os, table<dtype> &t) {
		t.print(os);
		return os;
	}

	/// Reads a table from a file in a format:
	/// ```
	/// col1 col2 col3 ...
	/// val1 val2 val3 ...
	/// val4 val5 val6 ...
	/// ...
	/// ```
	/// Note tha `val` may either be a real number or a question mark, denoting that the value is unknown
	/// `col` may be any string without whitespaeces.
	/// if the first column is named "__name__" (as in python), first val in each row is a string used as
	/// a row name.
	void read(std::ifstream& f) {
		std::string header;
		std::getline(f >> std::ws, header);
		std::istringstream h_stream(header);
		std::string buffer;

		bool read_names = false;
		h_stream >> buffer;
		if (buffer == "__name__") read_names = true;
		else names.push_back(buffer);

		for (size_t i = read_names ? 0 : 1; h_stream >> buffer; ++i) 
			names.push_back(buffer);
		
		std::vector<dtype> tmp_row(names.size());
		int __i = 0;
		while (!(f >> std::ws).eof()) {
			++__i;
			if (read_names) { 
				f >> buffer;
				opt_rownames.push_back(buffer);
			}
			for (auto it = tmp_row.begin(); it != tmp_row.end(); ++it) {
				if ((f >> std::ws).peek() == '?') {
					*it = NAN;
					f >> buffer;
				}
				else f >> *it;
			}
			data.resize(data.size() + names.size());
			std::copy_n(tmp_row.cbegin(), names.size(), data.end() - names.size());
			++rows;
		}
		columns = names.size();
	}
	
	/// Reads a table from a file specified by `path`. 
	/// For details, refer to documentation of `void read(std::ifstream&)` overload
	void read(const std::string &path) {
		std::ifstream f(path);
		read(f);
	}

	/// Fills a specified column with the same value `v`
	table &fill_column(const std::string &column, dtype v) {
		apply([&v](const std::vector<dtype>& _) -> dtype { return v; }, {}, column);
		return *this;
	}

	table &copy_column(const std::string &src, const std::string dest) {
		size_t si = index(src), di = index(dest);
		for (size_t i = 0; i < rows; ++i) 
			data[i*columns + di] = data[i*columns + si];
		return *this;
	}
	
	template <typename mult>
	table &multiply_column(const std::string &column, mult s) {
		size_t i;
		for (i = index(column); i < rows * columns; i += columns)
			data[i] *= s;
		return *this;
	}

	/// returns an std::pair with coefficients A and B in that order
	std::pair<prak::pvalue<dtype>, prak::pvalue<dtype>> 
	least_squares_linear(std::string x, std::string y, std::optional<std::string> sigma, std::optional<dtype> sigma_fixed) 
	noexcept(false) {
		if (sigma.has_value() == sigma_fixed.has_value()) 
			throw std::invalid_argument("sigma and sigma_fixed can't both have (no) value");
		prak::vector<dtype> _x(rows);
		prak::vector<dtype> _y(rows);
		prak::vector<dtype> _s(rows);

		std::copy(begin(x), end(x), _x.begin());
		std::copy(begin(y), end(y), _y.begin());
		if (sigma.has_value()) std::copy(begin(*sigma), end(*sigma), _s.begin());
		else _s = prak::vector<dtype>(rows, static_cast<dtype>(*sigma_fixed));

		std::pair<prak::pvalue<dtype>, prak::pvalue<dtype>> ret;
		prak::least_squares_linear<dtype>(_x, _y, _s, ret.first, ret.second);
		return ret;
	}

	/// calculate an average of the column 
	dtype col_avg(const std::string &column) {
		dtype accum = dtype{};
		for (auto it = begin(column); it != end(column); ++it) 
			accum += *it;
		return accum / rows;
	}

	/// calculate standard deviation of the column
	dtype col_stddev(const std::string &column) {
		dtype accum = dtype{};
		dtype avg = col_avg(column);
		for (auto it = begin(column); it != end(column); ++it)
			accum += (*it - avg)*(*it - avg);
		return std::sqrt(accum);
	}

	/// takes columns [columns], calculates average and standard deviation for each row, puts them into `avg` and `stddev` and deleted original columns
	/// if create_columns is true, creates columns avg and stddev
	/// This is common thing to do, so might as well write a function for that
	table& into_avgstddev(const std::vector<std::string> &columns, const std::string &avg_out, const std::string &stddev_out, bool create_columns = false) {
		if (create_columns) {
			add_column(avg_out, std::vector<dtype>(rows, dtype{}));
			add_column(stddev_out, std::vector<dtype>(rows, dtype{}));
		}
		apply(avg<dtype>, columns, avg_out);
		apply(stddev<dtype>, columns, stddev_out);
		delete_cols(columns);
		return *this;
	}

	/// applies a function `func` to arguments in columns `args`, stores the result in column `result` and standard error in column `result_sigma`.
	/// `sigmas` must be in a 1-to-1 correspondance with `args`
	table& apply_with_err(function_t<dtype> func, const stringvec &args, const stringvec &sigmas, const std::string &result, const std::string result_sigma) {
		if (args.size() != sigmas.size()) throw dimension_error("Args and Sigmas did not have the same dimentinons");
		size_t	result_index = index(result),
			sigma_index = index(result_sigma);
		for (size_t i = 0; i < rows; ++i) {
			std::vector<dtype> __args(args.size()),
					   __sgms(args.size());
			for (size_t j = 0; j < args.size(); ++j) { 
				__args[j] = SUBSCR_OPRTR(args[j], i);
				__sgms[j] = SUBSCR_OPRTR(sigmas[j], i);
			}
			data[columns * i + result_index] = func(__args);
			data[columns * i + sigma_index] = prak::sigma(func, __args, __sgms);
		}
		return *this;
	}

	size_t find_index(const std::string &column, const dtype &val) {
		size_t col_idx = index(column); 
		for (size_t i = 0; i < rows; ++i) {
			if (data[col_idx + i * columns] == val) return i;
		}
		return col_idx;
	}

	/// Serialize data in format `data[args[0]][i] data[args[1]][i] data[args[2]][i]...`
	void print_plot(const stringvec &args, std::ostream &out = std::cout) const {
		std::vector<size_t> offsets(args.size());
		for (size_t i = 0; i < args.size(); ++i) {
			offsets[i] = index(args[i]);
		}
		for (size_t row = 0; row < rows; ++row) {
			size_t row_offset = columns * row;
			for (const auto column_offset : offsets)
				std::cout << data[row_offset + column_offset] << ' ';
			std::cout << std::endl;
		}
	}

	/// Serialize data in format `data[xs][i] data[ys][i] <data[xss][i]> <data>[yss][i]>`, readable by gnuplot with xyerrorbars
	table &write_plot_4(const std::string &file, const std::string &xs, const std::string &ys, const std::string &xss, const std::string &yss) {
		std::ofstream out(file);
		size_t xi = index(xs), yi = index(ys), xsi = index(xss), ysi = index(yss);
		for (size_t row = 0; row < rows; ++row) {
			size_t offset = columns * row;
			out << data[offset + xi]  << ' ' << data[offset + yi]  << ' ' 
			    << data[offset + xsi] << ' ' << data[offset + ysi] << std::endl;
		}
		return *this;
	}	

	/// Serialize data in format `data[xs][i] data[ys][i] <data[ss][i]>`, readable by gnuplot with yerrorbars
	table &write_plot(const std::string &xs, const std::string &ys, std::optional<std::string> yss = std::nullopt, std::ostream &out = std::cout) {
		size_t nosigma = std::numeric_limits<size_t>::max();
		size_t xsi = index(xs), ysi = index(ys), ssi = nosigma;
		if (yss.has_value()) ssi = index(*yss);
		for (size_t row = 0; row < rows; ++row) {
			size_t offset = columns * row;
			out << data.at(offset + xsi) << ' ' << data.at(offset + ysi);
			if (ssi != nosigma) out << ' ' << data[offset+ssi];
			out << std::endl;
		}
		return *this;
	}

	/// Serialize data into a file `file`. For details, refer to documentation for overload with std::ifstream as an argument
	table &write_plot(const std::string &file, const std::string &xs, const std::string &ys, std::optional<std::string> yss = std::nullopt) {
		std::ofstream out(file);
		return write_plot(xs, ys, yss, out);
	}
	
	void plot_png(
		const std::string output_filename, 
		const std::string &x, 
		const std::string &y, 
		const std::optional<std::string> &xlabel = std::nullopt,
		const std::optional<std::string> &ylabel = std::nullopt,
		const std::optional<std::string> &sigma = std::nullopt) {
		// TODO: Finish	
	}	
	
	struct plot {
		std::string x;
		std::string y;
		std::optional<std::string> sigma;
		bool plot_points = true;
		std::optional<std::string> label;
	};

	void plot_png(const std::string output, const std::vector<struct plot> &plots) {
		// TODO: Finish later
		/* FILE *gnuplot = open_gnuplot(); */
		/* fprintf(gnuplot, */ 
		/* 	"set terminal pngcairo enhanced size 800,600 dpi 300\n" */
		/* 	"set output '%s'\n" */
		/* 	, output.data() */
		/* ); */
	}
};

} // namespace prak
