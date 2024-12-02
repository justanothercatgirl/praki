#include "include/praktable.hpp"

using table = prak::table<f64>;
using vecarg = const std::vector<f64> &;
using f64v = std::vector<f64>;
using f64p = prak::pvalue<f64>;

table prepare(std::string f) {
	return table(f)
		.add_column("t", {})
		.add_column("st", {})
		.add_column("h", {})
		.add_column("sh", {})
		.add_column("2T", {})
		.add_column("s2T", {})
		.apply(prak::avg<f64>,		{"t1", "t2", "t3"},	"t")
		.apply(prak::avg<f64>,		{"h1", "h2", "h3"},	"h")
		.apply(prak::avg<f64>,		{"2T1", "2T2", "2T3"},	"2T")
		.apply([] (vecarg a) -> f64 { return std::sqrt(std::pow(prak::stddev<f64>(a), 2) + 0.001*0.001); }, {"t1", "t2", "t3"},	"st")
		.apply([] (vecarg a) -> f64 { return std::sqrt(std::pow(prak::stddev<f64>(a), 2) + 0.100*0.100); }, {"h1", "h2", "h3"},	"sh")
		.apply([] (vecarg a) -> f64 { return std::sqrt(std::pow(prak::stddev<f64>(a), 2) + 0.050*0.050); }, {"2T1", "2T2", "2T3"},"s2T")
		.delete_cols({"t1", "t2", "t3", "h1", "h2", "h3", "2T1", "2T2", "2T3"});
}

// ex1
f64 a_ht(vecarg a) { return 8 * a[0]/100 / std::pow(a[1], 2); }
f64 v_ht(vecarg a) { return 4 * a[0] / 100 / a[1]; }
// ex2
f64 tau_rvv(vecarg a) { return 2 * prak::PI * a[0] / (a[1] + a[2]); }
f64 D2T_mvvr(vecarg a) { return a[0] * (a[1] + a[2]) * (a[1] + a[2]) / 4 / a[3]; }
f64 Pcal(vecarg a) { return a[0] / a[1] + 1; }
// ex3
// [0] = m, [1] = r, [2] = g, [3] = a
f64 j_exp(vecarg a) { return a[0] * a[1] * a[1] * (a[2] / a[3] - 1); }
/* f64 getJ(f64 m) */ #include "J.hpp"
// ex4
f64 Wkin_mvjr(vecarg a) { return a[0] * a[1] * a[1] / 2 * (1 + a[2] / (a[0] * a[3] * a[3])); }
f64 Wpot_mgh(vecarg a) { return a[0] * a[1] * a[2]; }

table ex1(table &t) {
	table result = table({"N", "a", "sa", "v", "sv", "K", "sK"}, t.rows, NAN);
	result	.apply_function_n(
			a_ht, 
			{t.begin("h"), t.begin("t")},
			{t.begin("sh"), t.begin("st")},
			t.rows, "a", "sa"
		).apply_function_n(
			v_ht,
			{t.begin("h"), t.begin("t")},
			{t.begin("sh"), t.begin("st")},
			t.rows, "v", "sv"
		).apply_function_n(
			[](vecarg a){ return a[0] / a[1]; },
			{t.begin("h") + 1, t.begin("h")},
			{t.begin("sh") + 1, t.begin("sh")},
			t.rows - 1, "K", "sK");
	result["a", 0] *= 0.25;
	result["sa", 0] *= 0.25;
	result["v", 0] *= 0.5;
	result["sv", 0] *= 0.5;
	for (size_t i = 0; i < result.rows; ++i)
		result["N", i] = i+1;

	std::cout << "Предупреждение: Колонки K, sK НАДО сдвинуть на 1 позицию вниз в тетради!\n" << result << std::endl;
	std::cout << "среднее ускорение = " << result.col_avg("a") << std::endl;
	result.write_plot("ex1_v.plot", "N", "v", "sv");
	return result;
}

