
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFString.h>

using namespace v8;

Local<String> OSErrDescription(OSErr err) {

  switch (err) {
    case nsvErr: return String::New("Volume not found");
    case ioErr: return String::New("I/O error.");
    case bdNamErr: return String::New("Bad filename or volume name.");
    case mFulErr: return String::New("Memory full (open) or file won't fit (load)");
    case tmfoErr: return String::New("Too many files open.");
    case fnfErr: return String::New("File or directory not found; incomplete pathname.");
    case volOffLinErr: return String::New("Volume is offline.");
    case nsDrvErr: return String::New("No such drive.");
    case dirNFErr: return String::New("Directory not found or incomplete pathname.");
    case tmwdoErr: return String::New("Too many working directories open.");
  }

  return String::New("Could not get volume name");
}

v8::Handle<Value> MethodGetVolumeName(const Arguments& args) {
  HandleScope scope;

  String::AsciiValue aPath(args[0]);

  CFStringRef volumePath = CFStringCreateWithCString(NULL, *aPath, kCFStringEncodingUTF8);
  CFURLRef url = CFURLCreateWithFileSystemPath(NULL, volumePath, kCFURLPOSIXPathStyle, true);

  OSErr err;
  FSRef urlFS;
  FSCatalogInfo urlInfo;
  HFSUniStr255 outString;
  Local<String> errorDesc;

  if (CFURLGetFSRef(url, &urlFS) == false) {
    ThrowException(Exception::Error(String::New("Failed to convert URL to file or directory object"))->ToObject());
    return scope.Close(Undefined());
  }

  if ((err = FSGetCatalogInfo(&urlFS, kFSCatInfoVolume, &urlInfo, NULL, NULL, NULL)) != noErr) {
    ThrowException(Exception::Error(OSErrDescription(err))->ToObject());
    return scope.Close(Undefined());
  }

  if ((err = FSGetVolumeInfo(urlInfo.volume, 0, NULL, kFSVolInfoNone, NULL, &outString, NULL)) != noErr) {
    ThrowException(Exception::Error(OSErrDescription(err))->ToObject());
    return scope.Close(Undefined());
  }

  return scope.Close(String::New(outString.unicode, outString.length));
}
