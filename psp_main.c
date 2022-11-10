/* PSPFTPD */

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspthreadman.h>

#include <pspwlan.h>
#include <pspkernel.h>
#include <psppower.h>


# ifndef PSPFW30X
PSP_MODULE_INFO("PSPFTPD", 0x1000, 1, 1);
# else
PSP_MODULE_INFO("PSPFTPD", 0x0, 1, 1);
PSP_HEAP_SIZE_KB(12*1024);
# endif
PSP_MAIN_THREAD_ATTR(0);
PSP_MAIN_THREAD_STACK_SIZE_KB(64);

#include "std.h"
#include "util.h"
#include "ftp.h"
#include "ftpd.h"
#include "psp_init.h"
#include "psp_pg.h"
#include "psp_cfg.h"

#include <pspwlan.h>
#include <pspkernel.h>
#include <psppower.h>

static int loc_info_thread_end = 0;

# define STATUS_MESSAGE_LENGTH    55
# define STATUS_MESSAGE_LINE      20

static char status_messages[STATUS_MESSAGE_LINE][STATUS_MESSAGE_LENGTH];
static int  status_index = 0;

typedef struct sDevCtl
{
  s32 max_clusters;
  s32 free_clusters;
  s32 max_sectors;  // ???
  s32 sector_size;
  s32 sector_count;
} sDevCtl;

typedef struct sDevCommand
{
  sDevCtl * p_dev_inf;
} sDevCommand;


long
psp_get_memory_stick_free()
{
  sDevCtl     dev_ctl;
  sDevCommand command;

  command.p_dev_inf = &dev_ctl;

  if ( sceIoDevctl( "ms0:", 0x02425818, &command, sizeof( sDevCommand ), NULL, 0 ) == SCE_KERNEL_ERROR_OK ) {
    return dev_ctl.free_clusters * dev_ctl.sector_count * dev_ctl.sector_size;
  }
  return -1;
}

static char loc_batt_str[128];

char *
psp_get_battery_string()
{
  char tmp[128];
  int ret;

  strcpy(loc_batt_str, "none");
  if (scePowerIsBatteryExist()) {
    ret = scePowerGetBatteryLifePercent();

    if (ret >= 0) {
      sprintf(tmp, "%d", ret);
      strcpy(loc_batt_str,tmp);
      strcat(loc_batt_str,"%");
      if(!scePowerIsPowerOnline()){
        if((ret=scePowerGetBatteryLifeTime()) >= 0){
          sprintf(tmp, " %dh", ret/60);
          strcat(loc_batt_str,tmp);
          sprintf(tmp, "%d", (ret%60) + 100);
          strcat(loc_batt_str,tmp+1);
        }
      }
    }
  }
  return loc_batt_str;
}

int
psp_is_low_battery()
{
  int ret = 0;
  if (scePowerIsBatteryExist()) {
    ret = scePowerGetBatteryLifePercent();
    if (ret < 4) return 1;
  }
  return 0;
}

void
mftpDisplayBatteryMessage(void)
{
  char buffer[64];
  long space_free;
  int  color;

  snprintf(buffer, 50, " [%4s] ", psp_get_battery_string());

  if (psp_is_low_battery()) color = PG_TEXT_RED;
  else                      color = PG_TEXT_GREEN;

  pgFramePrint(25,29, buffer, color);

  space_free  = psp_get_memory_stick_free();
  space_free /= 1024 * 1024;

  if (space_free < 1) color = PG_TEXT_RED;
  else                color = PG_TEXT_YELLOW;
  snprintf(buffer, 20, "%ld MB free", space_free );

  /* Fill with space */
  int length = strlen(buffer);
  while (length < 20) buffer[length++] = ' ';
  buffer[length] = 0;
  
  pgFramePrint(0,29, buffer, color);
}

void
mftpDisplayMemoryMessage(void)
{
  char buffer[64];
  int color;

  pgFramePrint(25,29, buffer, color);
}

void
mftpAddNewStatusMessage(char *Message)
{
  int length = strlen(Message);
  int index  = 0;

  strncpy(status_messages[status_index], Message, STATUS_MESSAGE_LENGTH);

  status_messages[status_index][STATUS_MESSAGE_LENGTH-1] = 0;

  for (index = length; index < (STATUS_MESSAGE_LENGTH-1); index++) {
    status_messages[status_index][index] = ' ';
  }

  if ((status_index + 1) == STATUS_MESSAGE_LINE) status_index = 0;
  else status_index++;
}


void
mftpDisplayStatusMessage()
{
  int index;

  for (index = 0; index < STATUS_MESSAGE_LINE; index++) {
    pgFramePrint(1, 5 + index, 
    status_messages[(status_index + index) % STATUS_MESSAGE_LINE],
    PG_TEXT_WHITE);
  }
}


void
info_thread(SceSize args, void *argp)
{
  while (!loc_info_thread_end) {

    mftpDisplayStatusMessage();
    mftpDisplayBatteryMessage();

    sceKernelDelayThread(100000);
  }
}


