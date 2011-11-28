#include <iostream>
#include <cassert>

#include <node.h>

// the baton is how work is passed between the various stages of the
// thread pool cycle
struct Baton
{
    std::string to_print;
    v8::Persistent<v8::Function> callback;
};

/// this is the method that will do the actual 'work'
/// in our case this is just to print to stdout
/// you will not want to do something so trivial usually :)
void Print(uv_work_t* req)
{
    // the baton pointer can be found in the 'data' member of req
    // it is important to know what the type was when you cast it
    Baton* baton = static_cast<Baton*>(req->data);
    assert(baton);

    // the 'work'
    std::cout << baton->to_print;

    // if your work has any output that needs to be returned to the user
    // you will need to use the same baton object
    // look at AfterPrint for the final stage
}

void AfterPrint(uv_work_t* req)
{
    // again we need to get our baton object
    Baton* baton = static_cast<Baton*>(req->data);
    assert(baton);

    // you are responsible for the lifetime of the uv_work_t object
    // it should be deleted in the 'after' method
    // you could put a copy of it into the baton for easier mem management
    // but this is strange design imho so I have omitted it here to be clear
    // about what has to happen
    delete req;

    // manage lifetime of any locals we create here
    // in our case there are none
    v8::HandleScope scope;

    // this will capture any exception thrown from the function call
    v8::TryCatch try_catch;

    // our callback does not take any arguments
    baton->callback->Call(v8::Context::GetCurrent()->Global(), 0, NULL);

    // the function threw an exception
    // since we don't know where we are in the execution stack, we
    // inform node of the exception and it will handle throwing it for us
    if (try_catch.HasCaught())
        node::FatalException(try_catch);

    // we have to inform v8 that it can garbage collect our function
    // since we no longer need the function after executing it
    baton->callback.Dispose();

    // cleanup our baton object
    // you could put the dispose call in the destructor
    delete baton;
}

/// async print method
/// takes 2 arguments (string and callback)
/// after the string is printed (on next tick) the callback is invoked
v8::Handle<v8::Value> Print(const v8::Arguments& args)
{
    v8::HandleScope scope;

    // check if the user provided an argument and that it was a string
    // usually you will want to use v8::ThrowException(v8::Exception::Error())
    // this will create an actual error object with a stack
    if (args.Length() != 2)
        return v8::ThrowException(v8::Exception::Error(
                v8::String::New("must provide a string and a callback")));
    else if (!args[0]->IsString())
        return v8::ThrowException(v8::Exception::Error(
                v8::String::New("first argument must be a string")));
    else if (!args[1]->IsFunction())
        return v8::ThrowException(v8::Exception::Error(
                v8::String::New("second argument must be a function")));

    // a 'baton' object is what manages the data going into and out of the
    // worker thread. You can create any class you want and use different baton types
    // for different async methods.
    // In our example, we just need to store the string to print and the callback
    Baton* baton = new Baton();

    // store the string to print
    // it is important to either make copies of the object into real c++ objects
    // or to store persistent handles. All locals (arguents) will be up for garbage
    // collection if you do not store a persistent handle.
    // here we just make a c++ std::string since it will copy the characters for us
    v8::String::Utf8Value string(args[0]);
    baton->to_print.assign(*string, string.length());

    // It is very important that the function argument be converted to persistent
    // if you fail to do this, v8 can garbage collect the function/closure and
    // there will be nothing to call when you return from the thread
    baton->callback = v8::Persistent<v8::Function>::New(args[1].As<v8::Function>());

    // the uv request, manages request details and holds our baton
    // we must manage the lifetime of this object ourselves
    uv_work_t* req = new uv_work_t();

    // the data pointer can be anything we want, convention is to use
    // a 'baton' like object for input/output
    req->data = baton;

    // ask uv to queue our work into the thread pool
    // there are 3 arguments we care about here
    //   req is the actual req object, for now we have to manage the memory
    //       allocation and deallocation ourselves
    //   Print will do the actual work in the separate thread.
    //       you should not call v8 methods from this thread!
    //   AfterPrint is called once we are back in the main loop
    //       you can call v8 methods and should call the callback
    uv_queue_work(uv_default_loop(), req, Print, AfterPrint);

    return scope.Close(v8::Undefined());
}

/// Syncronous version of the above print method
/// takes 1 argument (a string) and prints it to stdout
v8::Handle<v8::Value> PrintSync(const v8::Arguments& args)
{
    v8::HandleScope scope;

    // check if the user provided an argument and that it was a string
    // usually you will want to use v8::ThrowException(v8::Exception::Error())
    // this will create an actual error object with a stack
    if (args.Length() != 1)
        return v8::ThrowException(v8::Exception::Error(
                v8::String::New("must provide one argument")));
    else if (!args[0]->IsString())
        return v8::ThrowException(v8::Exception::Error(
                v8::String::New("argument must be a string")));

    // convert the argument to a local string
    // in our case, we will just print directly so we don't need it
    //v8::Local<v8::String> world = args[0]->ToString();

    // Utf8Value overload operator* to return a const char* of the string
    std::cout << *v8::String::Utf8Value(args[0]);

    // close the scope, always a good idea to do this in the function
    // that will be called directly from v8
    return scope.Close(v8::Undefined());
}

// this function will be called upon loading the module
// 'target' is the module insance which we can attach methods to
// you can think of 'target' like an object
void init(v8::Handle<v8::Object> target)
{
    // add a new method called 'hello' using 'Method' as the
    // function that will execute
    NODE_SET_METHOD(target, "print", Print);
    NODE_SET_METHOD(target, "printSync", PrintSync);
}

// makes the module available for loading
// 'hello' is the module name and it MUST match the filename
// init is the method that will be called to load the module
NODE_MODULE(hello, init)
