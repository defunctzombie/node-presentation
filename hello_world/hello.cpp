// this provides helper functions like NODE_SET_METHOD
#include <node.h>

// v8 objects and everything else
#include <v8.h>

// usually you will want this
// commented out to make it clear what comes from v8
//using namespace v8;

// args is an array of the arguments passed to the function
// you can index into it using [ ] and also call .Length() to get the size
// NOTE: v8 methods start with uppercase letters by convention
v8::Handle<v8::Value> Func(const v8::Arguments& args) {

    // this manages the 'scope' of any v8 heap objects you create
    // anything created as a 'local'
    v8::HandleScope scope;

    // we could return the string directly, but for demonstration
    // we will use the longer form
    //return scope.Close(v8::String::New("world"));

    // two things are happening here
    // we are creating a new variable in the 'local' stack space
    // this variable is 'managed' by the scope we created above
    // To make the string object, we use the ::New static function
    // v8 objects are never created with the keyword 'new'
    v8::Local<v8::String> world = v8::String::New("world");

    // Handles are like pointers in their access patterns
    // use -> to call the methods of the underlying type

    // copies the value into the parent scope and removes all local handles
    // the parent scope is not responsible for managing the object returned
    // in this case, we will be returning to js execution
    return scope.Close(world);
}

// this function will be called upon loading the module
// 'target' is the module insance which we can attach methods to
// you can think of 'target' like an object
void init(v8::Handle<v8::Object> target) {

    // add a new method called 'hello' using 'Method' as the
    // function that will execute
    NODE_SET_METHOD(target, "hello", Func);
}

// makes the module available for loading
// 'hello' is the module name and it MUST match the filename
// init is the method that will be called to load the module
NODE_MODULE(hello, init)
