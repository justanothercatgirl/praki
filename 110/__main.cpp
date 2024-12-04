#include <iostream>

#include "include/praktable.hpp"

using table = prak::table<double>;
using vecarg = const std::vector<f64> &;
using f64p = prak::pvalue<f64>;
table data;

const f64p g = {9.815710602, 0.001};

f64 get(std::string key) {
	return data[key, 0];
}

// [0] = x0
f64 getsqrt(vecarg v) {
	return std::sqrt(std::abs(get("x0_1") - v[0]));
}

f64 a_A(vecarg a) {
	return 2/a[0]/a[0];
}

f64 J_mrga(vecarg a) {
	return a[0] * a[1] * a[1] * (a[2]/a[3] - 1);
}

f64 ξ_x012(vecarg x) {
	return (x[1] - x[0]) / (2*x[2] - x[0] - x[1]);
}

f64 Mfr_mgRξ(vecarg a) {
	return a[0] * a[1] * a[2] * a[3];
}

f64 J_mrgtx034(vecarg a) {
	return a[0]*a[1]*a[1]*(a[2]*a[3]*a[3] / 2 / std::pow(std::sqrt(a[4]-a[6]) - std::sqrt(a[4]-a[5]), 2) - 1);
}

table ex1(std::string s) {
	table ret(s);
	ret.add_column("x1", std::vector<f64>(ret.rows, NAN))
		.add_column("t", std::vector<f64>(ret.rows, NAN))
		.add_column("st", std::vector<f64>(ret.rows, NAN))
		.add_column("sqrt", std::vector<f64>(ret.rows, NAN))
		.apply(prak::avg<f64>, {"x11", "x12", "x13"}, "x1")
		.apply(prak::avg<f64>, {"t1", "t2", "t3"}, "t")
		.apply(prak::stddev<f64>, {"t1", "t2", "t3"}, "st")
		.apply(getsqrt, {"x3"}, "sqrt")
		.delete_cols({"x11", "x12", "x13", "t1", "t2", "t3"});
	auto [A, B] = ret.least_squares_linear("sqrt", "t", "st", std::nullopt);

	f64p 	m1 =	{get("m1"), 0.00001},
		R =	{get("R2"), 0.00005},
		x0 =	{get("x0_1"), 0.01},
		x1 =	{ret.col_avg("x1"), 0.01 * std::sqrt(ret.rows)},
		x2 =	{get("x2_1"), 0.01};

	/*std::cout << x0 << '\n' << x1 << '\n' << x2 << '\n';*/
	f64p a = prak::function<f64>(a_A, {A});
	f64p J = prak::function<f64>(J_mrga, {m1, R, g, a});
	f64p ξ = prak::function<f64>(ξ_x012, {x0, x1, x2});
	f64p Mfr = prak::function<f64>(Mfr_mgRξ, {m1, g, R, ξ});
	std::cout << ret 
		  << "\na = " << a
		  << "\nJ = " << J
		  << "\nξ = " << ξ
		  << "\nМомент трения Mfr = " << Mfr 
		  << std::endl;
	return ret;
}

void ex2(std::string data) {
	table table(data);
	f64p	R0 = {get("R2_0"), 0.00005},
		R1 = {get("R2_1"), 0.00005},
		m0 = {get("m2_0"), 0.00001},
		m1 = {get("m2_1"), 0.00001},
		x0 = {get("x0_2"), 0.01},
		x3 = {get("x3_2"), 0.01},
		x4 = {get("x4_2"), 0.01};

	table	.add_column("t", std::vector<f64>(table.rows, NAN))
	 	.add_column("st", std::vector<f64>(table.rows, NAN))
	 	.add_column("Ji", std::vector<f64>(table.rows, NAN))
	 	.add_column("sJi", std::vector<f64>(table.rows, NAN))
		.add_column("0", std::vector<f64>(table.rows, 0.0))
		.apply(prak::avg<f64>, {"t1", "t2", "t3"}, "t")
		.apply(prak::stddev<f64>, {"t1", "t2", "t3"}, "st")
		.delete_cols({"t1", "t2", "t3"})
		;
	for (size_t i = 0; i < table.rows; ++i) {
		/* mrgtx034 */
		std::vector<f64p> args = {
			table["M", i] == 0 ? m0 : m1, 
			table["R", i] == 0 ? R0 : R1,
			g, {table["t", i], table["st", i]},
			// я проебался и только на первом измерении у меня x4 = 4см, на остальных 10см
			x0, x3, table["M", i] == 0 && table["R", i] == 0 ? f64p{0.04, 0.01} : x4,
		};
		auto [val, err] = prak::function<f64>(J_mrgtx034, args);
		table["Ji", i] = val;
		table["sJi", i] = err;
	}
	table.write_plot("ex2_1.plot", "Ji", "0", "sJi");
	table.multiply_column("sJi", 1 / std::sqrt(1 - 0.75));
	table.write_plot("ex2_2.plot", "Ji", "0", "sJi");
	std::cout << table;
}

int main() {
	data = table("common");
	ex1("data1");
	ex2("data2");
	return 0;
}
