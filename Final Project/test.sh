make
ndet=8

mkdir result

time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c17.ckt > ./result/c17.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c432.ckt > ./result/c432.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c499.ckt > ./result/c499.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c880.ckt > ./result/c880.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c1355.ckt > ./result/c1355.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c2670.ckt > ./result/c2670.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c3540.ckt > ./result/c3540.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c6288.ckt > ./result/c6288.pat
time ./src/atpg -tdfatpg -compression -ndet $ndet ./sample_circuits/c7552.ckt > ./result/c7552.pat

./bin/golden_tdfsim -ndet $ndet -tdfsim  ./result/c17.pat ./sample_circuits/c17.ckt > ./result/golden_c17.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./result/c432.pat ./sample_circuits/c432.ckt > ./result/golden_c432.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./result/c499.pat ./sample_circuits/c499.ckt > ./result/golden_c499.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./result/c880.pat ./sample_circuits/c880.ckt > ./result/golden_c880.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./result/c1355.pat ./sample_circuits/c1355.ckt > ./result/golden_c1355.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./result/c2670.pat ./sample_circuits/c2670.ckt > ./result/golden_c2670.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./result/c3540.pat ./sample_circuits/c3540.ckt > ./result/golden_c3540.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./result/c6288.pat ./sample_circuits/c6288.ckt > ./result/golden_c6288.pat
./bin/golden_tdfsim -ndet $ndet -tdfsim  ./result/c7552.pat ./sample_circuits/c7552.ckt > ./result/golden_c7552.pat