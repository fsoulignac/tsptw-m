
#include <iostream>
#include <vector>
#ifndef RUNNER
    #include <fstream>
    #include <sstream>
    std::stringstream custom_cin;
#endif

#include "debug_utils.h"
#include "input.h"
#include "route.h"
#include "solver.h"
#include "time_guardian.h"
#include "local_search.h"

using namespace std;
using namespace nlohmann;
using namespace tsptwm;

// Adapted from code by Gonzalo Lera-Romero (for debugging purposes)
void simulate_runner_input(
    const string& dataset_dir, 
    const string& instance_name, 
    const string& experiment_path,
    const string& experiment_name
)
{
#ifndef RUNNER
	// Read experiments_old from file.
	ifstream experiment_file(experiment_path);
	ASSERT(PRINT, experiment_file.good(), "The experiment file {} does not exist: ", experiment_path);
	json experiment_set, experiment;
	experiment_file >> experiment_set;
	for (auto& e: experiment_set["experiments"]) if (e["name"] == experiment_name) experiment = e;
	ASSERT(PRINT, experiment != json({}), "The experiment named {} does not exist.", experiment_name);
	custom_cin << experiment;
	experiment_file.close();
	
	// Read instance file.
	json instance;
	ifstream instance_stream(dataset_dir + "/" + instance_name + ".json");
	ASSERT(PRINT, instance_stream.good(), "The instance {} does not exist.", instance_name);
	instance_stream >> instance;
	custom_cin << instance;
	instance_stream.close();
		
	// Move the stream custon_cin to cin.
	cin.rdbuf(custom_cin.rdbuf());
#endif
}

bool FeasibleRoute(const Instance& instance, const tsptwm::Route& r) {
    return instance.ArrivalTime(r, r.departure) <= r.arrival;
}


// This program is supposed to be called using the Runner Library.
// For each combination of (instance, experiment), Runner calls this main with three consecutive
// JSON strings in the STDIN.
// experiment: includes the parameters of the current execution defined in the experiment files.
// instance: includes the JSON of the instance to be solved.
// 			  for the instance currently being solved.
int main()
{
    SET_DEBUG_LEVEL(ASSERTION);
    size_t retval = 0;
    json output;

    // This line only works when it is not being called from Runner. It puts the input specified in
    // the parameters in the STDIN.
    // The first parameter indicates the dataset folder, the second the instance name, the third
    // parameter indicates the experiment file, and finally the last parameter indicates the experiment
    // to execute.
    simulate_runner_input("../instances/ascheuer_1996/", "rbg010a", "../experiments/basic-256.json", "basic");
    //simulate_runner_input("../instances/gendreau_et_al_1998/", "n20w120.001", "../experiments/all-tw-128.json", "is-5");

    // Read input.
    json experiment, instance_json;
    cin >> experiment >> instance_json;

    // Parse experiment.
    Duration time_limit = std::chrono::seconds(experiment.value("time_limit", 600));
    bool reverse_instance = experiment.value("reverse", true);
    bool ms_only = experiment.value("makespan_only", false);
        
    // Show experiment details.
    // We use clog to output execution information, and cout to output the result of the execution.
    clog << "Experiment parameters" << endl 
         << "Time limit: " << to_seconds(time_limit)
         << ", Reverse?: " << reverse_instance
         << ", Makespan only?: " << ms_only << endl;

    // Parse instance.
    clog << "Parsing instance..." << endl;
    time_guardian.SetTimeLimit(time_limit);

            
    if(reverse_instance) {
        size_t n = instance_json["vertex_count"];
        vector<TimeWindow> tws = instance_json["time_windows"];

        json reverse_json = instance_json;
        auto times = vector<vector<Time>>(instance_json["travel_times"]);
        auto rev_times = times;
        for(auto i = 1ul; i < n-1; ++i)
        for(auto j = 1ul; j < n-1; ++j) {
            rev_times[i][j] = times[j][i];
        }
        
        for(auto j = 1ul; j < n-1; ++j) {
            rev_times[0][j] = times[j][n-1];
            rev_times[j][0] = times[n-1][j];
            rev_times[n-1][j] = times[j][0];
            rev_times[j][n-1] = times[0][j];
        }
        rev_times[0][n-1] = times[n-1][0];
        rev_times[n-1][0] = times[0][n-1];
        
        reverse_json["travel_times"] = rev_times;

        auto tmp_tws = tws;
        for(auto v = 0; v < n; ++v) {
            tws[v].release = tmp_tws[0].deadline - tmp_tws[v].deadline;
            tws[v].deadline = tmp_tws[0].deadline - tmp_tws[v].release; 
        }
        reverse_json["time_windows"] = tws;
        swap(instance_json, reverse_json);
    }
                    
    DurationSolverLog log;
    time_guardian.SetTimeLimit(time_limit);
    Route ub, ms_ub, reverse_ms_ub;
    try {
        Input tsp = instance_json;
        Instance instance(&tsp);
        tie(ub, ms_ub, reverse_ms_ub, log) = SolveDuration(instance, ms_only);
    }
    catch (std::bad_alloc& e)
    {
        log.status = SolverStatus::MemoryLimitExceeded;
        clog << "Memory limit reached" << endl;
        retval = 3; // Memory limit exceeded code for runner.
    }
        
    // Output result information.
    output["solution"] = ub;
    output["ms_solution"] = ms_ub; // Add execution information to the output.
    output["reverse_ms_solution"] = reverse_ms_ub; // Add execution information to the output.
    output["solver"] = log; // Add execution information to the output.
    cout << std::setw(4) << output << endl; // Return the output for Runner.
    DEBUG_ELEM(PRINT, "{}", nlohmann::json(log).dump(4));

	return retval;
}
