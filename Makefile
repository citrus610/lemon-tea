CXX = g++
ifeq ($(BUILD),debug)   
CXXFLAGS += -fdiagnostics-color=always -DUNICODE -std=c++20 -Og -g
else
CXXFLAGS += -DUNICODE -DNDEBUG -std=c++20 -O3 -s
endif

.PHONY: all client ppt tuner clean makedir

all: client ppt tuner

client: makedir
	@echo Building Lemon Tea client...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "client\*.cpp" -o "bin\client\client.exe"
	@echo Finished building Lemon Tea client!

ppt: makedir
	@echo Building Lemon Tea Puyo Puyo Tetris...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "ppt\*.cpp" "lib\ppt_sync\libppt_sync.dll.lib" -lsetupapi -lhid -o bin\ppt\ppt.exe
	@echo Finished building Lemon Tea Puyo Puyo Tetris!

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
	@IF NOT exist bin\ppt ( mkdir bin\ppt )
	@IF NOT exist bin\ppt\libppt_sync.dll ( copy lib\ppt_sync\libppt_sync.dll bin\ppt )
	@IF NOT exist bin\ppt\ppt-sync.exe ( copy lib\ppt_sync\ppt-sync.exe bin\ppt )
	@IF NOT exist bin\tuner ( mkdir bin\tuner )
	@IF NOT exist bin\tuner\batch ( mkdir bin\tuner\batch )

.DEFAULT_GOAL := client