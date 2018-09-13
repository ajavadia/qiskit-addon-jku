/*
DD-based simulator by JKU Linz, Austria

Developer: Alwin Zulehner, Robert Wille

With code from the QMDD implementation provided by Michael Miller (University of Victoria, Canada)
and Philipp Niemann (University of Bremen, Germany).

For more information, please visit http://iic.jku.at/eda/research/quantum_simulation

If you have any questions feel free to contact us using
alwin.zulehner@jku.at or robert.wille@jku.at

If you use the quantum simulator for your research, we would be thankful if you referred to it
by citing the following publication:

@article{zulehner2018simulation,
    title={Advanced Simulation of Quantum Computations},
    author={Zulehner, Alwin and Wille, Robert},
    journal={IEEE Transactions on Computer Aided Design of Integrated Circuits and Systems (TCAD)},
    year={2018},
    eprint = {arXiv:1707.00865}
}
*/

#include <iostream>
#include <stdlib.h>
#include <set>
#include <unordered_map>
#include <boost/program_options.hpp>
#include <vector>
#include <gmp.h>
#include <mpreal.h>
#include <QASMsimulator.h>
#include <queue>
#include <chrono>
#include <QMDDcore.h>
#include <QMDDpackage.h>
#include <QMDDcomplex.h>

#include <Simulator.h>
//#include <QASMscanner.hpp>

#define DEBUG_LOG std::cout

using mpfr::mpreal;

using namespace std;


int main(int argc, char** argv) {
        PRINT_LOCATION
	namespace po = boost::program_options;
	po::options_description description("Allowed options");
	description.add_options()
	    ("help", "produce help message")
		("seed", po::value<unsigned long>(), "seed for random number generator")
	    ("simulate_qasm", po::value<string>()->implicit_value(""), "simulate a quantum circuit given in QPENQASM 2.0 format (if no file is given, the circuit is read from stdin)")
		("shots", po::value<unsigned int>(), "number of shots")
		("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
		("display_statevector", "adds the state-vector to snapshots")
		("display_probabilities", "adds the probabilities of the basis states to snapshots")
		("precision", po::value<double>(), "two numbers are treated to be equal if their difference is smaller than this value")
	;

    std::cout << "ARGC = " << argc << std::endl;
    for (unsigned n = 0; n < argc; ++n){
        std::cout << "ARGV[" << n << "] = " << argv[n] << std::endl;
    }
	po::variables_map vm;
        PRINT_LOCATION
	po::store(po::parse_command_line(argc, argv, description), vm);
        PRINT_LOCATION
	po::notify(vm);
        PRINT_LOCATION
	if (vm.count("help")) {
	    cout << description << "\n";
	    return 1;
	}

	unsigned long seed = time(NULL);
	if (vm.count("seed")) {
		seed = vm["seed"].as<unsigned long>();
	}

	srand(seed);
        PRINT_LOCATION
	QMDDinit(0);

	if (vm.count("precision")) {
#if VERBOSE
		std::cout << "Set precision to " << vm["precision"].as<double>() << std::endl;
#endif
		Ctol = mpreal(vm["precision"].as<double>());
	}
        PRINT_LOCATION
	Simulator* simulator;

	if (vm.count("simulate_qasm")) {
		string fname = vm["simulate_qasm"].as<string>();
		if(fname == "") {
                        std::cerr << "No QASM file given, reading from the standard input" << std::endl;
			simulator = new QASMsimulator(vm.count("display_statevector"), vm.count("display_probabilities"));
		} else {
			simulator = new QASMsimulator(fname, vm.count("display_statevector"), vm.count("display_probabilities"));
		}
	} else {
		cout << description << "\n";
	    return 1;
	}
	PRINT_LOCATION

    auto t1 = chrono::high_resolution_clock::now();

	if(vm.count("shots")) {
		simulator->Simulate(vm["shots"].as<unsigned int>());
	} else {
		simulator->Simulate(1);
	}
	PRINT_LOCATION

	auto t2 = chrono::high_resolution_clock::now();
	chrono::duration<float> diff = t2-t1;

	delete simulator;
        PRINT_LOCATION
	if (vm.count("ps")) {
		cout << endl << "SIMULATION STATS: " << endl;
		cout << "  Number of applied gates: " << simulator->GetGatecount() << endl;
		cout << "  Simulation time: " << diff.count() << " seconds" << endl;
		cout << "  Maximal size of DD (number of nodes) during simulation: " << simulator->GetMaxActive() << endl;
	}
        PRINT_LOCATION
	return 0;
}
