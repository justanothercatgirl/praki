#include <iostream>

#include "include/praktable.hpp"

using table = prak::table<f64>;
using vecarg = const std::vector<f64> &;
using f64v = std::vector<f64>;
using f64p = prak::pvalue<f64>;

const f64 g = 9.81571; // moscow local acceleration
const f64 m_0 = 113.63 / 1000.0;
const f64 m_s = 0.01 / 1000.0;

f64p avg_mass(void) {
	std::ifstream f("mass");
	f64 acc = 0;
	u32 cnt = 0;
	for (f64 tmp; f >> tmp; acc += tmp, ++cnt);
	f64 avg = acc / cnt;
	f.clear();
	f.seekg(0, std::ios::beg);
	f64 stddev_acc = 0;
	for (f64 tmp; f >> tmp; stddev_acc += (tmp-avg)*(tmp-avg));
	return {.val = avg / 1000.0, .err = std::sqrt(stddev_acc / cnt / (cnt - 1)) / 1000.0};
}

// args[0] = dm
// args[1] = m
f64 acal(vecarg args) {
	return g * args[0] / args[1];
}

f64 get_g(vecarg args) {
	f64 C = args[0],
	    m_bl = args[1],
	    m = args[2],
	    m0 = args[3],
	    a = args[4];
	return C * (a*m_bl + m + m0);
}

f64 get_M(vecarg args) {
	f64 D = args[0],
	    m_bl = args[1],
	    m = args[2],
	    m0 = args[3],
	    a = args[4],
	    R = args[5];
	return D * (a*m_bl + m + m0) * R;
}

void ex1(f64p m) {
	table t("ex1");
	t	.apply(prak::avg<f64>, {"a1", "a2", "a3"}, "a")
		.apply(prak::stddev<f64>, {"a1", "a2", "a3"}, "sa")
		.apply([m](vecarg arg) -> f64 { 
			return acal({arg[0] * m.val, (m_0 + arg[0]*m.val)}); 
		}, {"N"}, "acal")
		.apply([m](vecarg arg) -> f64 {
			f64v args = {arg[0] * m.val, m_0 + arg[0]*m.val};
			f64v sigmas = {m.err * arg[0], m_s};
			return prak::sigma<f64>(acal, args, sigmas);
		}, {"N"}, "sacal");
	t.write_plot("ex1_exp.plot", "N", "a", "sa");
	t.write_plot("ex1_calc.plot", "N", "acal", "sacal");
	std::cout << "Упражнение 1: " << t << std::endl;
}

void ex2(f64p m) {
	table in("ex2");
	in	.apply([m] (vecarg a) {	return m.val * (a[0] - 1);	}, {"N"}, "m1")
		.apply([m] (vecarg a) {	return m.val * a[0];		}, {"N"}, "m2")
		.apply(prak::avg<f64>, {"a1", "a2", "a3"}, "a");
	table out({"N", "M", "1/M", "a_exp", "a_cal", "da(%)"}, in.rows, NAN); 
	{
		int N = 0;
		for (auto it = out.begin("N"); it != out.end("N"); ++it) *it = ++N;
		for (size_t i = 0; i < in.rows; ++i) out["a_exp", i] = in["a", i];
		out	.apply([m] (vecarg a) { return 	(2*a[0] - 1)*m.val + m_0;	}, {"N"}, "M")
			.apply([m] (vecarg a) { return 	acal({m.val, a[0]});		}, {"M"}, "a_cal")
			.apply([ ] (vecarg a) { return 	std::abs(a[0]-a[1])/a[0]*100;	}, {"a_cal", "a_exp"}, "da(%)")
			.apply([ ] (vecarg a) { return 	1.0/a[0];			}, {"M"}, "1/M");
	}
	std::cout << "Таблица 3: " << in << "\nТаблица 4: " << out << std::endl;
	
	out.write_plot("ex2_exp.plot", "1/M", "a_exp", std::nullopt);
	out.write_plot("ex2_cal.plot", "1/M", "a_cal", std::nullopt);
	out.write_plot("ex2_dev.plot", "1/M", "da(%)", std::nullopt);
}

void ex3(f64p m) {
	table in("ex3");
	in	.apply([m] (vecarg a) { return	(10 - a[0]) * m.val;	}, {"N"}, "m1")
		.apply([m] (vecarg a) { return	( 9 + a[0]) * m.val;	}, {"N"}, "m2")
		.apply([m] (vecarg a) { return	(2 * a[0] - 1) * m.val;	}, {"N"}, "dm")
		.apply(prak::avg<f64>, {"a1", "a2", "a3"}, "a");
	in.add_column("sa", {});
	in.apply(prak::stddev<f64>, {"a1", "a2", "a3"}, "sa");
	table out({"N", "dm", "M", "1/M", "a_exp", "a_cal", "da(%)"}, in.rows, NAN);
	{
		int N = 0;
		for (auto it = out.begin("N"); it != out.end("N"); ++it) *it = ++N;
		for (size_t i = 0; i < in.rows; ++i) out["a_exp", i] = in["a", i];
		out.fill_column("M", 19 * m.val + m_0);
		out	.apply([m] (vecarg a) {	return	(2 * a[0] - 1) * m.val;		}, {"N"}, "dm")
			.apply([ ] (vecarg a) { return 	acal({a[0], a[1]});		}, {"dm", "M"}, "a_cal")
			.apply([ ] (vecarg a) { return 	std::abs(a[0]-a[1])/a[0]*100;	}, {"a_cal", "a_exp"}, "da(%)")
			.apply([ ] (vecarg a) { return 	1.0/a[0];			}, {"M"}, "1/M");
	}
	std::cout << "Таблица 5: " << in << "\nТаблица 6: " << out << std::endl;
	out.write_plot("ex3_exp.plot", "dm", "a_exp", std::nullopt);
	out.write_plot("ex3_cal.plot", "dm", "a_cal", std::nullopt);
	out.write_plot("ex3_dev.plot", "dm", "da(%)", std::nullopt);
	auto [C, D] = out.least_squares_linear("dm", "a_exp", std::nullopt, in.col_avg("sa"));
	f64p g, M;
	f64v g_args	= {C.val, 0.0175, m.val * 19, m_0, 0.3},
	     g_sigmas	= {C.err, 0.0005, m.err * 19, m_s, 0.1};
	g.val = get_g(g_args);
	g.err = prak::sigma<f64>(get_g, g_args, g_sigmas);
	f64v M_args	= {D.val, 0.0175, m.val * 19, m_0, 0.3, 0.025},
	     M_sigmas	= {D.err, 0.0005, m.err * 19, m_s, 0.0, 0.001};
	M.val = get_M(M_args);
	M.err = prak::sigma<f64>(get_M, M_args, M_sigmas);
	std::cout << "Результаты регрессии:\n\tC = " << C << "\tD = " << D << '\n'
		  << "Полученное значение g:\n\tg = " << g << '\n'
		  << "Полученное значение M:\n\tM = " << M << std::endl;
}

int main() {
	f64p m = avg_mass();
	std::cout << "Средняя масса = " << m << std::endl;
	ex1(m);
	ex2(m);
	ex3(m);

	return 0;
}
