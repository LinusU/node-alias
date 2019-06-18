
#ifdef __APPLE__
#include "impl-apple.cc"
#else
#error This platform is not implemented yet
#endif

Napi::Object Initialize(Napi::Env env, Napi::Object exports) {
  exports["getVolumeName"] = Napi::Function::New(env, MethodGetVolumeName, "getVolumeName");
  return exports;
}

NODE_API_MODULE(volume, Initialize)
