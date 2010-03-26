MM=/home/martin/src/micromanager1.3
LDFLAGS=-lSDL
NAME=sdlslm
all: $(NAME).cpp
	g++ -g -Wall -W -Wall -fPIC -shared -o libmmgr_dal_$(NAME).so -I$(MM) $(NAME).cpp $(MM)/MMDevice/.libs/libMMDevice.a $(LDFLAGS)

install:
	sudo cp libmmgr_dal_$(NAME).so /usr/lib/micro-manager/libmmgr_dal_$(NAME).so.0
