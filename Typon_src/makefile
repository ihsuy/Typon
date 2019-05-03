CXX = g++
CXXFLAGS = -std=c++11 -O3
LDLIB = -lncurses -lpthread


DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PREFIX=/usr/local



objects = main.o \
          game_engine.o graphics.o \
          quote_loader.o save_handler.o \
          game_mode.o timer.o tools.o special_keys.o

typon: $(objects) 
	$(CXX) -o typon $(CXXFLAGS) $(objects) $(LDLIB)
	mv *.o build/

main.o: ./src/main.cpp 
	$(CXX) -c $(CXXFLAGS) ./src/main.cpp
game_engine.o: ./src/game_engine.cpp ./src/game_engine.hpp
	$(CXX) -c $(CXXFLAGS) ./src/game_engine.cpp
graphics.o: ./src/graphics.cpp ./src/graphics.hpp
	$(CXX) -c $(CXXFLAGS) ./src/graphics.cpp
quote_loader.o: ./src/quote_loader.cpp ./src/quote_loader.hpp
	$(CXX) -c $(CXXFLAGS) ./src/quote_loader.cpp
save_handler.o: ./src/save_handler.cpp ./src/save_handler.hpp
	$(CXX) -c $(CXXFLAGS) ./src/save_handler.cpp
game_mode.o: ./src/game_mode.cpp ./src/game_mode.hpp
	$(CXX) -c $(CXXFLAGS) ./src/game_mode.cpp
timer.o: ./src/timer.cpp ./src/timer.hpp
	$(CXX) -c $(CXXFLAGS) ./src/timer.cpp
tools.o: ./src/tools.cpp ./src/tools.hpp
	$(CXX) -c $(CXXFLAGS) ./src/tools.cpp
special_keys.o: ./src/special_keys.cpp
	$(CXX) -c $(CXXFLAGS) ./src/special_keys.cpp

.PHONY: clean install uninstall
clean: 
	rm -f ./typon
	rm -f ./build/*    
install: 
	install -d $(DESTDIR)$(PREFIX)/bin
	exec ./typon -new
	install typon_run.sh $(DESTDIR)$(PREFIX)/bin/typon
	rm -f typon_run.sh
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/typon
