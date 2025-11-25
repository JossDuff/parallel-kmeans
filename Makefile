CXX = g++
CXXFLAGS = -std=c++11 -msse2 -O3 -pthread -Wall -Wextra -I./include/kmeans -I./include/prime -I./include
LDFLAGS = -L$(TBBROOT)/lib/intel64/gcc4.8 -Wl,-rpath,$(TBBROOT)/lib/intel64/gcc4.8 -ltbb

# Check if TBB environment is set
ifndef TBBROOT
$(error TBB environment not set. Please run: source oneapi-tbb-2022.2.0/env/vars.sh)
endif

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj

# Both executables
TARGETS = prime kmeans test-kmeans

# Prime sources and objects
PRIME_SRCS = $(SRCDIR)/prime/main.cpp \
             $(SRCDIR)/prime/serial.cpp \
			$(SRCDIR)/prime/tbb.cpp \
 			$(SRCDIR)/prime/static.cpp \
			$(SRCDIR)/prime/dynamic.cpp 

PRIME_OBJS = $(PRIME_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# K-means sources and objects
KMEANS_SRCS = $(SRCDIR)/kmeans/main.cpp 
KMEANS_OBJS = $(KMEANS_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

TEST_KMEANS_SRCS = $(SRCDIR)/kmeans/test.cpp 
TEST_KMEANS_OBJS = $(TEST_KMEANS_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Default target builds both
all: $(TARGETS)

# Generic rule for object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean everything
clean:
	rm -rf $(OBJDIR) $(TARGETS)

# Build without running
build-prime: $(PRIME_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o prime $(LDFLAGS)
	@echo "Built prime executable"

build-kmeans: $(KMEANS_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o kmeans $(LDFLAGS)
	@echo "Built kmeans executable"

build-test-kmeans: $(TEST_KMEANS_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o test-kmeans $(LDFLAGS)
	@echo "Built test-kmeans executable"

build: build-prime build-kmeans build-test-kmeans

# default N if not supplied
N ?= 10000

prime: build-prime
	@echo "Benchmarking Prime implementations:"
	./prime $(N)

# default file if not provided
file ?= "kmeans-data/dataset1.txt"

kmeans: build-kmeans
	@echo "Benchmarking K-Means implementations:"
	./kmeans < $(file)

test-kmeans: build-test-kmeans
	@echo "Testing K-Means implementations:"
	./test-kmeans < $(file)


.PHONY: all clean build build-prime build-kmeans prime kmeans test-kmeans
