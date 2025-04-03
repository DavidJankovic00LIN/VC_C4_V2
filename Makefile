CC = g++
PROJECT = c4_specifikacija
SRC = main.cpp interconnect.cpp hard.cpp cpu.cpp bram.cpp vp.cpp utils.cpp
OBJ = $(SRC:.cpp=.o)
SYSTEMC = /usr/local/systemc


CXXFLAGS = -g -Wall -I$(SYSTEMC)/include
CXXFLAGS += -Wno-unused-variable  
LDFLAGS = -L$(SYSTEMC)/lib-linux64 -lsystemc -lm

$(PROJECT): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)  

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

run:
	./$(PROJECT)

clean:
	rm -f $(PROJECT) *.o *.vcd

.PHONY: clean run