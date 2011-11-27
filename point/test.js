var assert = require('assert');

var NativePoint = require('./point_native');
var CppPoint = require('./build/Release/point').Point;

function run_tests(FunctionTemplate) {
    var p = new FunctionTemplate(3, 1);

    // in node.js assert is actually (actual, expected)
    // this is backwards from everything so I have ignored it here
    assert.equal(3, p.x);
    assert.equal(1, p.y);

    var p2 = new FunctionTemplate(1, 1);
    var norm = p2.normalized();
    assert.equal(0.707, Math.floor(norm.x * 1000) / 1000);
    assert.equal(0.707, Math.floor(norm.y * 1000) / 1000);
};

// the constructor 'function' is nothing more than another
// variable we can pass around
run_tests(NativePoint);

// we can just as easily pass the bindings interface to our test
// if you are careful about how you expose interfaces you can easily
// prototype in javascript and then finalize what you need in cpp
run_tests(CppPoint);

