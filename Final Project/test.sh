make

time ./src/atpg  ./sample_circuits/c17.ckt > ./podem/c17.pat
time ./src/atpg  ./sample_circuits/c432.ckt > ./podem/c432.pat
time ./src/atpg  ./sample_circuits/c499.ckt > ./podem/c499.pat
time ./src/atpg  ./sample_circuits/c880.ckt > ./podem/c880.pat
time ./src/atpg  ./sample_circuits/c1355.ckt > ./podem/c1355.pat
time ./src/atpg  ./sample_circuits/c2670.ckt > ./podem/c2670.pat
time ./src/atpg  ./sample_circuits/c3540.ckt > ./podem/c3540.pat
time ./src/atpg  ./sample_circuits/c6288.ckt > ./podem/c6288.pat
time ./src/atpg  ./sample_circuits/c7552.ckt > ./podem/c7552.pat

./bin/golden_tdfsim -tdfsim ./podem/c17.pat ./sample_circuits/c17.ckt > ./podem/golden_c17.pat
./bin/golden_tdfsim -tdfsim ./podem/c432.pat ./sample_circuits/c432.ckt > ./podem/golden_c432.pat
./bin/golden_tdfsim -tdfsim ./podem/c499.pat ./sample_circuits/c499.ckt > ./podem/golden_c499.pat
./bin/golden_tdfsim -tdfsim ./podem/c880.pat ./sample_circuits/c880.ckt > ./podem/golden_c880.pat
./bin/golden_tdfsim -tdfsim ./podem/c1355.pat ./sample_circuits/c1355.ckt > ./podem/golden_c1355.pat
./bin/golden_tdfsim -tdfsim ./podem/c2670.pat ./sample_circuits/c2670.ckt > ./podem/golden_c2670.pat
./bin/golden_tdfsim -tdfsim ./podem/c3540.pat ./sample_circuits/c3540.ckt > ./podem/golden_c3540.pat
./bin/golden_tdfsim -tdfsim ./podem/c6288.pat ./sample_circuits/c6288.ckt > ./podem/golden_c6288.pat
./bin/golden_tdfsim -tdfsim ./podem/c7552.pat ./sample_circuits/c7552.ckt > ./podem/golden_c7552.pat