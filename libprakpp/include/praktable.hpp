#pragma once

#include <cstddef>
#include <functional>
#include <algorithm>
#include <optional>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ostream>
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

	size_t index(const std::string &str) const {
		return std::distance(names.cbegin(), std::find(names.cbegin(), names.cend(), str));
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
		bool operator==(iterator other) { return data_index == other.data_index && parent == other.parent && col_index == other.col_index; }
		bool operator!=(iterator other) { return data_index != other.data_index || parent != other.parent || col_index != other.col_index; }
		value_type operator*() { return parent->data[data_index]; };
	};
	/// Optional rownames: names of rows
	std::vector<std::string> opt_rownames;
	/// Mandatory columnnames: names of columns
	std::vector<std::string> names;
	/// width used for printing, defaults to 8
	size_t column_width = 12;

	/// default constructor
	table() = default;

	/// Data: array of ararys, not freed automatically.
	explicit table(const std::vector<std::string> &strings, dtype **data, size_t rows /*size_t columns = strings.size()*/)
		: rows{rows}, columns{strings.size()} 
	{
		names = strings;

		data = std::vector<dtype>(rows * strings.size());
		for (size_t i = 0; i < rows; ++i)
			for (size_t j = 0; j < strings.size(); ++j)
				data[i * strings.size() + j] = data[i][j];
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

	iterator begin(std::string column) { return iterator(this, column); }
	iterator end(std::string column) { return iterator(this, column, rows); }

	dtype & SUBSCR_OPRTR (const std::string &column, size_t row) {
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
	
	/// adds a column with name `name` and data `column_data`
	void add_column(std::string name, std::vector<dtype> column_data) {
		std::vector<dtype> data_new(rows * (++columns));

		for (size_t row = 0; row < rows; ++row) {
			for (size_t column = 0; column < names.size(); ++column) // columns variable is incremented already
				data_new[row * columns + column] = data[row * names.size() + column];
			data_new[(row + 1) * columns - 1] = column_data[row];
		}
		data = std::move(data_new);
		names.push_back(name);
	}

	/// Appends a column to the table. if name is set, appends it to `opt_rownames`
	void add_row(std::vector<dtype> values, std::optional<std::string> name = std::nullopt) {
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
		while (!(f >> std::ws).eof()) {
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

	/// returns an std::pair with coefficients A and B in that order
	std::pair<prak::pvalue<dtype>, prak::pvalue<dtype>> least_squares_linear(std::string x, std::string y, std::optional<std::string> sigma = std::nullopt) {
		prak::vector<dtype> _x(rows);
		prak::vector<dtype> _y(rows);
		prak::vector<dtype> _s(rows);

		std::copy(begin(x), end(x), _x.begin());
		std::copy(begin(y), end(y), _y.begin());
		if (sigma.has_value()) std::copy(begin(*sigma), end(*sigma), _s.begin());
		else _s = prak::vector<dtype>(rows, static_cast<dtype>(1));

		std::pair<prak::pvalue<dtype>, prak::pvalue<dtype>> ret;
		prak::least_squares_linear<dtype>(_x, _y, _s, ret.first, ret.second);
		return ret;
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

	/// Serialize data in format `data[xs][i] data[ys][i] <data[ss][i]>`, readable by gnuplot with yerrorbars
	void write_plot(const std::string &xs, const std::string &ys, std::optional<std::string> yss = std::nullopt, std::ostream &out = std::cout) const {
		size_t nosigma = std::numeric_limits<size_t>::max();
		size_t xsi = index(xs), ysi = index(ys), ssi = nosigma;
		if (yss.has_value()) ssi = index(*yss);
		for (size_t row = 0; row < rows; ++row) {
			size_t offset = columns * row;
			out << data[offset + xsi] << ' ' << data[offset + ysi];
			if (ssi != nosigma) out << ' ' << data[offset+ssi];
			out << std::endl;
		}
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
