
#include <Availability.h>
#ifndef __MAC_10_7
  #include <Carbon/Carbon.h>
#endif
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFString.h>

using namespace v8;

#ifndef __MAC_10_7
  bool oldVolumeNameFromURL(CFURLRef url, CFStringRef *out) {
    FSRef urlFS;
    FSCatalogInfo urlInfo;
    HFSUniStr255 outHFS;

    if (CFURLGetFSRef(url, &urlFS)) {
      if (FSGetCatalogInfo(&urlFS, kFSCatInfoVolume, &urlInfo, NULL, NULL, NULL) == noErr) {
        if (FSGetVolumeInfo(urlInfo.volume, 0, NULL, kFSVolInfoNone, NULL, &outHFS, NULL) == noErr) {
          out = (CFStringRef *) CFStringCreateWithCharacters(NULL, outHFS.unicode, outHFS.length);
          return true;
        }
      }
    }
    return false;
  }
#endif

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
  #ifdef __MAC_10_7
    CFErrorRef error;
  #endif
  bool gotName;

  CFStringRef volumePath = CFStringCreateWithCString(NULL, *aPath, kCFStringEncodingUTF8);
  CFURLRef url = CFURLCreateWithFileSystemPath(NULL, volumePath, kCFURLPOSIXPathStyle, true);

  #ifdef __MAC_10_7
    gotName = CFURLCopyResourcePropertyForKey(url, kCFURLVolumeNameKey, &out, &error);
  #else
    gotName = oldVolumeNameFromURL(url, &out);
  #endif

  if (gotName) {

    Local<String> result = MYCFStringGetV8String(out);

    return scope.Close(result);
  } else {

    Local<String> desc;
    #ifdef __MAC_10_7
      desc = MYCFStringGetV8String(CFErrorCopyDescription(error));
    #else
      desc = String::New("Could not get volume name from FSRef.");
    #endif
    ThrowException(Exception::Error(desc)->ToObject());

    return scope.Close(Undefined());
  }
}
