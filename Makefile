compile:
	gcc -o smm-fan smm-fan.c

clean:
	rm -rf smm-fan smm-fan-dist

package:
	mkdir smm-fan-dist
	cp smm-fan LICENSE README.md smm-fan-dist
	tar cvjf smm-fan-dist.tar.bz2 smm-fan-dist

all: clean compile package

