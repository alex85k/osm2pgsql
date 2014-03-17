#ifndef GCCATTRIBUTE_H
#define GCCATTRIBUTE_H

#ifdef _MSC_VER
#define __attribute__(x)
#endif
#define UNUSED  __attribute__ ((unused))

#endif