CXXFLAGS = -std=gnu++11 -Wall -Wextra -fdiagnostics-color=always -D_GNU_SOURCE

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

OUTPUT = reminder

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	g++ $(OBJECTS) -o $(OUTPUT)

.PHONY: clean
clean:
	rm -f $(OUTPUT) $(OBJECTS)
