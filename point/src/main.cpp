/// entry point for our module

#include "point.hpp"

void init(v8::Handle<v8::Object> target)
{
    // typically all of the things for a given class are encapsalated in it
    // here we delegate the initialization work (making the function template)
    // to the Init method in Point
    Point::Init(target);
}

// the module name 'point' needs to match the final shared object name
NODE_MODULE(point, init);

