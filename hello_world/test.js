
// load the native module
var native = require('./hello_native');

// load the file hello.node (must be built first)
// READ THE DOCS ON MODULE LOADING
var bindings = require('./build/Release/hello');

// show the output from the two different functions
console.log('native:', native.hello());
console.log('bindings:', bindings.hello());

// test
var assert = require('assert');
assert.equal(native.hello(), bindings.hello());
