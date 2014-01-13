
#include <node.h>
#include <v8.h>

#ifdef __APPLE__
#include "impl-apple.cc"
#else
#error This platform is not implemented yet
#endif

void init(v8::Handle<v8::Object> exports) {
  exports->Set(v8::String::NewSymbol("getVolumeName"), v8::FunctionTemplate::New(MethodGetVolumeName)->GetFunction());
}

NODE_MODULE(volume, init)
