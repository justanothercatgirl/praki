#include "include/praktable.hpp"
#include <functional>

using table = prak::table<double>;

double mu(const std::vector<double> &ab) {
	return ab[0] / ab[1];
}

void proc_table(table &t) {
	using namespace std::placeholders;

	t.fill_column("sb", 0.003); // 3 millimeters
	t.apply(prak::avg<double>, {"a1", "a2", "a3"}, "a");
	t.apply([](const std::vector<double>& args) -> double {
			return std::sqrt(std::pow(prak::stddev<double>(args), 2) + 0.003*0.003);
		}, {"a1", "a2", "a3"}, "sa"); // error = sqrt(stddev^2 + systerr^2)
	t.apply(mu, {"a", "b"}, "mu");
	t.apply([](const std::vector<double> &arg) -> double {
			std::vector<double> args = {arg[0], arg[1]};
			std::vector<double> sargs = {arg[2], arg[3]};
			return prak::sigma<double>(mu, args, sargs);
		}, {"a", "b", "sa", "sb"}, "smu");
}

void ex1(void) {
	table t;
	t.read("data1");
	proc_table(t);
	std::cout << "Таблица для 1 упражнения: " << t;
}
void ex2(void) {
	table a1, a2;
	a1.read("data21");
	a2.read("data22");
	proc_table(a1);
	proc_table(a2);
	std::cout << "Таблица для 2.1: " << a1;
	std::cout << "Таблица для 2.2: " << a2;
}

int main() {
	ex1();
	ex2();
	return 0;
}
