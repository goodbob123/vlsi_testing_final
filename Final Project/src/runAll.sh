ndet=1

rm ../output.txt
# make
./atpg -ndet $ndet  -DC -tdfsim ../podemx/c17.pat ../sample_circuits/c17.ckt >> ../output.txt 2>&1
echo "===============================" >> ../output.txt
./atpg -ndet $ndet  -DC -tdfsim ../podemx/c432.pat ../sample_circuits/c432.ckt >> ../output.txt 2>&1
echo "===============================" >> ../output.txt
./atpg -ndet $ndet  -DC -tdfsim ../podemx/c499.pat ../sample_circuits/c499.ckt >> ../output.txt 2>&1
echo "===============================" >> ../output.txt
./atpg -ndet $ndet  -DC -tdfsim ../podemx/c880.pat ../sample_circuits/c880.ckt >> ../output.txt 2>&1
echo "===============================" >> ../output.txt
./atpg -ndet $ndet  -DC -tdfsim ../podemx/c1355.pat ../sample_circuits/c1355.ckt >> ../output.txt 2>&1
echo "===============================" >> ../output.txt
./atpg -ndet $ndet  -DC -tdfsim ../podemx/c2670.pat ../sample_circuits/c2670.ckt >> ../output.txt 2>&1
echo "===============================" >> ../output.txt
./atpg -ndet $ndet  -DC -tdfsim ../podemx/c3540.pat ../sample_circuits/c3540.ckt >> ../output.txt 2>&1
echo "===============================" >> ../output.txt
./atpg -ndet $ndet  -DC -tdfsim ../podemx/c6288.pat ../sample_circuits/c6288.ckt >> ../output.txt 2>&1
echo "===============================" >> ../output.txt
./atpg -ndet $ndet  -DC -tdfsim ../podemx/c7552.pat ../sample_circuits/c7552.ckt >> ../output.txt 2>&1

sed -i '/^remove/d' ../output.txt
sed -i '/^ROFS/d' ../output.txt