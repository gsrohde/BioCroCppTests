# This extension works for macOS
LIBRARY_FILE_EXTENSION = so

BIOCRO_LIB = BioCro.$(LIBRARY_FILE_EXTENSION)

# root directory for BioCro header files
BIOCRO_SOURCE_PATH = ../src

# Boost files
BOOST_PATH = ../inc

BIOCRO_INCLUDES = -I $(BIOCRO_SOURCE_PATH) -I $(BOOST_PATH)

default: 1 2 3 4 5

1: run_test_dynamical_system
2: run_test_biocro
3: run_test_harmonic_oscillator
4: run_segfault_test
5: run_test_repeat_runs
6: run_test_module_evaluation

run_test_dynamical_system: test_dynamical_system
	./test_dynamical_system ‑‑gtest_repeat=1

run_test_biocro: test_biocro
	./test_biocro --gtest_also_run_disabled_tests ‑‑gtest_repeat=1

run_test_harmonic_oscillator: test_harmonic_oscillator
	./test_harmonic_oscillator --gtest_also_run_disabled_tests ‑‑gtest_repeat=1

run_segfault_test: segfault_test
	./segfault_test

run_test_repeat_runs: test_repeat_runs
	./test_repeat_runs
	#/Applications/Xcode.app/Contents/Developer/usr/bin/lldb test_repeat_runs

run_test_module_evaluation: test_module_evaluation
	./test_module_evaluation


test_dynamical_system test_biocro test_harmonic_oscillator test_repeat_runs test_module_evaluation: $(BIOCRO_LIB)

$(BIOCRO_LIB): $(BIOCRO_SOURCE_PATH)/$(BIOCRO_LIB)
	cp $< $@

$(BIOCRO_SOURCE_PATH)/$(BIOCRO_LIB):
	echo "Build BioCro before running these tests."

test_dynamical_system: test_dynamical_system.cpp
	clang++ -std=c++14 $(BIOCRO_INCLUDES) $(BIOCRO_LIB)  test_dynamical_system.cpp -o test_dynamical_system -lgtest_main -lgtest

test_biocro: test_biocro.cpp
	clang++ -std=c++14 $(BIOCRO_INCLUDES) $(BIOCRO_LIB) test_biocro.cpp -o test_biocro -lgtest -lgtest_main

test_harmonic_oscillator: test_harmonic_oscillator.cpp
	clang++ -std=c++14 $(BIOCRO_INCLUDES) $(BIOCRO_LIB) test_harmonic_oscillator.cpp -o test_harmonic_oscillator -lgtest -lgtest_main

segfault_test: segfault_test.cpp
	clang++ -std=c++14 segfault_test.cpp -o segfault_test -lgtest

test_repeat_runs: test_repeat_runs.cpp
	clang++ -std=c++14 $(BIOCRO_INCLUDES) $(BIOCRO_LIB) test_repeat_runs.cpp -o test_repeat_runs -lgtest_main -lgtest

test_module_evaluation: test_module_evaluation.cpp
	clang++ -std=c++14 $(BIOCRO_INCLUDES) $(BIOCRO_LIB) test_module_evaluation.cpp -o test_module_evaluation -lgtest -lgtest_main
