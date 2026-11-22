/*
 * Pre-include for cppcheck only (not used when compiling the library).
 * Cppcheck predefines __GNUC_PREREQ=0, which breaks glibc headers.
 */
#ifndef MISC_CPPCHECK_ENV_H
#define MISC_CPPCHECK_ENV_H

#ifndef __GNUC__
#define __GNUC__ 13
#define __GNUC_MINOR__ 2
#endif

#ifndef __GNUC_PREREQ
#define __GNUC_PREREQ(maj, min) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#endif

#endif /* MISC_CPPCHECK_ENV_H */
