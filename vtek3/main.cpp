#include "include/praktable.hpp"
#include "include/prakmath.hpp"
#include <string>
#include <unordered_map>
#include <cmath>

using RT = float;

struct wire {
	RT length, diam;
	bool operator==(const struct wire &other) const {
		return prak::fequal(length, other.length) && prak::fequal(diam, other.diam);
	}
};
template <>
struct std::hash<wire> {
	std::size_t operator()(const struct wire &W) const {
		return (std::hash<RT>{}(W.length) << 1) ^ std::hash<RT>{}(W.diam);
	}
};
std::ostream &operator<<(std::ostream &s, const struct wire &w) {
	return s << "wire {length=" << w.length << ", diam=" << w.diam << "}";
}

// args: U
// output: DU
RT D_MS8040(const std::vector<RT> &args) {
	RT val = args[0];
	if (val < 0.2) return 0.00001;
	if (val < 2) return 0.0001;
	return 0.001;
}

// args: U, DU
// output: sD
RT err_MS8040(const std::vector<RT> &args) {
	return 0.0005 * args[0] + 6 * args[1];
}

// args: I
// output: DI
RT D_M830B(const std::vector<RT> &args) {
	RT val = args[0];
	if (val < 0.0002) return 1e-6;
	if (val < 0.002) return 1e-5;
	if (val < 0.020) return 1e-4;
	if (val < 0.200) return 1e-3;
	return 1e-3;
}

// args: I, DI
// output: sI
RT err_M830B(const std::vector<RT> &args) {
	return 0.01 * args[0] + 2 * args[1];
}

