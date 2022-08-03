cc = g++
prom = dpEnsemble
source = src/ensemble.cc src/Fasta.cc

$(prom): $(source)
	$(cc) -std=c++20 -O2 -o $(prom) $(source)