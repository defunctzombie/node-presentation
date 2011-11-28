// load the native module
var native = require('./hello_native');

// load the file hello.node (must be built first)
// READ THE DOCS ON MODULE LOADING
var bindings = require('./build/Release/hello');

// alternate way to expose bindings to end user
var wrapper = require('./hello_bindings');

var trace = [];

function run_native() {
    // even tho this is called first, the execution of the print
    // is deferred to the 'nextTick'
    native.print('Sometimes the ', function() {
        trace.push('async');

        native.printSync('order\n');

        // run the same thing for our c++ bindings
        run_bindings();
    });

    trace.push('sync');

    // this will print right away
    native.printSync('doesn\'t matter. ');
}

function run_bindings() {
    bindings.print('Sometimes the ', function() {
        trace.push('async');

        bindings.printSync('order\n');

        // run the same thing for the wrapper
        run_wrapper();
    });

    trace.push('sync');
    bindings.printSync('doesn\'t matter. ');
}

function run_wrapper() {
    wrapper.print('Sometimes the ', function() {
        trace.push('async');
        wrapper.print('order\n');

        // sanity check for execution order
        require('assert').deepEqual(['sync', 'async', 'sync', 'async', 'sync', 'async'], trace);
    });

    trace.push('sync');
    wrapper.print('doesn\'t matter. ');
}

// start the print chain
run_native();

// output should be
//doesn't matter. Sometimes the order
//doesn't matter. Sometimes the order
//doesn't matter. Sometimes the order
