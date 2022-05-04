#ifndef KSImage_defs_hpp
#define KSImage_defs_hpp

#ifdef _WIN32
#ifdef KSImage_BUILD_DLL_EXPORT
#define KSImage_API __declspec(dllexport)
#elif (defined KSImage_DLL)
#define KSImage_API __declspec(dllimport)
#else
#define KSImage_API
#endif // KSImage_BUILD_DLL_EXPORT
#else
#define KSImage_API
#endif // _WIN32

#endif // !KSImage_defs_hpp