target=./build/Release/point.node

all: $(target)

test: all
	node test.js

$(target): build/ point.cpp
	node-waf build

build/:
	node-waf configure

clean:
	rm -rf build

.PHONY: clean
