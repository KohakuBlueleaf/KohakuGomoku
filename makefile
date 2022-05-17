CXX = g++
CXXFLAGS = --std=c++14

SOURCES_DIR = src
BUILD_DIR = build
SOURCES = $(wildcard $(SOURCES_DIR)/*.cpp)

MAIN = $(SOURCES_DIR)/main.cpp
PLAYERS = $(wildcard $(SOURCES_DIR)/player_*.cpp)
STATE_SOURCE = $(SOURCES_DIR)/state/state.cpp
POLICY_DIR = $(SOURCES_DIR)/policy

TARGET_PLAYER = $(PLAYERS:$(SOURCES_DIR)/player_%.cpp=%)
TARGET_MAIN = main
OTHER = action state gamelog.txt


.PHONY: all clean

all: |$(BUILD_DIR) $(TARGET_MAIN) $(TARGET_PLAYER)
player: |$(BUILD_DIR) $(TARGET_PLAYER)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

ifeq ($(OS), Windows_NT)
$(TARGET_PLAYER): % : $(SOURCES_DIR)/player_%.cpp
	$(CXX) -Wall -Wextra -O2 $(CXXFLAGS) -o $(BUILD_DIR)/player_$@.exe $(STATE_SOURCE) $(POLICY_DIR)/$@.cpp $< 
$(TARGET_MAIN): % : $(SOURCES_DIR)/%.cpp
	$(CXX) -Wall -Wextra -O2 $(CXXFLAGS) -o $(BUILD_DIR)/$@.exe $< 
else
$(TARGET_PLAYER): % : $(SOURCES_DIR)/player_%.cpp
	$(CXX) -Wall -Wextra -O2 $(CXXFLAGS) -o $(BUILD_DIR)/player_$@ $(STATE_SOURCE) $(POLICY_DIR)/$@.cpp $< 
$(TARGET_MAIN): % : $(SOURCES_DIR)/%.cpp
	$(CXX) -Wall -Wextra -O2 $(CXXFLAGS) -o $(BUILD_DIR)/$@ $< 
endif

clean:
ifeq ($(OS), Windows_NT)
	del /f build\main.exe $(OTHER:%=$(BUILD_DIR)\%) $(TARGET_PLAYER:%=$(BUILD_DIR)\player_%.exe)
else
	rm -f build/main $(OTHER:%=$(BUILD_DIR)/%) $(TARGET_PLAYER:%=$(BUILD_DIR)/player_%)
endif
