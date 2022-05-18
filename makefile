CXX = g++
CXX_MERGE = cpp-merge
CXXFLAGS = --std=c++14

SOURCES_DIR = src
BUILD_DIR = build
MERGE_DIR = $(BUILD_DIR)/merged
SOURCES = $(wildcard $(SOURCES_DIR)/*.cpp)

MAIN = $(SOURCES_DIR)/main.cpp
PLAYERS = $(wildcard $(SOURCES_DIR)/player_*.cpp)
STATE_SOURCE = $(SOURCES_DIR)/state/state.cpp
POLICY_DIR = $(SOURCES_DIR)/policy

TARGET_PLAYER = $(PLAYERS:$(SOURCES_DIR)/player_%.cpp=%)
TARGET_MAIN = main
TARGET_MERGE_PLAYER = $(TARGET_PLAYER:%=merge_%)
TARGET_MERGE_MAIN = merge_main
OTHER = action state gamelog.txt


.PHONY: all clean
all: |$(BUILD_DIR) $(TARGET_MAIN) $(TARGET_PLAYER)
player: |$(BUILD_DIR) $(TARGET_PLAYER)
merge: |$(MERGE_DIR) $(TARGET_MERGE_PLAYER) $(TARGET_MERGE_MAIN)

# make build dir if it doesn't exist
$(BUILD_DIR):
	mkdir "$(BUILD_DIR)"
$(MERGE_DIR):
	mkdir "$(MERGE_DIR)"

# build target
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

# build merge file
$(TARGET_MERGE_PLAYER): merge_% : $(SOURCES_DIR)/player_%.cpp
	$(CXX_MERGE) $< -o $(MERGE_DIR)/player_$*_merged.cpp 
$(TARGET_MERGE_MAIN): merge_% : $(SOURCES_DIR)/%.cpp
	$(CXX_MERGE) $< -o $(MERGE_DIR)/$*_merged.cpp 

clean:
ifeq ($(OS), Windows_NT)
	del /f $(BUILD_DIR)\main.exe $(OTHER:%=$(BUILD_DIR)\\%) $(TARGET_PLAYER:%=$(BUILD_DIR)\player_%.exe)
	del /f $(BUILD_DIR)\merged\main_merged.cpp $(TARGET_PLAYER:%=$(BUILD_DIR)\merged\player_%_merged.cpp)
else
	rm -f $(BUILD_DIR)/main $(OTHER:%=$(BUILD_DIR)/%) $(TARGET_PLAYER:%=$(BUILD_DIR)/player_%)
	rm -f $(BUILD_DIR)/merged/main_merged.cpp $(TARGET_PLAYER:%=$(BUILD_DIR)/merged/player_%_merged.cpp)
endif
