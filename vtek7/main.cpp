#include <iostream>

#include "include/praktable.hpp"

using table = prak::table<f64>;
using f64p = prak::pvalue<f64>;
using f64v = std::vector<f64>;
using vecarg = const std::vector<f64> &;
using argvec = const std::vector<f64> &;

table common;

f64p get(const std::string &key, const std::string &skey) { return {common[key, 0], common[skey, 0]}; }
f64 &get(const std::string &key) { return common[key, 0]; }

f64 lux_err(vecarg a) {
	if (a[0] < 10000) return a[0] * 0.05;
	return a[0] * 0.1;
}
f64 U_err_MS8040(vecarg a) {
	f64 u = a[0];
	if (u < 0.2) return u * 0.0005 + 6e-5;
	if (u < 2) return u * 0.0005 + 6e-4;
	if (u < 20) return u * 0.0005 + 6e-3;
	if (u < 200) return u * 0.0005 + 6e-2;
	return u*0.0005 + 6e-1;
}
f64 I_err_MS8040(vecarg a) {
	f64 i = a[0];
	if (i < 2e-4) return i * 0.0015 + 15e-8;
	if (i < 2e-3) return i * 0.0015 + 15e-7;
	if (i < 2e-2) return i * 0.0015 + 15e-6;
	return i * 0.0015 + 15e-5;
}
f64 U_err_DT320B(vecarg a) {
	f64 u = a[0];
	if (u < 2e-1) return 0.0025*u + 2e-4;
	if (u < 2e0) return 0.005*u + 2e-3;
	if (u < 2e1) return 0.005*u + 2e-2;
	if (u < 2e2) return 0.005*u + 2e-1;
	return 0.005*u + 2e0;
}
f64 I_err_DT830B(vecarg a) {
	f64 i = a[0];
	if (i < 2e-4) return 0.01*i + 2e-7;
	if (i < 2e-3) return 0.01*i + 2e-6;
	if (i < 2e-2) return 0.01*i + 2e-5;
	if (i < 2e-1) return 0.012*i + 2e-4;
	return 0.02 * i + 2e-2;
}
f64 R_err_DT830B(vecarg a) {
	f64 r = a[0];
	if (r < 2e2) return 0.008*r + 2e-1;
	if (r < 2e3) return 0.008*r + 2e0;
	if (r < 2e4) return 0.008*r + 2e1;
	if (r < 2e5) return 0.008*r + 2e2;
	return 0.01*r + 2e3;
}

table ex1(std::string file) {
	std::cout << "Упражнение 1:\n";
	table t(file);
	f64 emacs = get("E_amb_max"),
	    emin  = get("E_amb_min");
	f64p E_amb = {(emacs + emin)/2, (emacs-emin)/2};
	f64 x_true = get("x_true");
	get("E_amb") = E_amb.val;
	get("sE_amb") = E_amb.err;
	t	.multiply_column("L", 1e-2)
		.into_avgstddev({"E1", "E2", "E3"}, "E", "sE", false)
		.apply(lux_err, {"E"}, "sLux")
		.apply([x_true](vecarg a){ return a[0] + x_true; }, {"L"}, "x")
		.apply([E_amb](vecarg a){ return a[0] - E_amb.val; }, {"E"}, "E")
		.apply([E_amb](vecarg a){ return std::sqrt(a[0]*a[0] + E_amb.err*E_amb.err / 3); }, {"sE"}, "sE")
		// Если буду подгонять погрешности, убрать ................................ ^^^ <- эту тройку
		.apply(prak::hypot<f64>, {"sE", "sLux"}, "sSum")
		.apply([](vecarg a){ return 1/a[0]/a[0];  }, {"L"}, "1/L^2")
		.print();
	t.write_plot("ex1_EL.plot", "1/L^2", "E", "sE");
	auto [a, b] = t.least_squares_linear("1/L^2", "E", "sE", std::nullopt);
	std::cout << "\nРезультаты мнк:\n\tA = " << a 
		  << "\n\tB = " << b 
		  << "\n E_фон = " << E_amb << std::endl;
	return t;
}

