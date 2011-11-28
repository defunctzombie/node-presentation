/// shows how to make a c++ class mesh with a js class
/// here we show how to extend a c++ described class using
/// native methods.
///
/// The same could be done the other way
/// but is not as easy as the created object will not have
/// the ability to store internal fields

// the bindings are never exposed directly
var CppPoint = require('../build/Release/point').Point;

// creates a point from the bindings
var Point = function(x, y) {
    // we cannot just call the super constructor
    // our object does not have internal fields and thus
    // the c++ pointer to Point cannot be stored
    // instead we create the point here and return it

    // create a new c++ point
    // we never actually create a js Point
    var bound = new CppPoint(x, y);
    return bound;
};

// makes Point the same thing as the Point from c++
// Point now has all of the methods of the c++ one
// any functions we attach to the js Point prototype onward
// are also attached to the bindings Point (which is what we want)
Point.prototype = CppPoint.prototype;

/// extend our bound Point class with a new method
/// the new method returns the length of the Point (vector)
/// using this approach (and variations) we can completely
/// hide the c++ bound point in a thin wrapper or add
/// methods to extend its functionality like we do here
Point.prototype.length = function() {
    return Math.sqrt(this.x * this.x + this.y * this.y);
};

// export the point object, using the Point method like a constructo
// will yield a bound c++ point with the new methods
module.exports = Point;
