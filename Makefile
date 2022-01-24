CXX = g++
ifeq ($(BUILD),debug)   
CXXFLAGS += -fdiagnostics-color=always -DUNICODE -std=c++20 -Wall -Og -g
else
CXXFLAGS += -DUNICODE -DNDEBUG -std=c++20 -Wall -O3 -flto -s -march=native
endif

.PHONY: all client tuner clean makedir

all: client tuner

client: makedir
	@echo Building Lemon Tea client...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "client\*.cpp" -o "bin\client\client.exe"
	@echo Finished building Lemon Tea client!

tuner: makedir
	@echo Building Lemon Tea tuner...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "tuner\*.cpp" -o "bin\tuner\tuner.exe"
	@echo Finished building Lemon Tea tuner!

clean: makedir
	@echo Cleaning the bin directory
	@rmdir /s /q bin
	@make makedir

makedir:
	@IF NOT exist bin ( mkdir bin )
	@IF NOT exist bin\client ( mkdir bin\client )
	@IF NOT exist bin\tuner ( mkdir bin\tuner )
	@IF NOT exist bin\tuner\batch ( mkdir bin\tuner\batch )

.DEFAULT_GOAL := client
