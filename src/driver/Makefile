.PHONY: all lint clean

DRIVERS :=$(sort $(wildcard */))


all: $(DRIVERS)
	@:

lint: $(DRIVERS)
	@:

clean: $(DRIVERS)
	@:

$(DRIVERS): FORCE
	@make --no-print-directory -C $@ $(MAKECMDGOALS)

FORCE:

