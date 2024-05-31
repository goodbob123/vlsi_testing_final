make

time ./src/atpg -tdfatpg ./sample_circuits/c17.ckt > ./podemx_scoap/c17.pat
time ./src/atpg -tdfatpg ./sample_circuits/c432.ckt > ./podemx_scoap/c432.pat
time ./src/atpg -tdfatpg ./sample_circuits/c499.ckt > ./podemx_scoap/c499.pat
time ./src/atpg -tdfatpg ./sample_circuits/c880.ckt > ./podemx_scoap/c880.pat
time ./src/atpg -tdfatpg ./sample_circuits/c1355.ckt > ./podemx_scoap/c1355.pat
time ./src/atpg -tdfatpg ./sample_circuits/c2670.ckt > ./podemx_scoap/c2670.pat
time ./src/atpg -tdfatpg ./sample_circuits/c3540.ckt > ./podemx_scoap/c3540.pat
time ./src/atpg -tdfatpg ./sample_circuits/c6288.ckt > ./podemx_scoap/c6288.pat
time ./src/atpg -tdfatpg ./sample_circuits/c7552.ckt > ./podemx_scoap/c7552.pat

./bin/golden_tdfsim -tdfsim ./podemx_scoap/c17.pat ./sample_circuits/c17.ckt > ./podemx_scoap/golden_c17.pat
./bin/golden_tdfsim -tdfsim ./podemx_scoap/c432.pat ./sample_circuits/c432.ckt > ./podemx_scoap/golden_c432.pat
./bin/golden_tdfsim -tdfsim ./podemx_scoap/c499.pat ./sample_circuits/c499.ckt > ./podemx_scoap/golden_c499.pat
./bin/golden_tdfsim -tdfsim ./podemx_scoap/c880.pat ./sample_circuits/c880.ckt > ./podemx_scoap/golden_c880.pat
./bin/golden_tdfsim -tdfsim ./podemx_scoap/c1355.pat ./sample_circuits/c1355.ckt > ./podemx_scoap/golden_c1355.pat
./bin/golden_tdfsim -tdfsim ./podemx_scoap/c2670.pat ./sample_circuits/c2670.ckt > ./podemx_scoap/golden_c2670.pat
./bin/golden_tdfsim -tdfsim ./podemx_scoap/c3540.pat ./sample_circuits/c3540.ckt > ./podemx_scoap/golden_c3540.pat
./bin/golden_tdfsim -tdfsim ./podemx_scoap/c6288.pat ./sample_circuits/c6288.ckt > ./podemx_scoap/golden_c6288.pat
./bin/golden_tdfsim -tdfsim ./podemx_scoap/c7552.pat ./sample_circuits/c7552.ckt > ./podemx_scoap/golden_c7552.pat