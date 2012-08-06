#ifdef WIN32
#include <winsock2.h>
#endif

#define GMOD_BETA

#include "include/mysql.h"

#include "utlvector.h"
#include "utlstack.h"
#include "vstdlib/jobthread.h"

#include "gmod/GMLuaModule.h"
#include "database.h"

#include "memdbgon.h"