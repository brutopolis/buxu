#bruter --debug test.br -o test # Compile the test.br file into a executable
#bruter -c test.br -o test.br.o # Compile the test.br as a static library
#bruter -c test.br -o test.br.so # Compile the test.br as a shared library
bruter txt.br # just interpret the txt.br file
bruter
bruter scopes.br --debug # interpret the scopes.br file in debug mode
bruter
bruterra --debug luapi.br # the order of the arguments doesn't matter when using flags, except for the -o flag
bruterra
bruterra -o test compiling.br --debug # Compile the compiling.br file into a executable called test
bruterra
bruter --debug conditions.br
bruter
bruter types.br
bruter
bruter --debug fakemodules.br
bruter
bruter --debug loop.br
bruter
bruter --debug functions.br
bruter
bruter --debug img.br
bruter
bruter --debug promises.br
./test # Run the test executable