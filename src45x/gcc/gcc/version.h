#ifndef GCC_VERSION_H
#define GCC_VERSION_H
#ifdef _BUILD_C30_
extern char *version_string;
#else
extern const char version_string[];
#endif

extern const char pkgversion_string[];
extern const char bug_report_url[];
#endif /* ! GCC_VERSION_H */
