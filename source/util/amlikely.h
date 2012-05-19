#ifndef AMLIKELY_H
#define AMLIKELY_H

/// Macros to use as hints for branch prediction in if statements:
#ifdef __GNUC__
/// Tells the compiler that the expression \c x is likely to be true.  Provides hints for branch optimization.
#define AM_LIKELY(x)       __builtin_expect((x),1)
/// Tells the compiler that the expression \c x is not likely to be true.  Provies hints for branch optimization.
#define AM_UNLIKELY(x)     __builtin_expect((x),0)
#else
#define AM_LIKELY(x)       (x)
#define AM_UNLIKELY(x)     (x)
#endif

#endif // AMLIKELY_H
