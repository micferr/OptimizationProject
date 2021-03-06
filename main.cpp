#include "optimizers.h"

#include <iostream>
#include <cmath>
#include <string>

int main()
{
	try {
		/// Example 1 - Simple problem to illustrate the framework's usage

		// Create the problem
		optimization_problem problem;
		
		// Add the problem's parameters (arguments are name, minimum allowed value, and maximum allowed value)
		problem.add_parameter({ "x1", 0.f, 100.f });
		problem.add_parameter({ "x2", 0.f, 100.f });
		
		// Add an objective function of the parameters
		problem.add_objective([](auto& params) {
			auto x1 = params[0].get_rval();
			auto x2 = params[1].get_rval();
			return x1 + x2;
		});
		
		// Call the chosen algorithm - for a single-ojective problem, the solution (if found)
		// will be stored in the problem's parameter vector
		bool success = particle_swarm(problem, 0.729f, 1.494f, 1.494f, 200, 100);
		
		if (success) { // Always true in this example
			// Output the solution
			problem.print_status();
		}
		
		// Sample output:
		// Parameters:
		// x1	100
		// x2	100
		// --------
		// Objective: 200

		/// N-queens

		optimization_problem queens;
		auto queens_size = 8;
		for (int i = 0; i < queens_size; i++) {
			queens.add_parameter({ "q" + std::to_string(i + 1), 1.f, float(queens_size) });
		}
		queens.add_objective([&](const auto& params) {
			auto fitness = 0.f;
			for (auto row1 = 0; row1 < params.size(); row1++) {
				// Penalty on non-integer values
				fitness -= std::min(
					params[row1].get_rval() - floorf(params[row1].get_rval()),
					params[row1].get_rval()+1 - params[row1].get_rval()
				)*10.f;

				for (auto row2 = row1+1; row2 < params.size(); row2++) {
					auto col1 = int(params[row1].get_rval());
					auto col2 = int(params[row2].get_rval());
					auto dx = col1 - col2;
					auto dy = row1 - row2;
					if (dx < 0) dx = -dx;
					if (dy < 0) dy = -dy;
					// Penalty if two queens attack each other
					if (dx == 0 || dx == dy) {
						fitness -= 1000.f;
					}
				}
			}
			return fitness;
		});

		// Omega, phi_i and phi_s taken from literature
		// (see for example Eberhart, Shi - Comparing inertia weights and constriction 
		// factors in particle swarm optimization)
		particle_swarm(queens, 0.729f, 1.494f, 1.494f, 200, 500);
		queens.print_status();

		// Sample Output
		// Parameters:
		// q1      2
		// q2      7
		// q3      5
		// q4      8
		// q5      1
		// q6      4
		// q7      6
		// q8      3
		// --------
		// Objective: 0
		// 
		// NB: Con n=8 già a volte sbaglia

		/// N-Queens, risoluzione tramite backtracking
		optimization_problem queens2;
		int queens2_size = 16;
		for (auto i = 0; i < queens2_size; i++) {
			queens2.add_parameter({ "Q" + std::to_string(i + 1), 1, queens2_size });
		}
		std::function<bool(optimization_problem&, unsigned)> queens_bt = [&](auto& problem, int index) {
			auto& params = problem.get_params();
			
			std::vector<int> allowed;
			for (int i = 1; i <= queens2_size; i++) {
				auto value_ok = true;
				for (int j = 0; j < index; j++) {
					auto row1 = i;
					auto row2 = params[j].get_ival();
					auto col1 = j + 1;
					auto col2 = index + 1;
					auto dx = col2 - col1;
					auto dy = row2 - row1;
					if (dx < 0) dx = -dx;
					if (dy < 0) dy = -dy;
					if (row1 == row2 || dx == dy) {
						value_ok = false;
						break;
					}
				}
				if (value_ok) allowed.push_back((i));
			}

			for (auto& i : allowed) {
				params[index].set_ival(i);
				if (index == queens2_size - 1) return true;
				else if (queens_bt(problem, index + 1)) return true;
			}
			return false;
		};
		queens_bt(queens2, 0);
		queens2.print_status();

		/// Ackley's function
		// f(x,y) = -20exp(-0.2sqrt(0.5(x^2+y^2)))-exp(0.5(cos(2pi*x)+cos(2pi*y)))+e+20
		optimization_problem ackley;
		ackley.add_parameter({ "x", -5.f, 5.f });
		ackley.add_parameter({ "y", -5.f, 5.f });
		ackley.add_objective([&](const auto& params) {
			auto x = params[0].get_rval();
			auto y = params[1].get_rval();
			auto pi = 3.14159265359;
			auto e1 = exp(-0.2*sqrt(x*x + y*y));
			auto e2 = exp(0.5f*(cos(2*pi*x) + cos(2*pi*y)));
			auto v = -20 * e1 - e2 + exp(1) + 20;
			return -v;
		});
		particle_swarm(ackley, 0.729f, 1.494f, 1.494f, 200, 500);
		ackley.print_status();

		// Sample Output:
		// Parameters:
		// x -3.09178e-17
		// y -2.16057e-16
		// --------
		// Objective : -0

		/// Simionescu

		optimization_problem simionescu;
		simionescu.add_parameter({ "x", -1.25f, 1.25f });
		simionescu.add_parameter({ "y", -1.25f, 1.25f });
		simionescu.add_objective([&](const auto& params) {
			auto x = params[0].get_rval();
			auto y = params[1].get_rval();
			auto fitness = -0.1*x*y;
			auto bound = pow(1.0 + 0.2*cos(8.0 * atan2(x, y)), 2.0);
			auto excess = x*x + y*y - bound;
			auto penalty = -std::max(excess, 0.0)*10.f;
			return fitness + penalty;
		});
		particle_swarm(simionescu, 0.729f, 1.494f, 1.494f, 400, 2000);
		simionescu.print_status();

		// Sample Output:
		// Parameters:
		// x	0.848537
		// y	-0.84852
		// --------
		// Objective : 0.072

		/// Kursawe function
		optimization_problem kursawe;
		kursawe.add_parameter({ "x1", -5.f, 5.f });
		kursawe.add_parameter({ "x2", -5.f, 5.f });
		kursawe.add_parameter({ "x3", -5.f, 5.f });
		kursawe.add_objective([&](const auto& params) {
			float p[3] = { params[0].get_rval(), params[1].get_rval(), params[2].get_rval() };
			auto f = 0.f;
			for (auto i = 0; i < 2; i++) {
				f += -10 * exp(-0.2*sqrt(p[i]*p[i] + p[i+1]*p[i+1]));
			}
			return -f;
		});
		kursawe.add_objective([&](const auto& params) {
			float p[3] = { params[0].get_rval(), params[1].get_rval(), params[2].get_rval() };
			auto f = 0.f;
			for (auto i = 0; i <= 2; i++) {
				f += (pow(abs(p[i]), 0.8) + 5 * sin(pow(p[i], 3.0)));
			}
			return -f;
		});
		auto kursawe_sols = spea2(kursawe, 100,25,200);
		std::cout << "Kursawe: \n";
		for (auto& sol : kursawe_sols) {
			for (auto& p : sol) {
				std::cout << p.get_name() << "\t" << (p.is_integer() ? p.get_ival() : p.get_rval()) << "\n";
			}
			auto objs = kursawe.get_objectives_values(sol);
			for (auto i = 0; i < objs.size(); i++) {
				std::cout << "Objective " << (i+1) << ": " << objs[i] << "\n";
			}
			std::cout << "----------\n";
		}
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}
    return 0;
}
