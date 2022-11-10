////////////////////////////////////////////////////////////////////
// PSP Network Library Helpers
//  by PspPet

////////////////////////////////////////////////////////////////////
// Reverse engineering source material:
// "fair use" for making compatible software.
// most call sequences and initialization values taken from Wipeout Pure game
//   Wipeout has both ad-hoc game sharing and Apctl init for web access
//   also useful resources:
//     Twisted Metal game
//       (also uses Apctl, in a slightly different way)
//     KD modules "pspnet_apctl.prx" and "pspnet_ap_dialog_dummy.prx"
//       (especially for NetParam values)
//   Standard Berkeley socket socket.h header (or the MS winsock.h)
//
////////////////////////////////////////////////////////////////////

#include "std.h"
#include "nlh.h"
#include "util.h"

#include "loadutil.h" // Must be linked with KMEM memory access

////////////////////////////////////////////////////////////////////
// system entries

// general init
int sceNetInit(u32 r4, u32 r5, u32 r6, u32 r7, u32 r8);
int sceNetTerm();

// internet
int sceNetResolverInit();
int sceNetApctlInit(u32 r4, u32 r5);
int sceNetApctlTerm();
int sceNetResolverTerm();
// NOT USED IN THIS VERSION:
// int sceNetApctlAddHandler(void* handler_proc, u32 r5);
// int sceNetApctlDelHandler(int handler_id);

int sceNetInetInit();
int sceNetInetTerm();

////////////////////////////////////////////////////////////////////

// call from KERNEL thread
int nlhLoadDrivers(SceModuleInfo* modInfoPtr)
{
    // libraries load where they want, patch workaround needed
    LoadAndStartAndPatch(modInfoPtr, "flash0:/kd/ifhandle.prx"); // kernel
    LoadAndStartAndPatch(modInfoPtr, "flash0:/kd/pspnet.prx");
    LoadAndStartAndPatch(modInfoPtr, "flash0:/kd/pspnet_inet.prx");
    LoadAndStartAndPatch(modInfoPtr, "flash0:/kd/pspnet_apctl.prx");
    LoadAndStartAndPatch(modInfoPtr, "flash0:/kd/pspnet_resolver.prx");
    FlushCaches();
    //REVIEW: add error checks
    return 0;
}

// everything else call from USER thread

int nlhInit()
{
    u32 err;
    err = sceNetInit(0x20000, 0x20, 0x1000, 0x20, 0x1000);
    if (err != 0)
    {
        //my_printn("sceNetInit returns ", err, "\n");
        return err;
    }
    err = sceNetInetInit();
    if (err != 0)
    {
        //my_printn("sceNetInetInit returns ", err, "\n");
        return err;
    }

    err = sceNetResolverInit();
    if (err != 0)
    {
        //my_printn("sceNetResolverInit returns ", err, "\n");
        return err;
    }

    err = sceNetApctlInit(0x1000, 0x42);
    if (err != 0)
    {
        //my_printn("sceNetApctlInit returns ", err, "\n");
        return err;
    }

    return 0;   // it worked!
}

int nlhTerm()
{
    u32 err;

//REVIEW: we need to do something first to stop the connection
//REVIEW: -- sceNetApctlTerm returns 80410A04 on V1 firmware
    err = sceNetApctlTerm();
    if (err == 0x80410A04)
        printf("sceNetApctlTerm returned common error\n");
    else if (err != 0) {
        //my_printn("sceNetApctlTerm returns ", err, "\n");
    }

    err = sceNetResolverTerm();
    if (err != 0) {
        //my_printn("sceNetResolverTerm returns ", err, "\n");
    }

    err = sceNetInetTerm();
    if (err != 0) {
        //my_printn("sceNetInetTerm returns ", err, "\n");
    }
    err = sceNetTerm();
    if (err != 0) {
        //my_printn("sceNetTerm returns ", err, "\n");
    }

    return 0; // assume it worked
}

////////////////////////////////////////////////////////////////////

unsigned short htons(unsigned short wIn)
{
    u8 bHi = (wIn >> 8) & 0xFF;
    u8 bLo = wIn & 0xFF;
    return ((unsigned short)bLo << 8) | bHi;
}

int nlhRecvBlockTillDone(SOCKET s, u8* buf, int len)
{
    // 'sceNetInetRecv' may return partial results
    //  call repeatedly until entire receive buffer is full
    int nTotal = 0;
    while (len > 0)
    {
        int n = sceNetInetRecv(s, buf, len, 0); // blocking for anything
        if (n <= 0)
            return n; // error
        nTotal += n;
        buf += n;
        len -= n;
    }
    return nTotal;
}
//REVIEW: will wedge forever while waiting

int nlhSetSockNoBlock(SOCKET s, u32 val)
{ 
    return sceNetInetSetsockopt(s, SOL_SOCKET, 0x1009, (const char*)&val, sizeof(u32));
}



SOCKET nlhSimpleConnectWithTimeout(int sockType, const u8 ip_addr[4], u16 port, int timeoutTicks, STATUS_PROC statusProc, void* statusData)
{
    SOCKET sock;
    struct sockaddr_in addrTo;
    int err;

    sock = sceNetInetSocket(AF_INET, sockType, 0);
    if (sock & 0x80000000)
        return INVALID_SOCKET;

    addrTo.sin_family = AF_INET;
    addrTo.sin_port = htons(port);
    addrTo.sin_addr[0] = ip_addr[0];
    addrTo.sin_addr[1] = ip_addr[1];
    addrTo.sin_addr[2] = ip_addr[2];
    addrTo.sin_addr[3] = ip_addr[3];

    //with regular blocking settings, the PSP wedges waiting for a connection
        // if the IP is not valid
    // instead we start a non-blocking connect, and check the status waiting
        // for a connect

    nlhSetSockNoBlock(sock, 1);

    err = sceNetInetConnect(sock, &addrTo, sizeof(addrTo));
        // start the process

    if (err == 0)
    {
        // surprising - it worked immediately
        nlhSetSockNoBlock(sock, 0); // turn blocking back on for recv
        return sock;    // worked immediately
    }

    if (err == 0xFFFFFFFF && sceNetInetGetErrno() == 0x77)
    {
        // connect process started, not yet complete
#define TICKS_PER_POLL 20
        int ticks;
        for (ticks = 0; ticks < timeoutTicks; ticks += TICKS_PER_POLL)
        {
            // try connect again, which should always fail
            // look at why it failed to figure out if it is connected
            //REVIEW: a conceptually cleaner way to poll this?? (accept?)
            err = sceNetInetConnect(sock, &addrTo, sizeof(addrTo));
            if (err == 0 || (err == 0xFFFFFFFF && sceNetInetGetErrno() == 0x7F))
            {
                // now connected - I hope
                nlhSetSockNoBlock(sock, 0); // turn blocking back on for recv
                return sock;
            }
            if (statusProc != NULL)
                (*statusProc)(100 * ticks / timeoutTicks, statusData);
            sceKernelDelayThread(TICKS_PER_POLL*1000);
        }
    }

    // something wrong - general error or timeout on connect
    //REVIEW: sceNetInetSocketAbort(sock); ???
    sceNetInetClose(sock);

    return INVALID_SOCKET;
}
