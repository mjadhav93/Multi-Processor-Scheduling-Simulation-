CXX ?= g++
CXXFLAGS := -std=c++20 -O2 -pthread

SRC := main.cpp Scheduler.cpp
HDR := Scheduler.h Processor.h Task.h Log.h
OUT := os_sim

all: $(OUT)

$(OUT): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