static void 
DoInetNetFtpd(void)
{
  int info_thid;
  int last_state;
  u32 err;
  char szMyIPAddr[32];
  int connectionConfig = -1;
  PICKER pickConn; // connection picker
  int iNetIndex;
  int iPick;

  err = pspSdkInetInit();

  if (err != 0) {
    pspDebugScreenPrintf("pspSdkInetInit: %d !\n", err);
    pspSdkInetTerm();
    return;
  }

  pgInit();

  info_thid = sceKernelCreateThread( "info_thread", 
     (SceKernelThreadEntry)info_thread, 0x18, 0x10000, 0, 0 );

  if(info_thid >= 0) {
    sceKernelStartThread(info_thid, 0, 0);
  }

  while (sceWlanGetSwitchState() != 1)  //switch = off
  {
    pgFramePrint(0,0, "Turn the wifi switch on!", PG_TEXT_COLOR);
    sceKernelDelayThread(1000*1000);
  }

  // enumerate connections
  my_initpicker(&pickConn, "Choose a connection:                      X OK    [] CANCEL");

  for (iNetIndex = 1; iNetIndex < 100; iNetIndex++) // skip the 0th connection
  {
    char data[128];
    char name[128];
    char detail[128];
    if (sceUtilityCheckNetParam(iNetIndex) != 0) continue;
    sceUtilityGetNetParam(iNetIndex, 0, name);

    sceUtilityGetNetParam(iNetIndex, 1, data);
    strcpy(detail, "SSID=");
    strcat(detail, data);

    sceUtilityGetNetParam(iNetIndex, 4, data);
    if (data[0]) {
      // not DHCP
      sceUtilityGetNetParam(iNetIndex, 5, data);
      strcat(detail, " IPADDR=");
      strcat(detail, data);
    } else {
      strcat(detail, " DHCP");
    }

    my_addpick(&pickConn, name, detail, (u32)iNetIndex);
    if (pickConn.pick_count >= MAX_PICK) break;  // no more
  }

  if (pickConn.pick_count == 0) {
    pgFramePrint(0,0, "No wifi connections, please create one !", PG_TEXT_COLOR);
    goto close_net;
  }

  pickConn.pick_start = 0;
  iPick = my_picker(&pickConn);
  if (iPick == -1) goto close_net;
  connectionConfig = (int)(pickConn.picks[iPick].userData);

connect_wifi:

  err = sceNetApctlConnect(connectionConfig);
  if (err != 0) {
    goto close_net;
  }

  {
    char buffer[128];
    pgClear();
    strcpy(buffer, "Trying to connect to ");
    strcat(buffer, pickConn.picks[iPick].szBig);
    pgFramePrint(0,0, buffer, PG_TEXT_COLOR);
  }

  // 4 connected with IP address
  
  last_state = -1;

  while (1) 
  {
    int state = 0;
    char buffer[128];

    sceKernelDelayThread(50*1000);
    err = sceNetApctlGetState(&state);

    if (err != 0)
    {
      pgFramePrint(0,1, "Connection failed !", PG_TEXT_COLOR);
      goto close_connection;
    }

    {
      SceCtrlData c;
      sceCtrlReadBufferPositive(&c, 1);
      if (c.Buttons & PSP_CTRL_SQUARE) goto close_connection;
    }

    if (state != last_state) 
    {
      if (last_state == 2 && state == 0)
      {
        pgFramePrint(0,2, "Connecting to wifi Failed, Retrying...", PG_TEXT_COLOR);
        sceKernelDelayThread(500*1000); // 500ms
        goto connect_wifi;
      }
     
      sprintf(buffer, "Connection state %d of 4", state);
      pgFramePrint(0,1, buffer, PG_TEXT_COLOR);
      last_state = state;
    }

    // 0 - idle
    // 1,2 - starting up
    // 3 - waiting for dynamic IP
    // 4 - got IP - usable
    if (state == 4) break;  // connected with static IP
  }

  // get IP address
  if (sceNetApctlGetInfo(8, szMyIPAddr) != 0) {
    strcpy(szMyIPAddr, "unknown IP address");
  }

  ftpdLoop(szMyIPAddr);

close_connection:
  err = sceNetApctlDisconnect();

close_net:
  pgClear();
  pgFramePrint(0,0, "Closing connection ...", PG_TEXT_COLOR);
  
  pspSdkInetTerm();
  sceKernelDelayThread(1000000); 

  sceKernelExitDeleteThread(info_thid);
  sceKernelWaitThreadEnd(info_thid, NULL);
}

void
user_thread(SceSize args, void *argp)
{
  psp_setup_callbacks();

  DoInetNetFtpd();
}

int 
main(int argc, char **argv)
{
  int user_thid;

  pspDebugScreenInit();

  psp_init_stuff(argc, argv);

# ifdef PSPFW30X
  sceUtilityLoadNetModule(1); /* common */
  sceUtilityLoadNetModule(3); /* inet */
# else
  if (pspSdkLoadInetModules() != 0) {
    pspDebugScreenPrintf("unable to load wifi drivers !");
    return 0;
  }
# endif

  psp_read_config();

  user_thid = sceKernelCreateThread( "user_thread", 
     (SceKernelThreadEntry)user_thread, 0x16, 256*1024, PSP_THREAD_ATTR_USER, 0 );
  if(user_thid >= 0) {
    sceKernelStartThread(user_thid, 0, 0);
    sceKernelWaitThreadEnd(user_thid, NULL);
  }

  psp_exit(0);

  return 0;
}

