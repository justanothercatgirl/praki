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

RT S(const std::vector<RT> &args) {
	return prak::PI * args[0]*args[0]/4;
}

RT S_1(const std::vector<RT> &args) {
	return 1 / S(args);
}

// args: I
// output: DI
RT D_M830B(const std::vector<RT> &args) {
	RT val = args[0];
	if (val < 0.002) return 1e-6;
	if (val < 0.020) return 1e-5;
	if (val < 0.200) return 1e-4;
	return 1e-3;
}

// args: I, DI
// output: sI
RT err_M380B(const std::vector<RT> &args) {
	return 0.01 * args[0] + 2 * args[1];
}

void ex1(void) {
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
		     .apply(D_M830B, {"I"}, "DI")
		     .apply(err_M380B, {"I", "DI"}, "sI");
		std::cout << index << ":\n" << table << std::endl;
		if (prak::fequal<RT>(index.length, 0.5))	resist_d.add_row({index.diam, NAN, NAN, NAN, NAN});
		if (prak::fequal<RT>(index.diam, 0.00015))	resist_L.add_row({index.length, NAN, NAN, NAN, NAN});
	}
	// LLS for L=const
	for (size_t i = 0; i < resist_d.rows; ++i) {
		prak::table<RT> &cur = map.at({.length = 0.5, .diam = resist_d["d", i]});
		auto [a, b] = cur.least_squares_linear("U", "I", "sI");
		std::cout << "diam = " << resist_d["d", i] 
			  << ": A = " << a
			  << "; B = " << b
			  << "; R = " << 1/a.val << std::endl;
		resist_d["R", i] = 1/a.val;
		resist_d["sR", i] = a.err/a.val/a.val;
		resist_d.apply(S_1, {"d"}, "1/S");
		std::ofstream outf("plot_" + std::to_string(0.5f) + "_" + std::to_string(resist_d["d", i]) + ".data");
		cur.write_plot("U", "I", "sI", outf);
	}
	auto [rho, _] = resist_d.least_squares_linear("1/S", "R", "sR");
	rho.val /= 0.5; 
	rho.err /= 0.5;
	std::cout << "Rho = " << rho << " (error = " << _ << ")" << std::endl;
	
	// LLS for d=const 
	resist_d.apply(S, {"d"}, "S")
		.apply(S_1, {"d"}, "1/S");
	std::cout << resist_d;
	
}

void ex3(void) {

}

int main(int argc, char *argvp[]) {
	ex1();
	ex3();
}
