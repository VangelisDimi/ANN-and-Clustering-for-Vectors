#makefile

COMMON 			?= ./src/common
INCLUDE_COMMON 	?= ./include/common

INPUT_FILE 		?= ./examples/Datasets/input_small_id
QUERY_FILE 		?= ./examples/Datasets/query_small_id
OUTPUT_FILE 	?= results
CONGIF_FILE		?= ./examples/cluster_example.conf

CUBE_ARGS 		?= -i $(INPUT_FILE) -o cube_$(OUTPUT_FILE) -q $(QUERY_FILE) 
LSH_ARGS 		?= -i $(INPUT_FILE) -o lsh_$(OUTPUT_FILE) -q $(QUERY_FILE) 
CLUSTER_ARGS 	?= -i $(INPUT_FILE) -o cluster_$(OUTPUT_FILE) -c $(CONGIF_FILE)

CC				= g++
CFLAGS 			?= -g -I$(INCLUDE_COMMON)
DEBUGFLAGS 		?= -g -Wextra -Wall -I$(INCLUDE_COMMON)

all: compile_lsh compile_cube compile_cluster

#LSH
clean_lsh:
	rm -f ./bin/cube
	rm -f ./output/lsh_$(OUTPUT_FILE)

compile_lsh: clean_lsh
	$(CC) ./src/lsh/main_lsh.cpp ./src/lsh/lsh.cpp $(COMMON)/hash_functions.cpp $(COMMON)/utils.cpp $(COMMON)/exhaustive_search.cpp -o ./bin/lsh -I./include/lsh $(CFLAGS)

run_lsh:
	./bin/lsh $(LSH_ARGS)

lsh: compile_lsh run_lsh

#CUBE
clean_cube:
	rm -f ./bin/cube
	rm -f ./output/cube_$(OUTPUT_FILE)

compile_cube: clean_cube
	$(CC) ./src/cube/main_cube.cpp ./src/cube/cube.cpp $(COMMON)/hash_functions.cpp $(COMMON)/utils.cpp $(COMMON)/exhaustive_search.cpp -o ./bin/cube -I./include/cube $(CFLAGS)

run_cube: 
	./bin/cube $(CUBE_ARGS)

cube: compile_cube run_cube

#CLUSTER
clean_cluster:
	rm -f ./bin/cluster
	rm -f ./output/cluster_$(OUTPUT_FILE)

compile_cluster: clean_cluster
	$(CC) ./src/cluster/main_cluster.cpp ./src/cluster/cluster.cpp ./src/cluster/cluster_ANN.cpp ./src/lsh/lsh.cpp ./src/cube/cube.cpp $(COMMON)/hash_functions.cpp $(COMMON)/utils.cpp $(COMMON)/exhaustive_search.cpp -o ./bin/cluster $(CFLAGS) -I./include/cluster -I./include/lsh -I./include/cube

run_cluster: 
	./bin/cluster $(CLUSTER_ARGS)

cluster: compile_cluster run_cluster
