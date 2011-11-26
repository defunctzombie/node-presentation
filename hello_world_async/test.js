
// load the native module
var native = require('./hello_native');

// load the file hello.node (must be built first)
// READ THE DOCS ON MODULE LOADING
var bindings = require('./build/Release/hello');

// alternate way to expose bindings to end user
var wrapper = require('./hello_bindings');

// even tho this is called first, the execution of the print
// is deferred to the 'nextTick'
native.print('Sometimes the ', function() {
    native.printSync('order\n');
});

// this will print right away
native.printSync('doesn\'t matter. ');

// invoke the c++ bindings after printing from native
// otherwise we get 'doesn't matter twice first
// we can easily put this in the callback of the native 'print' call
// using timeout here to just show the example
require('timers').setTimeout(function() {
    bindings.print('Sometimes the ', function() {
        bindings.printSync('order\n');
    });
    bindings.printSync('doesn\'t matter. ');
}, 30);

require('timers').setTimeout(function() {
    wrapper.print('Sometimes the ', function() {
        wrapper.print('order\n');
    });
    wrapper.print('doesn\'t matter. ');
}, 60);
