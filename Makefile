TARGET = webserv

FLAGS = -std=c++17 #-Wall -Wextra -Werror -g

SRCS = srcs/main.cpp srcs/conversion.cpp srcs/readconfig.cpp\
	srcs/ServerManager.cpp srcs/ServerInfo.cpp srcs/Request.cpp\
	srcs/Response.cpp

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