.PHONY: all drivers lint clean

DRIVERS :=$(sort $(wildcard */))

all: drivers
	@:

drivers: $(DRIVERS)
	@make -C $<

lint: $(DRIVERS)
	@make -C $< lint

clean: $(DRIVERS)
	@-make -C $< clean
