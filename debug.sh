mkdir build
cd build
cmake .. -DDISABLE_WARNINGS=true -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"
make
if [ $? -eq 0 ]; then
    echo "Executing faccha ..."
	./bin/faccha
else
    echo "Compilation failed ..."
fi