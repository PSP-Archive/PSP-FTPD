// Net Lib Helper - work in progress
# ifndef _NLH_H_
# define _NLH_H_

#include "my_socket.h"

// major functions - Access Point / NetInet
int nlhLoadDrivers(SceModuleInfo* modInfoPtr);
int nlhInit();
int nlhTerm();

// helpers
int nlhRecvBlockTillDone(SOCKET s, u8* buf, int len);
int nlhSetSockNoBlock(SOCKET s, u32 val);

typedef void (*STATUS_PROC)(int percent, void* statusData);
void nlhDefaultStatusProc(int percent, void* statusString);

SOCKET nlhSimpleConnectWithTimeout(int sockType, const u8 ip_addr[4], u16 port, int timeoutTicks, STATUS_PROC statusProc, void* statusData);

///////////////////////////////////////////////
// "Infrastructure" AP Connect
//  NOTE: will work on WiFi adhoc networks too (with manual settings)

// "Infrastructure" net configurations
//   iConfig = 0 is special (last used or last added?)
//   iConfig = 1 = first in connection list
int sceUtilityCheckNetParam(int iConfig);
int sceUtilityGetNetParam(int iConfig, u32 r5_gettype, void* r6return);
    // types used in order (pspnet_apctl): 0, 1, 2, 3, 4, 8, 9, 10, 13,
        //  14, 15, 11, 12, 5, 6, 7
    // interesting ones: 0 = name, 1 = ssid, 4,5 = ipaddr

// routines you will use to make connection
//  (other sceNetApctl functions in 'nlh.c' private implementation)
int sceNetApctlConnect(int connect_index); // 0 is special
int sceNetApctlGetState(int* state_ptr);
int sceNetApctlGetInfo(int code, void* data);
int sceNetApctlDisconnect();

///////////////////////////////////////////////
# endif
