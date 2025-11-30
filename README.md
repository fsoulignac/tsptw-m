# Beware of the Classical Benchmark Instances for the Traveling Salesman Problem with Time Windows (preprint version)

Source code used for the experiments in the preprint version of the paper.  It includes simple solvers for the TSPTW-M and the TSTPTW-D.

## Abstract

We propose a simple and exact informed search method for the Traveling Salesman Problem with Time Windows and Makespan objective (TSPTW-M) that solves all instances of the classical benchmark having 50 or more customers in less than ten seconds each.  Applying this algorithm as an off-the-shelf method, we also solve all but one of these instances for the Duration objective.  Our main conclusion is that these instances should no longer be employed for evaluating the TSTPW-M and its Duration variant: they can be "hacked" to yield results that seem outstanding at first sight.  Additionaly, caution is advised when designing hard training sets for machine learning algorithms.

## Results of our experiments

Folder ```final_results``` contains the raw results of our experiments (file ```results.json```), including the optimum routes, and the summary we use in the paper (file ```results.ods```).  You can visualize```results.json``` using kaleidoscope.

## Getting started to reproducing our results

The following instructions will guide you through the steps to execute the experiments.

### Prerequisites

- Python >= 3.6 [(more info)](https://www.python.org/)
- CMake >= 3.22.1 [(more info)](https://cmake.org/)
    - On Linux: ```sudo apt-get install cmake```
- C++20 or higher [(more info)](https://es.wikipedia.org/wiki/C%2B%2B20)
- G++ fully implementing C++20.

### Built with modified versions of

- Kaleidoscope: A tool to visualize the outputs of Optimization Problems [(more info)](https://github.com/gleraromero/kaleidoscope)
    - Parser for this project are included
- Runner: A script to ease the process of running experiments [(more info)](https://github.com/gleraromero/runner)

### Getting the benchmark instances

We do not provide the benchmark instances.  To replicate our results you must obtain them elsewhere, following these instructions:

1. Download the repository of the paper "An exact and anytime solver for the Time Dependent Traveling Salesman Problem with Time Windows" by Fontaine et al [here](https://github.com/romainfontaine/tdtsptw-ejor23).
1. Extract the zip files in the benchmark folder
1. Transform the benchmarks Asc, DaS, Dum, Gen, Lan, Ohl, Pes, Pot, and Rif to the required format.
1. Donwload the repository of the paper "Dynamic programming for the time-dependent traveling salesman problem with time windows" by Lera-Romero et al [here](https://github.com/gleraromero/tdtsptw).
1. Transform the benchmark D-Ohl into the required format.
1. Generate the index files describing all the benchmark sets

Alternatively, you can run the python script ```instances/get_benchmark.py``` at your own risk.

### Running the experiments.

1. Go to the root directory.
1. Execute ```python3 runner/runner.py <experiment_file> -M <memory limit>```
1. The execution output will be continually saved to the output folder.

> Experiment files are located in the _experiments_ folder. For more information see Section [Experiments](#Experiments)

### Experiments

There are five experiment files:
1. ```experiments/basic-512.json```: to run the experiments for the DaS benchmark
1. ```experiments/basic-256.json```: to run the experiments for the Asc (except rbg021.x, x = 6,7,8,9), Dum, Ohl, and D-Ohl benchmarks
1. ```experiments/basic-256-ms.json```: to run the experiments for the TSPTW-M only using the rbg021.x, x = 6,7,8,9, in Asc
1. ```experiments/basic-128.json```: to run the experiments for the Gen benchmark
1. ```experiments/basic-64-ms.json```: to run the experiments for the TSPTW-M only using the Lan, Pes, Pot, and Rif benchmarks

### Visualizing the experiment results

The results of the experiments are saved in the output folder and are in a human readable JSON format.  We also provide a JSON file with the results from our experiments in folder ```final_results```.

To visualize them in a table you can use the Kaleidoscope tool by Lera-Romero [(more info)](https://github.com/gleraromero/kaleidoscope), following these steps:
1. Donwload kaleidoscope
1. Copy the parser files located in the folder kaleidoscope respecting the directory structure
1. Go to the folder kaleidoscope and open the file index.html with a Web Browser (Chrome prefered, Firefox working).
1. Add the output file.
1. Select the experiments.
1. Add some attributes to visualize.
1. Click on Refresh.
1. If more details on an experiment are desired click on the + icon in a specific row.

### Instances

To run the experiment files as provided the instances must be located inside  the ```instances/``` folder respecting the following structure:

* folder ```instances/ascheuer_1996``` for Asc
* folder ```instances/d_ohlmann_thomas_2007``` for D-Ohl
* folder ```instances/dumas_et_al_1995``` for Dum
* folder ```instances/gendreau_et_al_1998``` for Gen
* folder ```instances/langevin_et_al_1993``` for Lan
* folder ```instances/ohlmann_thomas_2007``` for Ohl
* folder ```instances/pesant_et_al_1998``` for Pes
* folder ```instances/potvin_bengio_1996``` for Pot
* folder ```instances/rifki_et_al_2020``` for Rif

## Built With

* [GCC's Policy-Based Data Structures](https://gcc.gnu.org/onlinedocs/libstdc++/manual/policy_data_structures.html)
* [JSON for Modern C++](https://github.com/nlohmann/json)

## Author

Francisco J. Soulignac

## License

This project is licensed under the MIT License - see the LICENSE file for details
