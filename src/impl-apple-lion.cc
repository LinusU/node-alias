
#define NAPI_VERSION 3
#include <napi.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFString.h>

Napi::String MYCFStringGetV8String(Napi::Env env, CFStringRef aString) {
  if (aString == NULL) {
    return Napi::String::New(env, "");
  }

  CFIndex length = CFStringGetLength(aString);
  CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
  char *buffer = (char *) malloc(maxSize);

  if (!CFStringGetCString(aString, buffer, maxSize, kCFStringEncodingUTF8)) {
    return Napi::String::New(env, "");
  }

  Napi::String result = Napi::String::New(env, buffer);
  free(buffer);

  return result;
}

Napi::Value MethodGetVolumeName(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  std::string aPath = info[0].As<Napi::String>();

  CFStringRef out;
  CFErrorRef error;

  CFStringRef volumePath = CFStringCreateWithCString(NULL, aPath.c_str(), kCFStringEncodingUTF8);
  CFURLRef url = CFURLCreateWithFileSystemPath(NULL, volumePath, kCFURLPOSIXPathStyle, true);

  if(!CFURLCopyResourcePropertyForKey(url, kCFURLVolumeNameKey, &out, &error)) {
    throw Napi::Error::New(env, MYCFStringGetV8String(env, CFErrorCopyDescription(error)));
  }

  return MYCFStringGetV8String(env, out);
}
