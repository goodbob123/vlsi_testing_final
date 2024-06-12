make
ndet=8

time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c17.ckt > ./podemx/c17.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c432.ckt > ./podemx/c432.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c499.ckt > ./podemx/c499.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c880.ckt > ./podemx/c880.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c1355.ckt > ./podemx/c1355.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c2670.ckt > ./podemx/c2670.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c3540.ckt > ./podemx/c3540.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c6288.ckt > ./podemx/c6288.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c7552.ckt > ./podemx/c7552.pat

./bin/golden_tdfsim -ndet $ndet -tdfsim  ./podemx/c17.pat ./sample_circuits/c17.ckt > ./podemx/golden_c17.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./podemx/c432.pat ./sample_circuits/c432.ckt > ./podemx/golden_c432.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./podemx/c499.pat ./sample_circuits/c499.ckt > ./podemx/golden_c499.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./podemx/c880.pat ./sample_circuits/c880.ckt > ./podemx/golden_c880.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./podemx/c1355.pat ./sample_circuits/c1355.ckt > ./podemx/golden_c1355.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./podemx/c2670.pat ./sample_circuits/c2670.ckt > ./podemx/golden_c2670.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./podemx/c3540.pat ./sample_circuits/c3540.ckt > ./podemx/golden_c3540.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./podemx/c6288.pat ./sample_circuits/c6288.ckt > ./podemx/golden_c6288.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./podemx/c7552.pat ./sample_circuits/c7552.ckt > ./podemx/golden_c7552.pat