void ex2(table &t, table &e1) {
	table result({"N", "tau", "stau", "D2Tmax", "sD2Tmax", "P_cal", "P_exp", "mg"}, e1.rows - 1, NAN);
	e1.add_column("r", std::vector(e1.rows, 0.0036));
	e1.add_column("sr", std::vector(e1.rows, 0.00005));
	e1.add_column("m", std::vector(e1.rows, 0.385));
	e1.add_column("sm", std::vector(e1.rows, 0.004));	
	result.fill_column("mg", 3.78);
	result	.apply_function_n(tau_rvv, 
			{e1.begin("r"), e1.begin("v"), e1.begin("v")+1},
			{e1.begin("sr"), e1.begin("sv"), e1.begin("sv")+1},
			e1.rows - 1, "tau", "stau"
		).apply_function_n(D2T_mvvr, 
			{e1.begin("m"), e1.begin("v"), e1.begin("v")+1, e1.begin("r")},
			{e1.begin("sm"), e1.begin("sv"), e1.begin("sv")+1, e1.begin("sr")},
			e1.rows - 1, "D2Tmax", "sD2Tmax"
		).apply(
			Pcal, {"D2Tmax", "mg"}, "P_cal"
		);
	for (size_t i = 0; i < result.rows; ++i)
		result["N", i] = i+1;
	result.write_plot("ex2_t.plot", "N", "tau", "stau");
	result.write_plot("ex2_T.plot", "N", "D2Tmax", "sD2Tmax");
	std::cout << result << std::endl;
}

f64p ex3(table &ex1) {
	f64p g = {9.81, 0.01};
	f64p a = {ex1.col_avg("a"), ex1.col_stddev("a")};
	f64p r = {ex1.col_avg("r"), ex1.col_avg("sr")};
	f64p m = {0.385, 0.004};
	std::vector args = {m.val, r.val, g.val, a.val};
	std::vector sgms = {m.err, r.err, g.err, a.err};
	f64p J_exp = {j_exp(args), prak::sigma<f64>(j_exp, args, sgms)};
	f64 J_cal = getJ(m.val);
	std::cout << "Experimental J = " << J_exp << "\nCalculated J = " << J_cal << std::endl;
	return J_exp;
}

void ex4(table &t, table &e1, f64p &J) {
	table res({"N", "W_kin", "sW_kin", "W_pot", "sW_pot", "sumW", "dW", "dW(%)"}, e1.rows, NAN);
	e1.add_column("J", std::vector(e1.rows, J.val));
	e1.add_column("g", std::vector(e1.rows, 9.81));
	e1.add_column("sJ", std::vector(e1.rows, J.err));
	e1.add_column("sg", std::vector(e1.rows, 0.01));
	t.multiply_column("h", 0.01).multiply_column("sh", 0.01);

	for (size_t i = 0; i < res.rows; ++i)
		res["N", i] = i+1;


	res.apply_function_n(Wkin_mvjr, 
			{e1.begin("m"), e1.begin("v"), e1.begin("J"), e1.begin("r")},
			{e1.begin("sm"), e1.begin("sv"), e1.begin("sJ"), e1.begin("sr")},
			e1.rows, "W_kin", "sW_kin"
		).apply_function_n(Wpot_mgh, 
			{e1.begin("m"), e1.begin("g"), t.begin("h")},
			{e1.begin("sm"), e1.begin("sg"), t.begin("sh")},
			e1.rows, "W_pot", "sW_pot"
		).apply([](vecarg a){return std::abs(a[0]-a[1]);}, {"W_kin", "W_pot"}, "dW")
		.apply([](vecarg a){return std::abs(a[0]-a[1])/a[0]*100;}, {"W_kin", "W_pot"}, "dW(%)")
		.apply([](vecarg a){return (a[0]+a[1])/2;}, {"W_kin", "W_pot"}, "sumW");
	res.write_plot("ex4_WK.plot", "N", "W_kin", "sW_kin");
	res.write_plot("ex4_WP.plot", "N", "W_pot", "sW_pot");
	res.write_plot("ex4_Wfull.plot", "N", "sumW", std::nullopt);

	std::cout << res;
}

int main() {
	table t = prepare("data");
	t.print();
	table e1 = ex1(t);
	ex2(t, e1);
	f64p J = ex3(e1);
	ex4(t, e1, J);
	return 0;
}
