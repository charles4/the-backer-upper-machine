#ifndef _WIN32_H_
#define _WIN32_H_

#ifdef _WIN32
/* windows doesn't define these in sys/types.h */
typedef unsigned char u_int8_t;
typedef unsigned short int u_int16_t;
typedef unsigned int u_int32_t;
#endif

#endif /* _WIN32_H_ */
