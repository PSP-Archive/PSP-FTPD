# Additional Stubs for sceNet APIs (not yet in the PSPSDK)
# NOTE: require tricky user entry patching when loaded from kernel thread

      .set noreorder

    .include "common.s"

  STUB_START "sceNet",0x90000,0x00050005 
    STUB_FUNC 0x39af39a6,sceNetInit 
    STUB_FUNC 0x281928a9,sceNetTerm 
    STUB_FUNC 0xad6844c6,sceNetThreadAbort 
    STUB_FUNC 0x0bf0a3ae,sceNetGetLocalEtherAddr 
    STUB_FUNC 0xcc393e48,sceNetGetMallocStat 
  STUB_END 

  STUB_START "sceNetInet",0x90000,0x001c0005 
    STUB_FUNC 0x17943399,sceNetInetInit 
    STUB_FUNC 0xa9ed66b9,sceNetInetTerm 
    STUB_FUNC 0xdb094e1b,sceNetInetAccept 
    STUB_FUNC 0x1a33f9ae,sceNetInetBind 
    STUB_FUNC 0x8d7284ea,sceNetInetClose 
    STUB_FUNC 0x805502dd,sceNetInetCloseWithRST 
    STUB_FUNC 0x410b34aa,sceNetInetConnect 
    STUB_FUNC 0xe247b6d6,sceNetInetGetpeername 
    STUB_FUNC 0x162e6fd5,sceNetInetGetsockname 
    STUB_FUNC 0x4a114c7c,sceNetInetGetsockopt 
    STUB_FUNC 0xd10a1a7a,sceNetInetListen 
    STUB_FUNC 0xfaabb1dd,sceNetInetPoll 
    STUB_FUNC 0xcda85c99,sceNetInetRecv 
    STUB_FUNC 0xc91142e4,sceNetInetRecvfrom 
    STUB_FUNC 0xeece61d2,sceNetInetRecvmsg 
    STUB_FUNC 0x5be8d595,sceNetInetSelect 
    STUB_FUNC 0x7aa671bc,sceNetInetSend 
    STUB_FUNC 0x05038fc7,sceNetInetSendto 
    STUB_FUNC 0x774e36f4,sceNetInetSendmsg 
    STUB_FUNC 0x2fe71fe7,sceNetInetSetsockopt 
    STUB_FUNC 0x4cfe4e56,sceNetInetShutdown 
    STUB_FUNC 0x8b7b220f,sceNetInetSocket 
    STUB_FUNC 0x80a21abd,sceNetInetSocketAbort 
    STUB_FUNC 0xfbabe411,sceNetInetGetErrno 
    STUB_FUNC 0xb75d5b0a,sceNetInetInetAddr 
    STUB_FUNC 0x1bdf5d13,sceNetInetInetAton 
    STUB_FUNC 0xd0792666,sceNetInetInetNtop 
    STUB_FUNC 0xe30b8c19,sceNetInetInetPton 
  STUB_END 

  STUB_START "sceNetResolver",0x90000,0x00070005 
    STUB_FUNC 0xf3370e61,sceNetResolverInit  
    STUB_FUNC 0x6138194a,sceNetResolverTerm  
    STUB_FUNC 0x244172af,sceNetResolverCreate  
    STUB_FUNC 0x94523e09,sceNetResolverDelete  
    STUB_FUNC 0x224c5f44,sceNetResolverStartNtoA  
    STUB_FUNC 0x629e2fb7,sceNetResolverStartAtoN  
    STUB_FUNC 0x808f6063,sceNetResolverStop 
  STUB_END 

  STUB_START "sceNetApctl",0x90000,0x00080005 
    STUB_FUNC 0xe2f91f9b,sceNetApctlInit  
    STUB_FUNC 0xb3edd0ec,sceNetApctlTerm  
    STUB_FUNC 0x2befdf23,sceNetApctlGetInfo  
    STUB_FUNC 0x8abadd51,sceNetApctlAddHandler  
    STUB_FUNC 0x5963991b,sceNetApctlDelHandler  
    STUB_FUNC 0xcfb957c6,sceNetApctlConnect  
    STUB_FUNC 0x24fe91a1,sceNetApctlDisconnect  
    STUB_FUNC 0x5deac81b,sceNetApctlGetState 
  STUB_END 

