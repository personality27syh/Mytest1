SRC:=$(wildcard ./*.cpp)
OBJS:=$(patsubst %.cpp, %.o, $(SRC))
GCC:=g++
INC:=./

TARGET:=test

all:$(TARGET)
$(TARGET):$(OBJS)
	$(GCC) $^ -I$(INC) -lcrypto -pthread -lclntsh -locci -o $@

%.o:%.cpp
	$(GCC) -c -std=c++11 $< -o $@

.PHONY:clean
clean:
	$(RM) $(OBJS) $(TARGET)
