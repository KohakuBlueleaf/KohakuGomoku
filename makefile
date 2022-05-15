CXX = g++
CXXFLAGS = --std=c++14

SOURCESDIR = src
BUILDDIR = build
SOURCES = $(wildcard $(SOURCESDIR)/*.cpp)
STATE_SOURCE = $(SOURCESDIR)/state/state.cpp
POLICY_SOURCE = $(wildcard $(SOURCESDIR)/policy/*.cpp)

TARGET = $(SOURCES:$(SOURCESDIR)/%.cpp=%)

ifeq ($(OS),Windows_NT)
EXE = $(SOURCES:$(SOURCESDIR)/%.cpp=%.exe)
else
EXE = $(SOURCES:$(SOURCESDIR)/%.cpp=%)
endif
OTHER = action state gamelog.txt

.PHONY: all clean

all: |$(BUILDDIR) $(TARGET)

$(BUILDDIR):
	mkdir $(BUILDDIR)

ifeq ($(OS),Windows_NT)
$(TARGET): % : $(SOURCESDIR)/%.cpp
	$(CXX) -Wall -Wextra $(CXXFLAGS) -o $(BUILDDIR)/$@.exe $(STATE_SOURCE) $(POLICY_SOURCE) $< 
else
$(TARGET): % : $(SOURCESDIR)/%.cpp
	$(CXX) -Wall -Wextra $(CXXFLAGS) -o $(BUILDDIR)/$@ $(STATE_SOURCE) $< 
endif

clean:
ifeq ($(OS),Windows_NT)
	del /f $(TARGET) $(OTHER)
else
	rm -f $(TARGET) $(OTHER)
endif
