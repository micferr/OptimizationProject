#include "optimizer.h"

#include <iostream>
#include <string>

int main()
{
	try {
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
			for (auto& p : problem.get_params()) {
				std::cout << p.get_name() << "\t" << (p.is_integer() ? p.get_ival() : p.get_rval()) << "\n";
			}
		}
		
		// Sample output:
		// x1	100
		// x2	100
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}
    return 0;
}
