CXX ?= g++

FLAGS=-g -Wall 
INCLUDE=-I ../include -I /usr/include/cppconn
LIBS=-lm -lwiringPi -lmysqlcppconn
SRC_DIR=../src

.PHONY: default
default: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(FLAGS)
release: dirs
	@$(MAKE) -j$(NPROC) all

.PHONY: all
all: alphasense bme280 mcp3424 aeroqual libconfig circularlist sqlConnector flagmanager lora

.PHONY: dirs
dirs:
	mkdir -p bin

.PHONY: alphasense
alphasense: testAlphasense.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) $(SRC_DIR)/AlphasenseSensor.cpp $(SRC_DIR)/MCP3424.cpp $(SRC_DIR)/bme280.cpp $< -o bin/$@
	
.PHONY: bme280
bme280: testBME280.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) $(SRC_DIR)/bme280.cpp $< -o bin/$@

.PHONY: mcp3424
mcp3424: testMCP3424.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) $(SRC_DIR)/MCP3424.cpp $< -o bin/$@

.PHONY: aeroqual
aeroqual: testAeroqual.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) $(SRC_DIR)/MCP3424.cpp $(SRC_DIR)/bme280.cpp $(SRC_DIR)/aeroqualSM50.cpp $< -o bin/$@

.PHONY: libconfig
libconfig: testLibconfig.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -lconfig++ $(SRC_DIR)/cfgSettings.cpp $< -o bin/$@

.PHONY: circularlist
circularlist: testCircularList.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -I $(SRC_DIR) $(SRC_DIR)/CircularList.cpp $< -o bin/$@

.PHONY: sqlConnector
sqlConnector: testSqlConnector.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) $(SRC_DIR)/sqlConnector.cpp $< -o bin/$@

.PHONY: flagmanager
flagmanager: testFlagManager.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) $(SRC_DIR)/AlphasenseSensor.cpp $(SRC_DIR)/MCP3424.cpp $(SRC_DIR)/bme280.cpp $(SRC_DIR)/flagManager.cpp $< -o bin/$@

.PHONY: lora
lora: testLoRaStick.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) -lconfig++ $(SRC_DIR)/cfgSettings.cpp $(SRC_DIR)/clientModel.cpp $(SRC_DIR)/serial.cpp $< -o bin/$@

clean:
	rm bin/*

build_arm:
	@echo "[LOG] Building test in docker container workdir: $(PWD)"
	@docker run --rm -it \
			-v $(shell dirname $(PWD)):/workdir  \
			ivan28823/emcaworkspace 
