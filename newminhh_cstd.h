#ifndef NEWMINHH_CSTD_H
#define NEWMINHH_CSTD_H

#ifndef __ASSEMBLER__

#if !defined(__cplusplus) && (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L)
#error "newminhh requires GNU C23 (-std=gnu23) with Clang >= 21 or GCC >= 15"
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(__has_include) && defined(__STDC_VERSION__) && (__STDC_VERSION__ > 202311L)
#  if __has_include(<stdcountof.h>)
#    include <stdcountof.h>
#    define NM_COUNTOF(arr) countof(arr)
#  endif
#endif

#ifndef NM_COUNTOF
#  if defined(__cplusplus)
template <typename T, size_t N>
constexpr size_t nm_countof_helper(T const (&)[N]) noexcept { return N; }
#    define NM_COUNTOF(arr) nm_countof_helper(arr)
#  else
#    define NM_COUNTOF(arr) \
        (sizeof(arr) / sizeof((arr)[0]) + 0 * sizeof(int[1 - 2 * __builtin_types_compatible_p(__typeof__(arr), __typeof__(&(arr)[0]))]))
#  endif
#endif

/* Clang nullability qualifiers & compiler extension pragmas */
#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wnullability-extension"
#  pragma clang diagnostic ignored "-Wgcc-compat"
#  define NM_NONNULL_PTR _Nonnull
#  define NM_NULLABLE_PTR _Nullable
#  define NM_ASSUME_NONNULL_BEGIN _Pragma("clang assume_nonnull begin")
#  define NM_ASSUME_NONNULL_END   _Pragma("clang assume_nonnull end")
#else
#  define NM_NONNULL_PTR
#  define NM_NULLABLE_PTR
#  define NM_ASSUME_NONNULL_BEGIN
#  define NM_ASSUME_NONNULL_END
#endif

/* Clang parameter & compile-time diagnostics */
#if defined(__has_c_attribute)
#  if __has_c_attribute(clang::noescape)
#    define NM_NOESCAPE [[clang::noescape]]
#  endif
#endif
#ifndef NM_NOESCAPE
#  define NM_NOESCAPE
#endif

#if defined(__has_attribute)
#  if __has_attribute(diagnose_if)
#    define NM_DIAGNOSE_IF(cond, msg, kind) __attribute__((diagnose_if(cond, msg, kind)))
#  endif
#endif
#ifndef NM_DIAGNOSE_IF
#  define NM_DIAGNOSE_IF(cond, msg, kind)
#endif

/* GCC C23 pure attributes (Clang 23 returns 0 for __has_c_attribute(reproducible)) */
#if defined(__has_c_attribute)
#  if __has_c_attribute(reproducible)
#    define NM_REPRODUCIBLE [[reproducible]]
#  endif
#  if __has_c_attribute(unsequenced)
#    define NM_UNSEQUENCED [[unsequenced]]
#  endif
#endif
#ifndef NM_REPRODUCIBLE
#  define NM_REPRODUCIBLE
#endif
#ifndef NM_UNSEQUENCED
#  define NM_UNSEQUENCED
#endif

/* Cross-compiler attribute spelling normalization */
#if defined(__has_attribute)
#  if __has_attribute(counted_by)
#    define NM_COUNTED_BY(member) __attribute__((counted_by(member)))
#  endif
#endif
#ifndef NM_COUNTED_BY
#  define NM_COUNTED_BY(member)
#endif

#define NM_PRINTF_FMT(fmt_i, arg_i) [[gnu::format(printf, fmt_i, arg_i)]]
#define NM_CONSTRUCTOR              [[gnu::constructor]]
#define NM_PACKED                   __attribute__((packed))
#define NM_ALIGNED(n)               __attribute__((aligned(n)))

/* Pedantic-clean pointer conversions & branch hints */
#define NM_FN_TO_VOIDP(fn)            ((void *)(uintptr_t)(fn))
#define NM_VOIDP_TO_FN(fn_type, ptr)  ((fn_type)(uintptr_t)(ptr))
#define NM_LIKELY(x)                  __builtin_expect(!!(x), 1)
#define NM_UNLIKELY(x)                __builtin_expect(!!(x), 0)

/* Portable <stdbit.h> wrapper (C23); __NM_FORCE_NO_STDBIT forces the fallback */
#if !defined(__NM_FORCE_NO_STDBIT) && !defined(__cplusplus) && defined(__has_include) && __has_include(<stdbit.h>)
#  include <stdbit.h>
#  define NM_HAS_SINGLE_BIT(x) stdc_has_single_bit(x)
#  define NM_BIT_CEIL(x)       stdc_bit_ceil(x)
#else
#  define NM_HAS_SINGLE_BIT(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
#  define NM_BIT_CEIL(x)         ((x) <= 1 ? (__typeof__(x))1                   : (__typeof__(x))((__typeof__(x))2 <<                      ((sizeof(x) * 8u) - 1u - (unsigned)__builtin_clzg((__typeof__(x))((x) - 1)))))
#endif

/* memset_explicit: portable C23 secure zeroing across glibc and musl */
static inline void *NM_NONNULL_PTR nm_memset_explicit(void *NM_NONNULL_PTR p, int c, size_t n)
{
    volatile unsigned char *pv = (volatile unsigned char *)p;
    for (size_t i = 0; i < n; ++i) {
        pv[i] = (unsigned char)c;
    }
    __asm__ volatile("" : : "r"(p) : "memory");
    return p;
}

/* Current thread id via the Linux gettid syscall (hosted Linux builds only) */
#if defined(__linux__) && defined(__STDC_HOSTED__) && (__STDC_HOSTED__ != 0)
#  include <sys/syscall.h>
#  include <unistd.h>
static inline int nm_gettid(void)
{
    return (int)syscall(SYS_gettid);
}
#endif

#endif /* !__ASSEMBLER__ */

#endif /* NEWMINHH_CSTD_H */
