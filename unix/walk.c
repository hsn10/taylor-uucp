/* walk.c
   Walk a directory tree.  */

#include "uucp.h"
#include "sysdep.h"
#include "system.h"

#if HAVE_FTW_H
#include <ftw.h>
#endif

static int iswalk_dir P((const char *zname, struct stat *qstat,
			 int iflag));

/* Walk a directory tree.  */

static size_t cSlen;
static void (*puSfn) P((const char *zfull, const char *zrelative,
			pointer pinfo));
static pointer pSinfo;

boolean
usysdep_walk_tree (zdir, pufn, pinfo)
     const char *zdir;
     void (*pufn) P((const char *zfull, const char *zrelative,
		     pointer pinfo));
     pointer pinfo;
{
  cSlen = strlen (zdir) + 1;
  puSfn = pufn;
  pSinfo = pinfo;
  return ftw (zdir, iswalk_dir, 5) == 0;
}

/* Pass a file found in the directory tree to the system independent
   function.  */

static int
iswalk_dir (zname, qstat, iflag)
     const char *zname;
     struct stat *qstat;
     int iflag;
{
  size_t clen;
  char *zcopy;

  if (iflag != FTW_F)
    return 0;

  clen = strlen (zname + cSlen);
  zcopy = (char *) alloca (clen + 1);
  memcpy (zcopy, zname + cSlen, clen + 1);

  (*puSfn) (zname, zcopy, pSinfo);

  return 0;
}