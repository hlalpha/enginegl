#if !defined( PORT_H )
#define PORT_H

// up / down
#define	PITCH	0

// left / right
#define	YAW		1

// fall over
#define	ROLL	2


#define	MAX_QPATH		64			// max length of a quake game pathname
#define	MAX_OSPATH		128			// max length of a filesystem pathname

#define	ON_EPSILON		0.1			// point on plane side epsilon

#define	MAX_MSGLEN		8000		// max length of a reliable message
#define	MAX_DATAGRAM	1024		// max length of unreliable message

//
// per-level limits
//
#define	MAX_EDICTS		600			// FIXME: ouch! ouch! ouch!
#define	MAX_LIGHTSTYLES	64
#define	MAX_MODELS		256			// these are sent over the net as bytes
#define	MAX_SOUNDS		256			// so they cannot be blindly increased

#if !defined BYTE_DEFINED
typedef unsigned char 		byte;
#define BYTE_DEFINED 1
#endif

#if __cplusplus
#define FALSE 0
#define TRUE (!FALSE)

typedef int qboolean;
#else
#undef true
#undef false

typedef enum {false, true}	qboolean;
#endif

#if __cplusplus
#include "../engine/common.h"
#endif
#include "../engine/mathlib.h"

#endif // PORT_H

