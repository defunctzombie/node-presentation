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

////// see the native point.js file for details ///////

// test the merged point that has been extended
// with the native 'length' method
var Point = require('./point');

var p = new Point(1, 1);

// since we return from the Point function
// we can also just do (prefered by some people)
//var p = Point(1, 1);

// test that 'p' is an instance of all the expected types
assert.ok(p instanceof CppPoint);
assert.ok(p instanceof Point);
// not the same as our native p
assert.ok(! (p instanceof NativePoint));

// basic test of the length method
assert.equal(1.4142, Math.floor(p.length() * 10000)/10000);

// run the same tests on the merged binding as the others
run_tests(Point);

