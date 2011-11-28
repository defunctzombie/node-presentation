
/// 2d point
var Point = function(x, y) {
    this.x = x;
    this.y = y;
};

/// @return a new point with the x, y being normalized
/// length of 1
Point.prototype.normalized = function() {
    var norm = Math.sqrt(this.x * this.x + this.y * this.y);
    return new Point(this.x/norm, this.y/norm);
};

module.exports = Point;
