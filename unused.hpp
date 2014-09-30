#ifndef UNUSED_HPP
#define UNUSED_HPP

#ifdef _MSC_VER
#define __attribute__(x)
#endif
#define UNUSED  __attribute__ ((unused))
#endif