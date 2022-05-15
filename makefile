CXX = g++
CXXFLAGS = --std=c++14

SOURCESDIR = src
BUILDDIR = build
SOURCES = $(wildcard $(SOURCESDIR)/*.cpp)

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
	$(CXX) -Wall -Wextra $(CXXFLAGS) -o $(BUILDDIR)/$@.exe $<
else
$(TARGET): % : $(SOURCESDIR)/%.cpp
	$(CXX) -Wall -Wextra $(CXXFLAGS) -o $(BUILDDIR)/$@ $<
endif

clean:
ifeq ($(OS),Windows_NT)
	del /f $(TARGET) $(OTHER)
else
	rm -f $(TARGET) $(OTHER)
endif
