#ifndef PROBE_DLLPORT_H
#define PROBE_DLLPORT_H

// clang-format off
#ifdef _MSC_VER
#    ifdef PROBE_SHARED
#        ifdef PROBE_BUILDING
#            define PROBE_API __declspec(dllexport)
#        else
#            define PROBE_API __declspec(dllimport)
#        endif
#    endif
#elif defined(__GNUC__) || defined(__clang__)
#    if defined(PROBE_SHARED) || defined(PROBE_BUILDING)
#        define PROBE_API __attribute__((visibility("default")))
#    endif
#endif

#ifndef PROBE_API
#    define PROBE_API
#endif
// clang-format on

#endif //! PROBE_DLLPORT_H