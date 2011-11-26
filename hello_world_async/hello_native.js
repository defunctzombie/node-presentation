
exports.print = function(string, cb) {
    process.nextTick(function() {
        process.stdout.write(string);
        cb();
    });
};

exports.printSync = function(string) {
    process.stdout.write(string);
};

