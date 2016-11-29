#include <fcntl.h>

/** Returns 1 on success, or -1 if there was an error */
bool SetSocketBlockingEnabled(int fd, int blocking)
{
   if (fd < 0) return -1;

#ifdef WIN32
   unsigned long mode = blocking==1 ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? 1 : -1;
#else
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags < 0) return false;
   flags = blocking==1 ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0) ? 1 : -1;
#endif
}
