target=./build/Release/point.node

all: $(target)

test: all
	node test.js

$(target): build/ src/*
	node-waf build

build/:
	node-waf configure

clean:
	rm -rf build

.PHONY: clean
