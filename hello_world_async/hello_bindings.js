// alternate way to expose your bindings to the user
// instead of loading and exporting your bindings directly
// you will make a thin 'wrapper' around them for error checking
// and documentation, your end user never has to read the c++ code

// the bindings themselves are never exposed to the module
// only the module itself accesses the functions of the bindings
var bindings = require('./build/Release/hello');

/// print the string to stdout
/// if the callback parameter is omitted, then string is printed sync
/// if callback is provided, string is printed async
/// @param {String} string the string to print
/// @param {Function} [cb] the callback if async
///
/// NOTE: I am not advocating combining the printSync and print methods
/// meerly showing an alternate way to wrap the bindings and do error checking
/// you can do the same thing wrapping approach and keep the two different function
/// names if that suits you better
module.exports.print = function(string, cb) {

    // since we do error checking here, you _could_ avoid it in c++
    // this is for YOU to decide and document
    if (!string || typeof string !== 'string') {
        throw new Error('string must be specified and must be a string');
    }

    // if no callback, use sync version
    if (!cb) {
        return bindings.printSync(string);
    }

    // we have something for callback
    // check that it is a function
    if (typeof cb !== 'function') {
        throw new Error('callback argument must be a function');
    }

    // print using async version from bindings
    return bindings.print(string, cb);
};
