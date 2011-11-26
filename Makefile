all:
	$(MAKE) -C hello_world test
	$(MAKE) -C hello_world_async test

clean:
	$(MAKE) -C hello_world clean
	$(MAKE) -C hello_world_async clean

.PHONY: all clean
