#include <iostream>
#include <cassert>

#include <node.h>
#include <v8.h>

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
void EIO_Print(eio_req* req)
{
    // the baton pointer can be found in the 'data' member of req
    // it is important to know what the type was when you cast it
    Baton* baton = static_cast<Baton*>(req->data);
    assert(baton);

    // the 'work'
    std::cout << baton->to_print;

    // if your work has any output that needs to be returned to the user
    // you will need to use the same baton object
    // look at EIO_AfterPrint for the final stage
}

int EIO_AfterPrint(eio_req* req)
{
    // very important to deref the event loop
    // in the event of something going wrong here we don't want to
    // hang a possible exit
    ev_unref(EV_DEFAULT_UC);

    // again we need to get our baton object
    Baton* baton = static_cast<Baton*>(req->data);
    assert(baton);

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
    {
        node::FatalException(try_catch);
    }

    // we have to inform v8 that it can garbage collect our function
    // since we no longer need the function after executing it
    baton->callback.Dispose();

    // cleanup our baton object
    // you could put the dispose call in the destructor
    delete baton;

    return 0;
}

/// async print method
/// takes 2 arguments (string and callback)
/// after the string is printed (on next tick) the callback is invoked
v8::Handle<v8::Value> Print(const v8::Arguments& args) {

    v8::HandleScope scope;

    // check if the user provided an argument and that it was a string
    // usually you will want to use v8::ThrowException(v8::Exception::Error())
    // this will create an actual error object with a stack
    if (args.Length() != 2)
        return v8::ThrowException(v8::String::New("must provide a string and a callback"));
    else if (!args[0]->IsString())
        return v8::ThrowException(v8::String::New("first argument must be a string"));
    else if (!args[1]->IsFunction())
        return v8::ThrowException(v8::String::New("second argument must be a function"));

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

    // queue our work for the thread pool
    // 1st argument EIO_Print is the function to call to do the actual work
    // 3rd argument EIO_AFterPrint is the function to call after work is done
    // last argument is a pointer to the baton
    eio_custom(EIO_Print, EIO_PRI_DEFAULT, EIO_AfterPrint, baton);

    // add a ref count to the event loop
    // the event loop will not exit as long as the ref count > 0
    // this is important because if libev things there are no more events then the
    // loop (and program will exit). Since your 'print' invocation returns immedately
    // you need to make sure the event loop waits around for you to finish the work
    // see the EIO_AfterPrint method for the matching unref call
    ev_ref(EV_DEFAULT_UC);

    return scope.Close(v8::Undefined());
}

/// Syncronous version of the above print method
/// takes 1 argument (a string) and prints it to stdout
v8::Handle<v8::Value> PrintSync(const v8::Arguments& args) {

    v8::HandleScope scope;

    // check if the user provided an argument and that it was a string
    // usually you will want to use v8::ThrowException(v8::Exception::Error())
    // this will create an actual error object with a stack
    if (args.Length() != 1)
        return v8::ThrowException(v8::String::New("must provide one argument"));
    else if (!args[0]->IsString())
        return v8::ThrowException(v8::String::New("argument must be a string"));

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
void init(v8::Handle<v8::Object> target) {

    // add a new method called 'hello' using 'Method' as the
    // function that will execute
    NODE_SET_METHOD(target, "print", Print);
    NODE_SET_METHOD(target, "printSync", PrintSync);
}

// makes the module available for loading
// 'hello' is the module name
// init is the method that will be called to load the module
NODE_MODULE(hello, init)
