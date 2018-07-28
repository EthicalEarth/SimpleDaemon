CC  = g++
CFLAGS  = -I. -Wall -lwiringPi  -Wformat-security -fpermissive -Wreturn-local-addr

all: daemon-control clean

daemon-control: daemon-control.o
	$(CC) -o $@ $(CFLAGS) $^ 
	chmod 700 daemon-control

daemon-control.o: daemon-control.cpp
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
