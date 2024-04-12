/*
  libco
  auto-selection module
  license: public domain
*/

#define LIBCO_C

#if defined(__GNUC__) && defined(__i386__) || (defined(_MSC_VER) && defined(_M_IX86))
  #include "x86.c" 
#elif defined(__GNUC__) && defined(__amd64__) || (defined(_MSC_VER) && defined(_M_AMD64))
#if defined(SJLJ_BUG_WIN32)
  #include "fiber.c"
#else
 #include "amd64.c"
#endif
#elif defined(__GNUC__) && defined(_ARCH_PPC)
  #include "ppc.c"
#elif defined(__GNUC__) && (defined(__ARM_EABI__) || defined(__arm__))
  #include "armeabi.c"
#elif defined(__GNUC__)
  #include "sjlj.c"
#elif defined(_MSC_VER)
  #include "fiber.c"
#else
  #error "libco: unsupported processor, compiler or operating system"
#endif
