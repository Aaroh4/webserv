TARGET = test

FLAGS = -Wall -Wextra -Werror -std=c++11

SRCS = srcs/main.cpp srcs/conversion.cpp srcs/readconfig.cpp\
	srcs/ServerManager.cpp srcs/ServerInfo.cpp srcs/Request.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
		c++ $(FLAGS) -o $@ $^

%.o: %.cpp
	c++ $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean:
	rm -f $(OBJS)
	rm -f $(TARGET)

re:	fclean all