#ifndef _GLOBAL_DEFINES_
#define _GLOBAL_DEFINES_

#define TESTING 1
#define VERSION "2.0.0"
#ifdef TESTING
#define VERSION_STRING VERSION "-testing"
#else
#define VERSION_STRING VERSION
#endif
#define FEATURE_FLOATSWITCH
//#define FEATURE_ULTRASONIC
//#define FEATURE_FLOATMETER

#endif
