# This extension works for macOS
LIBRARY_FILE_EXTENSION = so

BIOCRO_LIB = BioCro.$(LIBRARY_FILE_EXTENSION)

# root directory for BioCro header files
BIOCRO_SOURCE_PATH = ../src

# Boost files
BOOST_PATH = ../inc

BIOCRO_INCLUDES = -I $(BIOCRO_SOURCE_PATH) -I $(BOOST_PATH)

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXE = $(OBJECTS:.o=)
RUN_TARGETS = $(patsubst %,run_%,$(EXE))

.PHONY: clean $(RUN_TARGETS)

run_all_tests: test_all
	./test_all


1: run_test_dynamical_system
2: run_test_biocro
3: run_test_harmonic_oscillator
4: run_segfault_test
5: run_test_repeat_runs
6: run_test_module_evaluation

$(RUN_TARGETS) : run_% : %
	./$<


$(BIOCRO_LIB): $(BIOCRO_SOURCE_PATH)/$(BIOCRO_LIB)
	cp $< $@

$(BIOCRO_SOURCE_PATH)/$(BIOCRO_LIB):
	echo "Build BioCro before running these tests."


test_all : $(OBJECTS)
	clang++ -std=c++14 -o $@ $(BIOCRO_LIB) $^ -lgtest_main -lgtest


$(EXE) : % : %.o
	clang++ -std=c++14 -o $@ $(BIOCRO_LIB) $< -lgtest_main -lgtest

# header file dependencies
test_biocro.o test_dynamical_system.o test_harmonic_oscillator.o test_repeat_runs.o: print_result.h
segfault_test.o test_module_evaluation.o: Random.h


segfault_test : segfault_test.o Random.o
	clang++ -std=c++14 -o $@ $(BIOCRO_LIB) $< -lgtest


$(OBJECTS) : %.o : %.cpp
	clang++ -std=c++14 $(BIOCRO_INCLUDES) $< -o $@ -c

segfault_test.o: segfault_test.cpp
	clang++ -std=c++14 $< -o $@ -c


clean:
	rm -f $(EXE) $(OBJECTS)
