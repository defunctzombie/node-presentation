target=./build/Release/hello.node

all: $(target)

test: all
	node test.js

$(target): build/ hello.cpp
	node-waf build

build/:
	node-waf configure

clean:
	rm -rf build

.PHONY: clean
