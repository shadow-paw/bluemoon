.PHONY: all clean

WARN    =-Wall -Wextra -Wno-unused-parameter -Wno-multichar
CFLAGS  =-O3 -c $(WARN)
CXXFLAGS=-O3 -c $(WARN)

BIN_PATH=../../../bin
PROJECT_BIN :=$(BIN_PATH)/mkosimage
PROJECT_TMP :=.build/
PROJECT_C   :=$(wildcard *.c)
PROJECT_CPP :=$(wildcard *.cpp)
PROJECT_OBJ :=$(patsubst %.c,   $(PROJECT_TMP)%_c.o,  $(PROJECT_C)) \
	      $(patsubst %.cpp, $(PROJECT_TMP)%_cpp.o,$(PROJECT_CPP))

all: $(PROJECT_BIN)
	@:

$(PROJECT_BIN): $(PROJECT_TMP) $(PROJECT_OBJ) Makefile
	@echo [LINK] $@
	@mkdir -p "$(BIN_PATH)"
	@$(CXX) -lstdc++ -o $@ $(PROJECT_OBJ)

$(PROJECT_TMP):
	@mkdir -p $@

$(PROJECT_TMP)%_c.o: %.c
	@echo [GCC ] $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(PROJECT_TMP)%_cpp.o: %.cpp
	@echo [CXX ] $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@


##############################################################
clean:
	@rm -f $(PROJECT_OBJ)
