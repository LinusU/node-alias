
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFString.h>

using namespace v8;

Local<String> MYCFStringGetV8String(CFStringRef aString) {

  if (aString == NULL) {
    return String::New("");
  }

  CFIndex length = CFStringGetLength(aString);
  CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
  char *buffer = (char *) malloc(maxSize);

  if (CFStringGetCString(aString, buffer, maxSize, kCFStringEncodingUTF8)) {

    Local<String> result = String::New(buffer);
    free(buffer);

    return result;
  }

  return String::New("");
}

Handle<Value> MethodGetVolumeName(const Arguments& args) {
  HandleScope scope;

  String::AsciiValue aPath(args[0]);

  CFStringRef out;
  CFErrorRef error;

  CFStringRef volumePath = CFStringCreateWithCString(NULL, *aPath, kCFStringEncodingUTF8);
  CFURLRef url = CFURLCreateWithFileSystemPath(NULL, volumePath, kCFURLPOSIXPathStyle, true);

  if(CFURLCopyResourcePropertyForKey(url, kCFURLVolumeNameKey, &out, &error)) {

    Local<String> result = MYCFStringGetV8String(out);

    return scope.Close(result);
  } else {

    Local<String> desc = MYCFStringGetV8String(CFErrorCopyDescription(error));
    ThrowException(Exception::Error(desc)->ToObject());

    return scope.Close(Undefined());
  }
}
