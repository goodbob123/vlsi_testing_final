cd src
make
cd ..

#rm -rf  podem/* diff* podemx/*
#2670 7552
time ./src/atpg  ./sample_circuits/c17.ckt > ./podemx/c17.pat
time ./src/atpg  ./sample_circuits/c432.ckt > ./podemx/c432.pat
time ./src/atpg  ./sample_circuits/c499.ckt > ./podemx/c499.pat
time ./src/atpg  ./sample_circuits/c880.ckt > ./podemx/c880.pat
time ./src/atpg  ./sample_circuits/c1355.ckt > ./podemx/c1355.pat
time ./src/atpg  ./sample_circuits/c2670.ckt > ./podemx/c2670.pat
time ./src/atpg  ./sample_circuits/c3540.ckt > ./podemx/c3540.pat
time ./src/atpg  ./sample_circuits/c6288.ckt > ./podemx/c6288.pat
time ./src/atpg  ./sample_circuits/c7552.ckt > ./podemx/c7552.pat

./bin/golden_tdfsim -tdfsim ./podemx/c17.pat ./sample_circuits/c17.ckt > ./podemx/golden_c17.pat
./bin/golden_tdfsim -tdfsim ./podemx/c432.pat ./sample_circuits/c432.ckt > ./podemx/golden_c432.pat
./bin/golden_tdfsim -tdfsim ./podemx/c499.pat ./sample_circuits/c499.ckt > ./podemx/golden_c499.pat
./bin/golden_tdfsim -tdfsim ./podemx/c880.pat ./sample_circuits/c880.ckt > ./podemx/golden_c880.pat
./bin/golden_tdfsim -tdfsim ./podemx/c1355.pat ./sample_circuits/c1355.ckt > ./podemx/golden_c1355.pat
./bin/golden_tdfsim -tdfsim ./podemx/c2670.pat ./sample_circuits/c2670.ckt > ./podemx/golden_c2670.pat
./bin/golden_tdfsim -tdfsim ./podemx/c3540.pat ./sample_circuits/c3540.ckt > ./podemx/golden_c3540.pat
./bin/golden_tdfsim -tdfsim ./podemx/c6288.pat ./sample_circuits/c6288.ckt > ./podemx/golden_c6288.pat
./bin/golden_tdfsim -tdfsim ./podemx/c7552.pat ./sample_circuits/c7552.ckt > ./podemx/golden_c7552.pat


# ./src/atpg  ./sample_circuits/c17.ckt > ./podem/c17.pat
# ./src/atpg  ./sample_circuits/c432.ckt > ./podem/c432.pat
# ./src/atpg  ./sample_circuits/c499.ckt > ./podem/c499.pat
# ./src/atpg  ./sample_circuits/c880.ckt > ./podem/c880.pat
# ./src/atpg  ./sample_circuits/c1355.ckt > ./podem/c1355.pat
# ./src/atpg  ./sample_circuits/c2670.ckt > ./podem/c2670.pat
# ./src/atpg  ./sample_circuits/c3540.ckt > ./podem/c3540.pat
# ./src/atpg  ./sample_circuits/c6288.ckt > ./podem/c6288.pat
# ./src/atpg  ./sample_circuits/c7552.ckt > ./podem/c7552.pat

# ./bin/golden_tdfsim -tdfsim ./podem/c17.pat ./sample_circuits/c17.ckt > ./podem/golden_c17.pat
# ./bin/golden_tdfsim -tdfsim ./podem/c432.pat ./sample_circuits/c432.ckt > ./podem/golden_c432.pat
# ./bin/golden_tdfsim -tdfsim ./podem/c499.pat ./sample_circuits/c499.ckt > ./podem/golden_c499.pat
# ./bin/golden_tdfsim -tdfsim ./podem/c880.pat ./sample_circuits/c880.ckt > ./podem/golden_c880.pat
# ./bin/golden_tdfsim -tdfsim ./podem/c1355.pat ./sample_circuits/c1355.ckt > ./podem/golden_c1355.pat
# ./bin/golden_tdfsim -tdfsim ./podem/c2670.pat ./sample_circuits/c2670.ckt > ./podem/golden_c2670.pat
# ./bin/golden_tdfsim -tdfsim ./podem/c3540.pat ./sample_circuits/c3540.ckt > ./podem/golden_c3540.pat
# ./bin/golden_tdfsim -tdfsim ./podem/c6288.pat ./sample_circuits/c6288.ckt > ./podem/golden_c6288.pat
# ./bin/golden_tdfsim -tdfsim ./podem/c7552.pat ./sample_circuits/c7552.ckt > ./podem/golden_c7552.pat



# diff -y --suppress-common-lines ./result/c17.pat ./pattern/golden_c17.pat > diff_1
# diff -y --suppress-common-lines ./result/c432.pat ./pattern/golden_c432.pat > diff_2
# diff -y --suppress-common-lines ./result/c499.pat ./pattern/golden_c499.pat > diff_3
# diff -y --suppress-common-lines ./result/c880.pat ./pattern/golden_c880.pat > diff_4
# diff -y --suppress-common-lines ./result/c1355.pat ./pattern/golden_c1355.pat > diff_5
# diff -y --suppress-common-lines ./result/c2670.pat ./pattern/golden_c2670.pat > diff_6
# diff -y --suppress-common-lines ./result/c3540.pat ./pattern/golden_c3540.pat > diff_7
# diff -y --suppress-common-lines ./result/c6288.pat ./pattern/golden_c6288.pat > diff_8
# diff -y --suppress-common-lines ./result/c7552.pat ./pattern/golden_c7552.pat > diff_9

# sed -i '/#atpg: cputime for/d' diff_1
# sed -i '/#atpg: cputime for/d' diff_2
# sed -i '/#atpg: cputime for/d' diff_3
# sed -i '/#atpg: cputime for/d' diff_4
# sed -i '/#atpg: cputime for/d' diff_5
# sed -i '/#atpg: cputime for/d' diff_6
# sed -i '/#atpg: cputime for/d' diff_7
# sed -i '/#atpg: cputime for/d' diff_8
# sed -i '/#atpg: cputime for/d' diff_9

# for i in {1..9}
# do
#     FILE="diff_$i"
#     if [ -s "$FILE" ]; then
#         echo "${FILE} error"
#     fi
# done