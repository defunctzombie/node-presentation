all:
	$(MAKE) -C hello_world
	$(MAKE) -C hello_world_async
	$(MAKE) -C point

test: all
	$(MAKE) -C hello_world test
	$(MAKE) -C hello_world_async test
	$(MAKE) -C point test

clean:
	$(MAKE) -C hello_world clean
	$(MAKE) -C hello_world_async clean
	$(MAKE) -C point clean

.PHONY: all clean
