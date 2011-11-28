#pragma once

#include <node.h>

/// c++ implementation of a 2d point class
///
/// Object wrap is a conveience base class which can
/// wrap, unwrap, and refcount c++ objects <-> js objects
///
/// if you never need to create these objects from other c++
/// another approach is to have a Point namespace with an Init
/// method and then hide all of the details in the point.cpp file
/// using a .hpp/.cpp approach does have the advantage of being easier to read
class Point : public node::ObjectWrap
{
    double x;
    double y;

    /// used to create new Point instances from c++
    /// see the Normalized function
    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    // the constructor is private since points are only
    // created from the New function.
    Point(double x, double y)
        : x(x)
        , y(y)
    {}

public:
    /// create the function template and prototypes for a point
    /// Think of this as declaring what the Point will be able to do
    /// Attaches the c++ methods to symbols for the object and prototype
    static void
    Init(v8::Handle<v8::Object> target);

    /// Constructor, called when user instantiates a new Point using 'new Point'
    /// or when c++ creates a new Instance using the FunctionTemplate
    /// equivalent to function(x, y) { ... }
    static v8::Handle<v8::Value>
    New(const v8::Arguments& args);

    /// getter for x
    static v8::Handle<v8::Value>
    GetPointX(v8::Local<v8::String> property, const v8::AccessorInfo& info);

    /// getter for y
    static v8::Handle<v8::Value>
    GetPointY(v8::Local<v8::String> property, const v8::AccessorInfo& info);

    /// setter for x
    static void
    SetPointX(v8::Local<v8::String> property, v8::Local<v8::Value> value,
            const v8::AccessorInfo& info);

    /// setter for y
    static void
    SetPointY(v8::Local<v8::String> property, v8::Local<v8::Value> value,
            const v8::AccessorInfo& info);

    /// returns a new point which is the normalized version of our point
    static v8::Handle<v8::Value>
    Normalized(const v8::Arguments& args);

};

