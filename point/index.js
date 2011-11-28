/// the index.js file is the entry point for a project
/// when requiring a folder, the index.js file is what gets
/// loaded

// this is not a solution for all cases, this will load all three modules
// when index.js itself is loaded
//
// if you want lazy loading only when the user wants a feature
// you will need to expose that yourself
// you can use factory like functions, fs paths, etc...

// native point
module.exports.Native = require('./lib/point_native');

// our pure c++ clone of native point
// c++ attached 'Point' into the module object
// the point.node file itself is not the Point class
// this is different from the Native point module above
module.exports.Cpp = require('./build/Release/point').Point;

// c++ point extended with native methods
module.exports.Point = require('./lib/point');

