rm *.hex
make
cp `find -name "*.hex"` ./
