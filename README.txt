
                      Welcome to PSP-FTPD


Original Authors of PSP-FTPD
  PSPKrazy and Raf.

Author of this new version :
  Ludovic.Jacomme (ludovic.jacomme@gmail.com) also known as Zx-81

Old Design by Cortès the Killer
New Design using Raven's stuff


1. INTRODUCTION
   ------------

  PSP-FTPD is a simple FTP server for the PSP.  It was originally written by
  PSPKrazy, and then improved by Raf (PSPDEV environnement compilation etc ...)

  It permits to send and retrieve files or complete directories to and from
  your PSP without any cables, using your Wireless connection (WIFI), with a FTP
  client on your PC such as IExplorer or Filezilla (http://filezilla.sourceforge.net/).

  It has been tested using gFtp on Linux, Filezilla and IExplorer on Windows.

2. CHANGE-LOG
   ------------

Version 0.5.0

- Improve speed transfert 
  (tested at home with 500k for download, 250k for upload)
- Tested on PSP slim with fw4x-m33
- New graphics for EBOOT & background
- Add music for EBOOT
- Bug fix in battery status display

If you like Eboot music you may find the complete album here :
http://www.jamendo.com/en/album/22408

Version 0.4.0

- Compatible with fw3x-0E 

Version 0.3.8

  - Add battery status 
  - Add memory stick free space status
  - Display FTP commands received by clients

Version 0.3.7

  - Fix the bug identified by Danzel in all wifi stuff
  - UMD can now be read !

Version 0.3.6

  - Fix the well known issue of DHCP on 1.5FW

Version 0.3.5

  Here is the list of the new features and changes of this version :
  - Ask and wait for the user to set the wifi switch on 
  - Retry to connect to the wifi access point when an error
    occured on startup.
  - The user can stop the wifi access point connection using [] key

Version 0.3.4

  Here is the list of the new features and changes of this version :
  - Add background designed by Cortès the Killer
  - Diplay IP address of the last connected FTP Client

Version 0.3.3

  Here is the list of the new features and changes of this version :
  - You can now specify several user name, password and root directory
    in a configuration file (psp-ftpd.cfg) in order to host a FTP server
    with multi-user and multi root/home directories.

    See the psp-ftp.cfg file for the exact syntax.  

Version 0.3.2

  Here is the list of the new features and changes of this version :
  - Capability to specify in a configuration file (psp-ftpd.cfg), a
    root directory for the ftp server, to prevent ftp clients to access
    all the content of your memory stick. See the psp-ftp.cfg file for
    the syntax.  

  [WARNING: the psp-ftp.cfg file format has changed !]

  - Improve transfert rate: 500kbps from PSP->PC and 350kbps from PC->PSP
    using Filezilla. DON'T USE Internet Explorer if you expect high transfert
    rates, use rather Filezilla or other suitable FTP clients.

Version 0.3.1

  Here is the list of the new features and changes of this version :
  - Bug fix in rename command, RNFR handshake message was not strictly
    compliant with the RFC, and it wasn't understood by IExplorer.
  - Capability to specify in a configuration file (psp-ftpd.cfg), a
    couple user/password for FTP authentication. 
  - Add usage informations in the README file
  - Binary version for 1.5FW
  - Improve transfert rate (500kbps from PSP->PC, 200kbps from PC->PSP)
  
Version 0.3.0

  Here is the list of the new features and changes of this version :
  - A menu to select your WIFI Access point.
  - A really working multi-threaded version that can handle several 
    client at the same time, and exit properly.
  - Removing all user/password security check stuff (useless for file
    transfert between a PC and a PSP).
  - Capability to rename files (FTP commands RNFR and RNTO)
  - Capability to download or upload recursively a complete directory
  - Many bug fixes.
  - Working version for 2.5 and 2.6 FW.


3. INSTALLATION
   ------------

  Unzip the zip file, and copy the directory psp-ftpd on the psp/game 
  directory (as any others homebrew).

  It has been developped on Firmware 2.5, but it works also for any 2.x
  and 1.5 using the great GTA eloader (0.97) from Ditlew and Fanjita
  (see http://www.fanjita.org/ for details).

  If you want to use non-anonymous FTP transfert, you can edit the file
  psp-ftpd.cfg (with notepad for example) and specify a line without any
  blank characters :
 
  auth=user:password

  If you don't want to let the all content of your memory stick visible to
  FTP clients, you can specify a root directory for the FTP server. Edit
  the psp-ftpd.cfg file (using  notedpad for example) and specify a line 
  without any blank characters: 

  root=ms0:/psp/music
 
  That way, PSP-FTPD will let see only the content of the /psp/music directory
  (and its subdirectories) to FTP clients.

  If you want to use the multiple user / homedir feature, then you can 
  specify several couple lines with 'auth' and 'root' commands, as follows:

  auth=umd:zx
  root=disc0:

  auth=ms:zx
  root=ms0:

  On this example, using a FTP client with 'umd' as login and 'zx' as password
  you will be able to see the content of a UMD insterted in your PSP. 
  While using 'ms' as login, and 'zx' as password you will be able to 
  browse your memory stick.

4. USAGE
   ------------

   Run PSP-FTPD on your PSP, and choose your wifi access point using arrow and
   press X to validate. After few seconds, you should see the message 
   "FTP Server is now running on ftp://192.168.0.10/ (the IP of your connection)

   If you use anonymous connection (without any password in the psp-ftpd.cfg file)
   then you can access your PSP using Internet Explorer with the previous URL 
   (ftp://192.168.0.10/).

   If you use authentication, you have to specify user and password in the
   URL, for example : ftp://zx:zx@192.168.0.10/

   When you have finished to transfert, rename, remove etc ... files on your PSP,
   you can press the [] key to close the connection and go back to the eloader.

   It may happens, that PSP-FTPD failed to connect to your wifi access point,
   particulary when you have a 1.5 FW. To overcome this problem you can
   use a static IP (instead of DHCP) in the network configuration of your PSP.

  
5. COMPILATION
   ------------

  It has been developped/compiled under Linux using gcc with PSPSDK. 
  To rebuild the homebrew run the Makefile in the src archive.

  For any comments or questions on this version, please visit 
  http://zx81.zx81.free.fr, http://www.dcemu.co.uk, 
  http://forums.qj.net or http://www.psp-generation.com (french site).

