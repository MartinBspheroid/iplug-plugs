#pragma once
//#include <common.hpp>
#include <complex>
#include <algorithm> // for std::min, max


/** Supplemental `<cmath>` functions and types
*/
namespace SA{
namespace math {

  static const float FREQ_C4 = 261.6256f;
  static const float FREQ_A4 = 440.0000f;
  static const float FREQ_SEMITONE = 1.0594630943592953f;
////////////////////
// basic integer functions
////////////////////

/** Returns true if `x` is odd. */
template <typename T>
bool isEven(T x) {
	return x % 2 == 0;
}

/** Returns true if `x` is odd. */
template <typename T>
bool isOdd(T x) {
	return x % 2 != 0;
}

/** Limits `x` between `a` and `b`.
If `b < a`, returns a.
*/
inline int clamp(int x, int a, int b) {
	return std::max(std::min(x, b), a);
}

/** Limits `x` between `a` and `b`.
If `b < a`, switches the two values.
*/
inline int clampSafe(int x, int a, int b) {
	return (a <= b) ? clamp(x, a, b) : clamp(x, b, a);
}

/** Euclidean modulus. Always returns `0 <= mod < b`.
`b` must be positive.
See https://en.wikipedia.org/wiki/Euclidean_division
*/
inline int eucMod(int a, int b) {
	int mod = a % b;
	if (mod < 0) {
		mod += b;
	}
	return mod;
}

/** Euclidean division.
`b` must be positive.
*/
inline int eucDiv(int a, int b) {
	int div = a / b;
	int mod = a % b;
	if (mod < 0) {
		div -= 1;
	}
	return div;
}

inline void eucDivMod(int a, int b, int* div, int* mod) {
	*div = a / b;
	*mod = a % b;
	if (*mod < 0) {
		*div -= 1;
		*mod += b;
	}
}

/** Returns `floor(log_2(n))`, or 0 if `n == 1`. */
inline int log2(int n) {
	int i = 0;
	while (n >>= 1) {
		i++;
	}
	return i;
}

/** Returns whether `n` is a power of 2. */
template <typename T>
bool isPow2(T n) {
	return n > 0 && (n & (n - 1)) == 0;
}

/** Returns 1 for positive numbers, -1 for negative numbers, and 0 for zero.
See https://en.wikipedia.org/wiki/Sign_function.
*/
template <typename T>
T sgn(T x) {
	return x > 0 ? 1 : (x < 0 ? -1 : 0);
}

////////////////////
// basic float functions
////////////////////

/** Limits `x` between `a` and `b`.
If `b < a`, returns a.
*/
inline float clamp(float x, float a, float b) {
	return std::fmax(std::fmin(x, b), a);
}

/** Limits `x` between `a` and `b`.
If `b < a`, switches the two values.
*/
inline float clampSafe(float x, float a, float b) {
	return (a <= b) ? clamp(x, a, b) : clamp(x, b, a);
}

/** Converts -0.f to 0.f. Leaves all other values unchanged. */
inline float normalizeZero(float x) {
	return x + 0.f;
}

/** Euclidean modulus. Always returns `0 <= mod < b`.
See https://en.wikipedia.org/wiki/Euclidean_division.
*/
inline float eucMod(float a, float b) {
	float mod = std::fmod(a, b);
	if (mod < 0.f) {
		mod += b;
	}
	return mod;
}

/** Returns whether `a` is within epsilon distance from `b`. */
inline bool isNear(float a, float b, float epsilon = 1e-6f) {
	return std::fabs(a - b) <= epsilon;
}

/** If the magnitude of `x` if less than epsilon, return 0. */
inline float chop(float x, float epsilon = 1e-6f) {
	return std::fabs(x) <= epsilon ? 0.f : x;
}

inline float rescale(float x, float xMin, float xMax, float yMin, float yMax) {
	return yMin + (x - xMin) / (xMax - xMin) * (yMax - yMin);
}

inline float crossfade(float a, float b, float p) {
	return a + (b - a) * p;
}

/** Linearly interpolates an array `p` with index `x`.
The array at `p` must be at least length `floor(x) + 2`.
*/
inline float interpolateLinear(const float* p, float x) {
	int xi = x;
	float xf = x - xi;
	return crossfade(p[xi], p[xi + 1], xf);
}

/** Complex multiplication `c = a * b`.
Arguments may be the same pointers.
Example:
	cmultf(ar, ai, br, bi, &ar, &ai);
*/
inline void complexMult(float ar, float ai, float br, float bi, float* cr, float* ci) {
	*cr = ar * br - ai * bi;
	*ci = ar * bi + ai * br;
}


/** Expands a Vec and Rect into a comma-separated list.
Useful for print debugging.
	printf("(%f %f) (%f %f %f %f)", VEC_ARGS(v), RECT_ARGS(r));
Or passing the values to a C function.
	nvgRect(vg, RECT_ARGS(r));
*/
} // namespace math
}