void excercise1(void) {
	std::unordered_map<struct wire, prak::table<RT>> map {
		{{0.5, 0.00015}, prak::table<RT>{}},
		{{0.5, 0.00025}, prak::table<RT>{}},
		{{0.5, 0.00040}, prak::table<RT>{}},
		{{1.0, 0.00015}, prak::table<RT>{}},
		{{1.5, 0.00015}, prak::table<RT>{}},
	};
	struct prak::pvalue<RT> pho_d;
	prak::table<RT> resist_d({"d", "R", "sR", "S", "1/S"}, {});
	resist_d.column_width = 16;
	prak::table<RT> resist_L({"L", "R", "sR", "Rho", "sRho"}, {});
	std::string name_prefix = "data_";
	for (auto &[index, table] : map) {
		std::string filename = name_prefix 
		     + std::to_string((int)(10 * index.length)) 
		     + '_' 
		     + std::to_string((int)(100000 * index.diam));
		std::ifstream f(filename);
		table.read(f);
		table.column_width = 10;
		table.apply(D_MS8040, {"U"}, "DU")
		     .apply(err_MS8040, {"U", "DU"}, "sU")
		     .apply(D_M830B, {"I"}, "DI")
		     .apply(err_M830B, {"I", "DI"}, "sI");
		std::cout << index << ":\n" << table << std::endl;
		if (prak::fequal<RT>(index.length, 0.5))	resist_d.add_row({index.diam, NAN, NAN, NAN, NAN});
		if (prak::fequal<RT>(index.diam, 0.00015))	resist_L.add_row({index.length, NAN, NAN, NAN, NAN});
	}
	// LLS for L=const
	for (size_t i = 0; i < resist_d.rows; ++i) {
		RT diam = resist_d["d", i];
		prak::table<RT> &cur = map.at({.length = 0.5, .diam = diam});
		auto [a, b] = cur.least_squares_linear("U", "I", "sI");
		resist_d["R", i] = 1/a.val;
		resist_d["sR", i] = a.err/a.val/a.val;
		std::cout << "МНК для L=const, d=" << diam << ": " << "A=" << a << "; B=" << b << std::endl;
		std::ofstream outf("plot_" + std::to_string(0.5f) + "_" + std::to_string(diam) + ".data");
		cur.write_plot("U", "I", "sI", outf);
	}

	std::ofstream rsplot("plot_r_1s.data");
	resist_d
		.apply([](const std::vector<RT> &args) -> RT {
			return prak::PI * args[0]*args[0]/4;
		}, {"d"}, "S")
		.apply([](const std::vector<RT> &args) -> RT {
			return 1 / (prak::PI * args[0]*args[0]/4);
		}, {"d"}, "1/S")
		.write_plot("1/S", "R", "sR", rsplot);
	
	auto [rhod_a, rhod_b] = resist_d.least_squares_linear("1/S", "R", "sR");
	rhod_a.val /= 0.5; 
	rhod_a.err /= 0.5;
	std::cout << "МНК для Rho_d = " << rhod_a*2.0f << " (error = " << rhod_b*2.0f << ")" << std::endl;
	std::cout << "Rho = " << rhod_a << " (error = " << rhod_b << ")" << std::endl;

	// LLS for d=const 
	for (size_t i = 0; i < resist_L.rows; ++i) {
		RT length = resist_L["L", i];
		prak::table<RT> &cur = map.at({.length = length, .diam = 0.00015});
		auto [a, b] = cur.least_squares_linear("U", "I", "sI");
		resist_L["R", i] = 1/a.val;
		resist_L["sR", i] = a.err/a.val/a.val;
		std::cout << "МНК для d=const, L=" << length << ": " << "A=" << a << "; B=" << b << std::endl;
		std::ofstream outf("plot_" + std::to_string(length) + "_" + std::to_string(0.00015f) + ".data");
		cur.write_plot("U", "I", "sI", outf);
	}

	RT _d = 0.00015;
	auto rho_f = [](const std::vector<RT> &args) -> RT {
			return args[1] * prak::PI * args[2] * args[2] / args[0] / 4;
		};
	std::ofstream rlplot("plot_r_l.data");
	resist_L
		.apply([_d, &rho_f](const std::vector<RT> &args) {
			return rho_f({args[0], args[1], _d});
		}, {"L", "R"}, "Rho")
		.apply([_d, &rho_f](const std::vector<RT> &args) -> RT {
			return prak::sigma<RT>(rho_f, {args[0], args[1], _d}, {0, args[2], 0});
		}, {"L", "R", "sR"}, "sRho")
		.write_plot("L", "R", "sR", rlplot);
	auto [_1, _2] = resist_L.least_squares_linear("L", "R", "sR");	
	RT d_15_s = 0.00015 * 0.00015 * prak::PI / 4;
	std::cout << "МНК для Rho_L = " << _1 << " (error = " << _2 << ")" << std::endl;
	std::cout << "Rho_L = " << _1 * d_15_s << " (error = " << _2 * d_15_s << ")" << std::endl;
	// output
	std::cout << resist_d << resist_L;
}

void excercise3(void) {
	prak::table<RT> red, green; 
	{
		std::ifstream red_stream("data_red"), green_stream("data_green");
		red.read(red_stream);
		green.read(green_stream);
	}
	green	.apply(D_MS8040,	{"U"}, 		"DU")
		.apply(err_MS8040,	{"U", "DU"}, 	"sU")
		.apply(D_M830B,		{"I"}, 		"DI")
		.apply(err_M830B,	{"I", "DI"}, 	"sI");
	red	.apply(D_MS8040,	{"U"},		"DU")
		.apply(err_MS8040,	{"U", "DU"}, 	"sU")
		.apply(D_M830B,		{"I"}, 		"DI")
		.apply(err_M830B,	{"I", "DI"}, 	"sI");
	{
		std::ofstream red_out("plot_red.data"), green_out("plot_green.data");
		red.write_plot("U", "I", "sI", red_out);
		green.write_plot("U", "I", "sI", green_out);
	}
	std::cout << "Red: " << red << "\nGreen: " << green;
}

int main(int argc, char *argvp[]) {
	excercise1();
	excercise3();
}
