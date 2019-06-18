
#define NAPI_VERSION 3
#include <napi.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFString.h>

const char* OSErrDescription(OSErr err) {
  switch (err) {
    case nsvErr: return "Volume not found";
    case ioErr: return "I/O error.";
    case bdNamErr: return "Bad filename or volume name.";
    case mFulErr: return "Memory full (open) or file won't fit (load)";
    case tmfoErr: return "Too many files open.";
    case fnfErr: return "File or directory not found; incomplete pathname.";
    case volOffLinErr: return "Volume is offline.";
    case nsDrvErr: return "No such drive.";
    case dirNFErr: return "Directory not found or incomplete pathname.";
    case tmwdoErr: return "Too many working directories open.";
  }

  return "Could not get volume name";
}

Napi::Value MethodGetVolumeName(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  std::string aPath = info[0].As<Napi::String>();

  CFStringRef volumePath = CFStringCreateWithCString(NULL, aPath.c_str(), kCFStringEncodingUTF8);
  CFURLRef url = CFURLCreateWithFileSystemPath(NULL, volumePath, kCFURLPOSIXPathStyle, true);

  OSErr err;
  FSRef urlFS;
  FSCatalogInfo urlInfo;
  HFSUniStr255 outString;

  if (CFURLGetFSRef(url, &urlFS) == false) {
    throw Napi::Error::New(env, "Failed to convert URL to file or directory object");
  }

  if ((err = FSGetCatalogInfo(&urlFS, kFSCatInfoVolume, &urlInfo, NULL, NULL, NULL)) != noErr) {
    throw Napi::Error::New(env, OSErrDescription(err));
  }

  if ((err = FSGetVolumeInfo(urlInfo.volume, 0, NULL, kFSVolInfoNone, NULL, &outString, NULL)) != noErr) {
    throw Napi::Error::New(env, OSErrDescription(err));
  }

  return Napi::String::New(env, reinterpret_cast<char16_t *>(outString.unicode), outString.length);
}