void ex2(std::string file, table &e1) {
	std::cout << "Упражнение 2:\n";
	table t(file);
	f64 U_amb = get("U_amb");
	f64 I_amb = get("I_amb");
	std::copy_n(e1.begin("E"), e1.rows, t.begin("E"));
	t	.multiply_column("L", 1e-2)
		.multiply_column("I_fd0", 1e-6)
		.apply([U_amb](vecarg a){ return a[0] - U_amb; }, {"U_fd0"}, "U_fd")
		.apply([I_amb](vecarg a){ return a[0] - I_amb; }, {"I_fd0"}, "I_fd")
		.apply(U_err_MS8040, {"U_fd"}, "sU")
		.apply(I_err_MS8040, {"I_fd"}, "sI")
		.write_plot("ex2_UE.plot", "E", "U_fd", "sU")
	 	.write_plot("ex2_IE.plot", "E", "I_fd", "sI")
		.print();
}

void ex3(std::string file) {
	std::cout << "Упражнение 3:\n";
	table t(file);
	t	.multiply_column("I1", 1e-6)
		.multiply_column("I2", 1e-6)
		.apply(U_err_MS8040, {"U1"}, "sU1")
		.apply(U_err_MS8040, {"U2"}, "sU2")
		.apply(I_err_DT830B, {"I1"}, "sI1")
		.apply(I_err_DT830B, {"I2"}, "sI2")
		.write_plot_4("ex3_1.plot", "U1", "I1", "sU1", "sI1")
		.write_plot_4("ex3_2.plot", "U2", "I2", "sU2", "sI2")
		.print();
}

void ex4(std::string file, table &e1) {
	std::cout << "Упражнение 4:\n";
	table t(file);
	f64 U_amb = get("U_amb_4");
	f64 I_amb = get("I_amb_4");
	std::copy_n(e1.begin("E"), e1.rows, t.begin("E"));
	t	.multiply_column("L", 1e-2)
		.multiply_column("I0(1)", 1e-6)
		.multiply_column("I0(2)", 1e-6)
		.add_column("U")
		.add_column("I(1)")
		.add_column("I(2)")
		.apply([U_amb](vecarg a){ return a[0] - U_amb; }, {"U0"}, "U")
		.apply([I_amb](vecarg a){ return a[0] - I_amb; }, {"I0(1)"}, "I(1)")
		.apply([I_amb](vecarg a){ return a[0] - I_amb; }, {"I0(2)"}, "I(2)")
		.apply(U_err_MS8040, {"U0"}, "sU")
		.apply(I_err_MS8040, {"I0(1)"}, "sI(1)")
		.apply(I_err_MS8040, {"I0(2)"}, "sI(2)")
		.write_plot("ex4_UE.plot", "E", "U", "sU")
		.write_plot("ex4_IE_1.plot", "E", "I(1)", "sI(1)")
		.write_plot("ex4_IE_2.plot", "E", "I(2)", "sI(2)")
		.print();
}

void ex6(std::string file, table &e1) {
	std::cout << "Упражнение 6\n";
	table t(file);
	size_t idx = e1.find_index("L", 10);
	f64p a = {0.024, 0.0003},
	     b = {0.038, 0.0003};
	f64p S = prak::function<f64>(prak::prod<f64>, {a, b}),
	     E = {e1["E", idx], e1["sE", idx]};
	f64p Ф = prak::function<f64>([](vecarg a){ return a[0] * a[1] / 683; }, {S, E});
	t	.multiply_column("I", 1e-6)
		.apply(U_err_DT320B, {"U"}, "sU")
		.apply(I_err_MS8040, {"I"}, "sI")
		.add_column("sR", 0)
		.apply(R_err_DT830B, {"R"}, "sR")
		.apply_with_err(prak::prod<f64>, {"U", "I"}, {"sU", "sI"}, "W", "sW")
		.copy_column("W", "Nu")
		.copy_column("sW", "sNu")
		.multiply_column("Nu", 1/Ф.val)
		.apply
			([Ф](vecarg a) { return a[0] * std::sqrt(std::pow(a[1]/a[2], 2) + std::pow(Ф.err/Ф.val, 2)); },
			{"Nu", "sW", "W"}, "sNu"
		)
		.multiply_column("R", 1e-3)
		.write_plot_4("ex6_UI.plot", "U", "I", "sU", "sI")
		.write_plot_4("ex6_nuR.plot", "R", "Nu", "sR", "sNu")
		.print();
}

int main() {
	common = table("data_common");
	table e1 = ex1("data1");	
	ex2("data2", e1);
	ex3("data3");
	ex4("data4", e1);
	ex6("data6", e1);
	return 0;
}
