
#include <cmath>

#include <node.h>
#include <v8.h>

/// c++ implementation of a 2d point class
///
/// Object wrap is a conveience base class which can
/// wrap, unwrap, and refcount c++ objects <-> js objects
class Point : public node::ObjectWrap
{
    double x;
    double y;

    /// used to create new Point instances from c++
    /// see Normalized method below
    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    Point(double x, double y)
        : x(x)
        , y(y)
    {}

public:

    /// create the function template and prototypes for a point
    /// Think of this as declaring what the Point will be able to do
    /// Attaches the c++ methods to symbols for the object and prototype
    static void Init(v8::Handle<v8::Object> target)
    {
        // always have a scope object to manage the lifetime of locals you create
        // it is enough to have just one of these existing on the thread local stack
        // use more locally if you plan on creating more intermediate handles
        // that you then want deleted when the scope variable goes away
        v8::HandleScope scope;

        // create a function template (something we can construct an object from)
        // equivalent to var Point = function(x, y) ...
        v8::Local<v8::FunctionTemplate> funcTempl = v8::FunctionTemplate::New(Point::New);
        funcTempl->SetClassName(v8::String::NewSymbol("Point"));

        // the object templates are details for an individual instance of the object
        v8::Local<v8::ObjectTemplate> objTempl = funcTempl->InstanceTemplate();

        // internal fields are not accessible from js
        // this is where the raw pointer to the Point instance will be placed
        // it allows for the raw c++ Pointer instance to be retrieved from the js object
        objTempl->SetInternalFieldCount(1);

        // create accessors for the member variables
        // an alternative is to just create functions getX, setY, ... and hide the members
        // I like this approach as it is more idiomatic and still provides the typical
        // OOP "encapsilation" practices
        objTempl->SetAccessor(v8::String::New("x"), Point::GetPointX, Point::SetPointX);
        objTempl->SetAccessor(v8::String::New("y"), Point::GetPointY, Point::SetPointY);

        // convenience for adding a prototype method to the function template
        // equivalent to Point.prototype.normalize = function() ...
        NODE_SET_PROTOTYPE_METHOD(funcTempl, "normalized", Point::Normalized);
        // NODE SET PROTOTYPE METHOD is just shorthand for:
        //v8::Local<v8::FunctionTemplate> Normalized = v8::FunctionTemplate::New(Point::Normalize);
        //funcTempl->PrototypeTemplate()->Set(v8::String::NewSymbol("normalized"), Normalized);

        // expose the constructor function as the Point object into the module
        // equivalend to module.exports.Point = Point;
        target->Set(v8::String::NewSymbol("Point"), funcTempl->GetFunction());

        // store for future use in the Normalized method below
        // this is not the only way to create another instance from c++
        constructor_template = v8::Persistent<v8::FunctionTemplate>::New(funcTempl);
    }

    /// Constructor, called when user instantiates a new Point using 'new Point'
    /// or when c++ creates a new Instance using the FunctionTemplate
    /// equivalent to function(x, y) { ... }
    static v8::Handle<v8::Value> New(const v8::Arguments& args)
    {
        // scope to manage any local handles we create
        v8::HandleScope scope;

        // error check the constructor arguments
        if (args.Length() != 2)
            return v8::ThrowException(v8::String::New("x and y arguments required"));
        if (!args[0]->IsNumber() || !args[1]->IsNumber())
            return v8::ThrowException(v8::String::New("x and y arguments must be numbers"));

        // extract the values from the constructor arguments
        double x = args[0]->NumberValue();
        double y = args[1]->NumberValue();

        // create the raw c++ object, this stores the state completely in c++
        Point* point = new Point(x, y);

        // this is provided by ObjectWrap to set the internal field to the Point*
        // you must have SetInternalFieldCount to > 0 because this will set field 0
        point->Wrap(args.Holder());

        // return the 'this' argument into the parent scope
        return scope.Close(args.Holder());
    }

    /// getter for x
    static v8::Handle<v8::Value>
    GetPointX(v8::Local<v8::String> property,
            const v8::AccessorInfo& info)
    {
        v8::HandleScope scope;

        // extract the raw Point* from the js object
        // this is shorthand for getting the internal field point and casting it
        Point* point = ObjectWrap::Unwrap<Point>(info.Holder());

        // return the value as a js Number
        return scope.Close(v8::Number::New(point->x));
    }

    /// setter for x
    static void
    SetPointX(v8::Local<v8::String> property, v8::Local<v8::Value> value,
            const v8::AccessorInfo& info)
    {
        // again, unwrap from the js object instance
        Point* point = ObjectWrap::Unwrap<Point>(info.Holder());
        // set the value
        point->x = value->NumberValue();
    }

    static v8::Handle<v8::Value>
    GetPointY(v8::Local<v8::String> property,
            const v8::AccessorInfo& info)
    {
        v8::HandleScope scope;
        Point* point = ObjectWrap::Unwrap<Point>(info.Holder());
        return scope.Close(v8::Number::New(point->y));
    }

    static void
    SetPointY(v8::Local<v8::String> property, v8::Local<v8::Value> value,
            const v8::AccessorInfo& info)
    {
        Point* point = ObjectWrap::Unwrap<Point>(info.Holder());
        point->y = value->NumberValue();
    }

    /// normalized method
    /// All bound methods are static methods
    /// the 'this' argument is extracted from args
    static v8::Handle<v8::Value>
    Normalized(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        // unwrap from js instance
        Point* point = ObjectWrap::Unwrap<Point>(args.Holder());

        // do some math
        const double x = point->x;
        const double y = point->y;
        const double norm = std::sqrt(x*x + y*y);

        // create an array for the arguments to pass to the constructor
        // it is important that we get this right as the constructor expects
        // 2 arguments (x,y).
        v8::Local<v8::Value> c_args[2];
        c_args[0] = v8::Number::New(x/norm);
        c_args[1] = v8::Number::New(y/norm);

        // create a new instance of the Point 'class' passing in the arguments
        // also close the scope over the returned instance
        return scope.Close(constructor_template->GetFunction()->NewInstance(2, c_args));
    }
};

// c++ is fun ;)
v8::Persistent<v8::FunctionTemplate> Point::constructor_template;

void init(v8::Handle<v8::Object> target)
{
    // typically all of the things for a given class are encapsalated in it
    // here we delegate the initialization work (making the function template)
    // to the Init method in Point
    Point::Init(target);
}

// notice that 'point' is lower case even tho our class is uppercase
// if we want to have an uppercase module name we must change the filename accordingly
// the filename and module name MUST match (case matters)
NODE_MODULE(point, init);

