#-------------------------------------------------------------------
all : ushcn.exe

ushcn.exe : Makefile Main.cpp USHCN.cpp USHCN.h
	g++ -O3 -o ushcn.exe Main.cpp USHCN.cpp

clean :
	rm -f ushcn.exe
