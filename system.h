/* system.h
   Header file for system dependent stuff in the Taylor UUCP package.
   This file is not itself system dependent.

   Copyright (C) 1991, 1992 Ian Lance Taylor

   This file is part of the Taylor UUCP package.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   The author of the program may be contacted at ian@airs.com or
   c/o AIRS, P.O. Box 520, Waltham, MA 02254.  */

#ifndef SYSTEM_H

#define SYSTEM_H

/* Any function which returns an error should also report an error
   message.

   Many of the function may share a common static buffer; this is
   noted in the description of the function.  */

/* The maximum length of a remote system name.  */
extern int cSysdep_max_name_len;

/* Initialize.  If something goes wrong, this routine should just
   exit.  The argument is 0, or a combination of any of the following
   flags.  */

/* This is a daemon program.  This is TRUE when called from uucico or
   uuxqt.  */
#define INIT_DAEMON (01)

/* This program needs to know the current working directory.  This is
   used because on Unix it can be expensive to determine the current
   working directory (some versions of getcwd fork a process), but in
   most cases we don't need to know it.  However, we are going to
   chdir to the spool directory (unless INIT_CHDIR is set), so we have
   to get the cwd now if we are ever going to get it.  Both uucp and
   uux use the function fsysdep_needs_cwd to determine whether they
   will need the current working directory, and pass the argument to
   usysdep_initialize appropriately.  There's probably a cleaner way
   to handle this, but this will suffice for now.  */
#define INIT_GETCWD (02)

/* This program should not chdir to the spool directory.  This may
   only make sense on Unix.  It is set by cu.  */
#define INIT_NOCHDIR (04)

extern void usysdep_initialize P((int iflags));

/* Exit the program.  The fsuccess argument indicates whether to
   return an indication of success or failure to the outer
   environment.  This routine should not return.  */
extern void usysdep_exit P((boolean fsuccess));

/* Called when a non-standard configuration file is being used, to
   avoid handing out privileged access.  If it returns FALSE, the
   default configuration file will be used.  This is called before
   the usysdep_initialize function is called.  */
extern boolean fsysdep_other_config P((const char *));

/* Detach from the controlling terminal.  This probably only makes
   sense on Unix.  It is called by uucico to try to get the modem port
   as a controlling terminal.  It is also called by uucico before it
   starts up uuxqt, so that uuxqt will be a complete daemon.  */
extern void usysdep_detach P((void));

/* Get the local node name if it is not specified in the configuration
   file.  This is called before the usysdep_initialize function is
   called.  It should return NULL on error.  The return value should
   point to a static buffer.  */
extern const char *zsysdep_local_name P((void));

/* Get the login name.  This is used when uucico is started up with no
   arguments in slave mode, which causes it to assume that somebody
   has logged in.  It also used by uucp and uux for recording the user
   name.  This may not return NULL.  The return value should point to
   a static buffer.  */
extern const char *zsysdep_login_name P((void));

/* Set a signal handler for a signal.  If the signal occurs, the
   appropriate element of afSignal should be set to the signal number
   (see the declaration of afSignal in uucp.h).  This routine might be
   able to just use signal, but 4.3 BSD requires more complex
   handling.  This is called before usysdep_initialize.  */
extern void usysdep_signal P((int isig));

/* Catch a signal.  This is actually defined as a macro in the system
   dependent header file, and the prototype here just indicates how it
   should be called.  It is called before a routine which must exit if
   a signal occurs, and is expected to set do a setjmp (which is why
   it must be a macro).  It is actually only called in one place in
   the system independent code, before the call to read stdin in uux.
   This is needed to handle 4.2 BSD restartable system calls, which
   require a longjmp.  On systems which don't need to do
   setjmp/longjmp around system calls, this can be redefined in
   <sysdep.h> to TRUE.  It should return TRUE if the routine should
   proceed, or FALSE if a signal occurred.  After having this return
   TRUE, usysdep_start_catch should be used to start catching the
   signal; this basically tells the signal handler that it's OK to do
   the longjmp, if fsysdep_catch did not already do so.  */
extern boolean fsysdep_catch P((void));

/* Start catching a signal.  This is called after fsysdep_catch to
   tell the signal handler to go ahead and do the longjmp.  */
extern void usysdep_start_catch P((void));

/* Stop catching a signal.  This is called when it is no longer
   necessary for fsysdep_catch to handle signals.  */
extern void usysdep_end_catch P((void));

/* Link two files.  On Unix this should attempt the link.  If it
   succeeds it should return TRUE with *pfworked set to TRUE.  If the
   link fails because it must go across a device, it should return
   TRUE with *pfworked set to FALSE.  If the link fails for some other
   reason, it should print an error message and return FALSE.  On a
   system which does not support links to files, this should just
   return TRUE with *pfworked set to FALSE.  */
extern boolean fsysdep_link P((const char *zfrom, const char *zto,
			       boolean *pfworked));

/* Get the port name.  This is used when uucico is started up in slave
   mode to figure out which port was used to call in so that it can
   determine any appropriate protocol parameters.  This may return
   NULL if the port cannot be determined, which will just mean that no
   protocol parameters are applied.  The name returned should be the
   sort of name that would appear in the port file.  This should set
   *pftcp_port to TRUE if it can determine that the port is a TCP
   connection rather than a normal serial port.  */
extern const char *zsysdep_port_name P((boolean *pftcp_port));

/* Make a spool directory for a system.  This will be called each time
   the system might be accessed.  It should return FALSE on error.  */
extern boolean fsysdep_make_spool_dir P((const struct ssysteminfo *qsys));

/* Return whether a file name is in a directory, and check for read or
   write access.  This should check whether zfile is within zdir (or
   is zdir itself).  If it is not, it should return FALSE.  If zfile
   is in zdir, then fcheck indicates whether further checking should
   be done.  If fcheck is FALSE, no further checking is done.
   Otherwise, if freadable is TRUE the user zuser should have search
   access to all directories from zdir down to zfile and should have
   read access on zfile itself (if zfile does not exist, or is not a
   regular file, this function may return FALSE but does not have to).
   If freadable is FALSE, the user zuser should have search access to
   all directories from zdir down to zfile and should have write
   access on zfile (which may be a directory, or may not actually
   exist, which is acceptable).  The zuser argument may be NULL, in
   which case the check should be made for any user, not just zuser.
   There is no way for this function to return error.  The qsys
   argument should be used to expand ~ into the public directory.  */
extern boolean fsysdep_in_directory P((const struct ssysteminfo *qsys,
				       const char *zfile,
				       const char *zdir,
				       boolean fcheck,
				       boolean freadable,
				       const char *zuser));

/* Return TRUE if a file exists, FALSE otherwise.  There is no way to
   return error.  */
extern boolean fsysdep_file_exists P((const char *zfile));

/* Exit the current program and start a new one.  If the ffork
   argument is TRUE, the new program should be started up and the
   current program should continue (but in all current cases, it will
   immediately exit anyhow); if the ffork argument is FALSE, the new
   program should replace the current program.  The three string
   arguments may be catenated together to form the program to execute;
   I did it this way to make it easy to call execl(2), and because I
   never needed more than two arguments.  The program will always be
   "uucico" or "uuxqt".  The return value will be passed directly to
   usysdep_exit, and should be TRUE on success, FALSE on error.  */
extern boolean fsysdep_run P((boolean ffork, const char *zprogram,
			      const char *zarg1, const char *zarg2));

/* Send a mail message.  This function will be passed an array of
   strings.  All necessary newlines are already included; the strings
   should simply be concatenated together to form the mail message.
   It should return FALSE on error, although the return value is often
   ignored.  */
extern boolean fsysdep_mail P((const char *zto, const char *zsubject,
			       int cstrs, const char **paz));

/* Get the time in seconds since some epoch.  The actual epoch is
   unimportant, so long as the time values are consistent across calls
   to the program, and the value is never negative.  If the pimicros
   argument is not NULL, it should be set to the number of
   microseconds (if this is not available, *pimicros should be set to
   zero).  */
 extern long isysdep_time P((long *pimicros));

/* Get the time in seconds and microseconds (millionths of a second)
   since some epoch.  The actual epoch is not important, and it may
   change in between program invocations; this is provided because on
   Unix the times function may be used.  If microseconds can not be
   determined, *pimicros can just be set to zero.  */
extern long isysdep_process_time P((long *pimicros));

/* Parse the value returned by isysdep_time into a struct tm.  I
   assume that this structure is defined in <time.h>.  This is
   basically just localtime, except that the ANSI function takes a
   time_t which may not be what is returned by isysdep_time.  The
   typedef is a hack to avoid the problem of mentioning a structure
   for the first time in a prototype while remaining compatible with
   standard C.  The type tm_ptr is never again referred to.  */
typedef struct tm *tm_ptr;
extern void usysdep_localtime P((long itime, tm_ptr q));

/* Sleep for a number of seconds.  */
extern void usysdep_sleep P((int cseconds));

/* Pause for half a second.  */
extern void usysdep_pause P((void));

/* Lock a remote system.  This should return FALSE if the system is
   already locked (no error should be reported).  */
extern boolean fsysdep_lock_system P((const struct ssysteminfo *qsys));

/* Unlock a remote system.  This should return FALSE on error
   (although the return value is generally ignored).  */
extern boolean fsysdep_unlock_system P((const struct ssysteminfo *qsys));

/* Get the conversation sequence number for a remote system, and
   increment it for next time.  This should return -1 on error.  */
extern long isysdep_get_sequence P((const struct ssysteminfo *qsys));

/* Get the status of a remote system.  This should return FALSE on
   error.  Otherwise it should set *qret to the status.  If no status
   information is available, this should set *qret to sensible values
   and return TRUE.  If pfnone is not NULL, then it should be set to
   TRUE if no status information was available or FALSE otherwise.  */
extern boolean fsysdep_get_status P((const struct ssysteminfo *qsys,
				     struct sstatus *qret,
				     boolean *pfnone));

/* Set the status of a remote system.  This should return FALSE on
   error.  The system will be locked before this call is made.  */
extern boolean fsysdep_set_status P((const struct ssysteminfo *qsys,
				     const struct sstatus *qset));

/* Check whether there is work for a remote system.  This should set
   *pbgrade to the highest grade of work waiting to execute.  It
   should return TRUE if there is work, FALSE otherwise; there is no
   way to indicate an error.  */
extern boolean fsysdep_has_work P((const struct ssysteminfo *qsys,
				   char *pbgrade));

/* Initialize the work scan.  This will be called before
   fsysdep_get_work.  The bgrade argument is the minimum grade of
   execution files that should be considered (e.g. a bgrade of 'd'
   will allow all grades from 'A' to 'Z' and 'a' to 'd').  The fcheck
   argument is TRUE if the work will not actually be done (i.e. this
   is being called by uustat).  This function should return FALSE on
   error.  */
extern boolean fsysdep_get_work_init P((const struct ssysteminfo *qsys,
					int bgrade, boolean fcheck));

/* Get the next command to be executed for a remote system.  The
   bgrade and fcheck arguments will be the same as for
   fsysdep_get_work_init; probably only one of these functions will
   use them, namely the function for which it is more convenient.
   This should return FALSE on error.  The structure pointed to by
   qcmd should be filled in.  The strings may point into a static
   buffer; they will be copied out if necessary.  If there is no more
   work, this should set qcmd->bcmd to 'H' and return TRUE.  This
   should set qcmd->pseq to something which can be passed to
   fsysdep_did_work to remove the job from the queue when it has been
   completed.  */
extern boolean fsysdep_get_work P((const struct ssysteminfo *qsys,
				   int bgrade, boolean fcheck,
				   struct scmd *qcmd));

/* Remove a job from the work queue.  This must also remove the
   temporary file used for a send command, if there is one.  It should
   return FALSE on error.  */
extern boolean fsysdep_did_work P((pointer pseq));

/* Save the temporary file for a send command.  This function should
   return a string that will be put into a mail message.  On success
   this string should say something like ``The file has been saved as
   ...''.  On failure it could say something like ``The file could not
   be saved because ...''.  If there is no temporary file, or for some
   reason it's not appropriate to include a message, this function
   should just return NULL.  This function is used when a file send
   fails for some reason, to make sure that we don't completely lost
   the file.  */
extern const char *zsysdep_save_temp_file P((pointer pseq));

/* Cleanup anything left over by fsysdep_get_work_init and
   fsysdep_get_work.  This may be called even though
   fsysdep_get_work_init has not been.  */
extern void usysdep_get_work_free P((const struct ssysteminfo *qsys));

/* Get the real file name for a file.  The file may or may not exist
   on the system.  If the zname argument is not NULL, then the zfile
   argument may be a directory; if it is, zname should be used for the
   name of the file within the directory.  The zname argument may not
   be simple (it may be in the format of another OS) so care should be
   taken with it.  On Unix, if the zfile argument begins with ~user/
   it goes in that users home directory, and if it begins with ~/
   (~uucp/) it goes in the public directory (note that each system may
   have its own public directory); similar conventions may be
   desirable on other systems.  The return value may point to a common
   static buffer.  This should return NULL on error.  */
extern const char *zsysdep_real_file_name P((const struct ssysteminfo *,
					     const char *zfile,
					     const char *zname));

/* Get a file name from the spool directory.  This should return
   NULL on error.  The return value may point to a common static
   buffer.  */
extern const char *zsysdep_spool_file_name P((const struct ssysteminfo *,
					      const char *zfile));

/* Make necessary directories.  This should create all non-existent
   directories for a file.  If the fpublic argument is TRUE, anybody
   should be permitted to create and remove files in the directory;
   otherwise anybody can list the directory, but only the UUCP system
   can create and remove files.  It should return FALSE on error.  */
extern boolean fsysdep_make_dirs P((const char *zfile, boolean fpublic));

/* Create a stdio file, setting appropriate protection.  If the
   fpublic argument is TRUE, the file is made publically accessible;
   otherwise it is treated as a private data file.  If the fappend
   argument is TRUE, the file is opened in append mode; otherwise any
   previously existing file of the same name is removed, and the file
   is kept private to the UUCP system.  If the fmkdirs argument is
   TRUE, then any necessary directories should also be created.  On a
   system in which file protections are unimportant (and the necessary
   directories exist), this may be implemented as

   fopen (zfile, fappend ? "a" : "w");

   */
extern FILE *esysdep_fopen P((const char *zfile, boolean fpublic,
			      boolean fappend, boolean fmkdirs));

/* Open a file to send to another system; the qsys argument is the
   system the file is being sent to.  If fcheck is TRUE, it should
   make sure that the file is readable by zuser (if zuser is NULL the
   file must be readable by anybody).  This is to eliminate a window
   between fsysdep_in_directory and esysdep_open_send.  The function
   should set *pimode to the mode that should be sent over (this
   should be a UNIX style file mode number).  It should set *pcbytes
   to the number of bytes contained in the file.  If an error occurs,
   it should return EFILECLOSED and, if pfgone is not NULL, it should
   *pfgone to TRUE if the file no longer exists or FALSE if there was
   some other error.  */
extern openfile_t esysdep_open_send P((const struct ssysteminfo *qsys,
				       const char *zname,
				       boolean fcheck,
				       const char *zuser,
				       unsigned int *pimode,
				       long *pcbytes,
				       boolean *pfgone));

/* Open a file to receive from another system.  Receiving a file is
   done in two steps.  First esysdep_open_receive is called.  This
   should open a temporary file and return the file name in *pztemp.
   It may ignore qsys (the system the file is coming from) and zto
   (the file to be created) although they are passed in case they are
   useful.  The file mode is not available at this point.  The *pztemp
   return value may point to a common static buffer.  The amount of
   free space should be returned in *pcbytes; ideally it should be the
   lesser of the amount of free space on the file system of the
   temporary file and the amount of free space on the file system of
   the final destination.  If the amount of free space is not
   available, *pcbytes should be set to -1.  The function should
   return EFILECLOSED on error.

   After the file is written, fsysdep_move_file will be called to move
   the file to its final destination, and to set the correct file
   mode.  */
extern openfile_t esysdep_open_receive P((const struct ssysteminfo *qsys,
					  const char *zto,
					  const char **pztemp,
					  long *pcbytes));

/* Move a file.  This is used to move a received file to its final
   location.  It is also used by uuxqt to move files in and out of the
   execute directory.  The zto argument is the file to create.  The
   zorig argument is the name of the file to move.  If fmkdirs is
   TRUE, then any necessary directories are created; fpublic indicates
   whether they should be publically writeable or not.  If fcheck is
   TRUE, this should make sure the directory is writeable by the user
   zuser (if zuser is NULL, then it must be writeable by any user);
   this is to avoid a window of vulnerability between
   fsysdep_in_directory and fsysdep_move_file.  This function should
   return FALSE on error; the zorig file should be removed even if an
   error occurs.  */
extern boolean fsysdep_move_file P((const char *zorig, const char *zto,
				    boolean fmkdirs, boolean fpublic,
				    boolean fcheck, const char *zuser));

/* Change the mode of a file.  The imode argument is a Unix mode.
   This should return FALSE on error.  */
extern boolean fsysdep_change_mode P((const char *zfile,
				      unsigned int imode));

/* Truncate a file which we are receiving into.  This may be done by
   closing the original file, removing it and reopening it.  This
   should return FALSE on error.  */
extern openfile_t esysdep_truncate P((openfile_t e, const char *zname));

/* Start expanding a wildcarded file name.  This should return FALSE
   on error; otherwise subsequent calls to zsysdep_wildcard should
   return file names.  The argument may have leading ~ characters.  */
extern boolean fsysdep_wildcard_start P((const struct ssysteminfo *qsys,
					 const char *zfile));

/* Get the next wildcard name.  This should return NULL when there are
   no more names to return.  The return value may point to a common
   static buffer.  The argument should be the same as that to
   fsysdep_wildcard_start.  There is no way to return error.  */
extern const char *zsysdep_wildcard P((const struct ssysteminfo *qsys,
				       const char *zfile));

/* Finish getting wildcard names.  This may be called before or after
   zsysdep_wildcard has returned NULL.  It should return FALSE on
   error.  */
extern boolean fsysdep_wildcard_end P((void));

/* Prepare to execute a bunch of file transfer requests.  This should
   make an entry in the spool directory so that the next time uucico
   is started up it will transfer these files.  The bgrade argument
   specifies the grade of the commands.  The commands themselves are
   in the pascmds array, which has ccmds entries.  The function should
   return NULL on error, or the jobid on success.  The jobid is a
   string that may be printed or passed to fsysdep_kill_job and
   related functions, but is otherwise uninterpreted.  */
 extern const char *zsysdep_spool_commands P((const struct ssysteminfo *qsys,
					      int bgrade, int ccmds,
					      const struct scmd *pascmds));

/* Get a file name to use for a data file to be copied to another
   system.  A file which will become an execute file will use a grade
   of 'X' (actually this is just convention, but it affects where the
   file will be placed in the spool directory on Unix).  The ztname,
   zdname and zxname arguments will all either be NULL or point to an
   array of CFILE_NAME_LEN characters in length.  The ztname array
   should be set to a temporary file name that could be passed to
   zsysdep_spool_file_name to retrieve the return value of this
   function; this will be appropriate for the temporary name in a send
   request.  The zdname array should be set to a data file name that
   is appropriate for the spool directory of the other system; this
   will be appropriate for the name of the destination file in a send
   request of a data file for an execution of some sort.  The zxname
   array should be set to an execute file name that is appropriate for
   the other system.  This should return NULL on error.  The return
   value may point to a common static buffer.  */

#define CFILE_NAME_LEN (15)

extern const char *zsysdep_data_file_name P((const struct ssysteminfo *qsys,
					     int bgrade, char *ztname,
					     char *zdname, char *zxname));

/* Get a name for a local execute file.  This is used by uux for a
   local command with remote files.  It should return NULL on error.
   The return value may point to a common static buffer.  */
extern const char *zsysdep_xqt_file_name P((void));

/* Beginning getting execute files.  To get a list of execute files,
   first fsysdep_get_xqt_init is called, then zsysdep_get_xqt is
   called several times until it returns NULL, then finally
   usysdep_get_xqt_free is called.  */
extern boolean fsysdep_get_xqt_init P((void));

/* Get the next execute file.  This should return NULL when finished
   (with *pferr set to FALSE).  On an error this should return NULL
   with *pferr set to TRUE.  This should set *pzsystem to the name of
   the system for which the execute file was created.  */
extern const char *zsysdep_get_xqt P((const char **pzsystem,
				      boolean *pferr));

/* Clean up after getting execute files.  */
extern void usysdep_get_xqt_free P((void));

/* Get the absolute pathname of a command to execute.  This is given
   the legal list of commands (which may be the special case "ALL")
   and the path.  It must return an absolute pathname to the command.
   If it gets an error it should set *pferr to TRUE and return NULL;
   if the command is not found it should set *pferr to FALSE and
   return NULL.  Otherwise, the return value may point to a common
   static buffer.  */
extern const char *zsysdep_find_command P((const char *zcmd,
					   const char *zcmds,
					   const char *zpath,
					   boolean *pferr));

#if ! ALLOW_FILENAME_ARGUMENTS
/* Check an argument to an execution command to make sure that it
   doesn't refer to a file name that may not be accessed.  This should
   check the argument to see if it is a filename.  If it is, it should
   either reject it out of hand or it should call fin_directory_list
   on the file with both qsys->zremote_receive and qsys->zremote_send.
   If the file is rejected, it should log an error and return FALSE.
   Otherwise it should return TRUE.  */
extern boolean fsysdep_xqt_check_file P((const struct ssysteminfo *qsys,
					 const char *zfile));
#endif /* ! ALLOW_FILENAME_ARGUMENTS */

/* Run an execute file.  The arguments are:

   qsys -- system for which execute file was created
   zuser -- user who requested execution
   pazargs -- list of arguments to command (element 0 is command)
   zfullcmd -- command and arguments stuck together in one string
   zinput -- file name for standard input (may be NULL)
   zoutput -- file name for standard output (may be NULL)
   fshell -- if TRUE, use /bin/sh to execute file
   pzerror -- set to name of standard error file
   pftemp -- set to TRUE if error is temporary, FALSE otherwise

   If fshell is TRUE, the command should be executed with /bin/sh
   (obviously, this can only really be done on Unix systems).  If an
   error occurs this should return FALSE and set *pftemp
   appropriately.  */
extern boolean fsysdep_execute P((const struct ssysteminfo *qsys,
				  const char *zuser,
				  const char **pazargs,
				  const char *zfullcmd,
				  const char *zinput,
				  const char *zoutput,
				  boolean fshell,
				  const char **pzerror,
				  boolean *pftemp));

/* Lock a particular uuxqt command (e.g. rmail).  This should return
   FALSE if the command is already locked.  This is used to make sure
   only one uuxqt process is handling a particular command.  There is
   no way to return error.  */

/* Lock for uuxqt execution.  If the global variable cMaxuuxqts is not
   zero, this should make sure that no more than cMaxuuxqts uuxqt
   processes are running at once.  Also, only one uuxqt may execute a
   particular command (specified by the -c option) at a time.  If zcmd
   is not NULL, it is a command that must be locked.  This should
   return a nonegative number which will be passed to
   fsysdep_unlock_uuxqt, or -1 on error.  */
extern int isysdep_lock_uuxqt P((const char *zcmd));

/* Unlock a uuxqt process.  This is passed the return value of
   isysdep_lock_uuxqt, as well as the zcmd argument passed to
   isysdep_lock_uuxqt.  It may return FALSE on error, but at present
   the return value is ignored.  */
extern boolean fsysdep_unlock_uuxqt P((int iseq, const char *zcmd));

/* See whether a particular uuxqt command is locked.  This should
   return TRUE if the command is locked (because isysdep_lock_uuxqt
   was called with it as an argument), FALSE otherwise.  There is no
   way to return error.  */
extern boolean fsysdep_uuxqt_locked P((const char *zcmd));

/* Lock an execute file in order to execute it.  This should return
   FALSE if the execute file is already locked.  There is no way to
   return error.  */
extern boolean fsysdep_lock_uuxqt_file P((const char *zfile));

/* Unlock an execute file.  This should return FALSE on error.  */
extern boolean fsysdep_unlock_uuxqt_file P((const char *zfile));

/* Lock the execution directory.  This should return FALSE if the
   directory is already locked.  There is no way to return error.  */
extern boolean fsysdep_lock_uuxqt_dir P((void));

/* Remove all files in the execution directory, and unlock it.  This
   should return FALSE on error.  */
extern boolean fsysdep_unlock_uuxqt_dir P((void));

/* Add the working directory to a file name.  If the file already has
   a directory, it should not be changed.  The return value may point
   to a common static buffer.  If the flocal argument is TRUE, then
   this is a local file (so, for example, a leading ~ should be
   expanded as appropriate); otherwise the file is on a remote system.
   This should return NULL on error.  */
extern const char *zsysdep_add_cwd P((const char *zfile,
				      boolean flocal));

/* See whether a file name will need the current working directory
   when zsysdep_add_cwd is called on it.  This will be called before
   usysdep_initialize.  It should just check whether the argument is
   an absolute path.  See the comment above usysdep_initialize in this
   file for an explanation of why things are done this way.  */
extern boolean fsysdep_needs_cwd P((const char *zfile));

/* Get the base name of a file.  The file will be a local file name,
   and this function should return the base file name, ideally in a
   form which will make sense on most systems; it will be used if the
   destination of a uucp is a directory.  */
extern const char *zsysdep_base_name P((const char *zfile));

/* Return a filename within a directory.  */
extern const char *zsysdep_in_dir P((const char *zdir, const char *zfile));

/* Get the mode of a file.  This should return a Unix style file mode.
   It should return 0 on error.  */
extern unsigned int isysdep_file_mode P((const char *zfile));

/* See whether the user has access to a file.  This is called by uucp
   and uux to prevent copying of a file which uucp can read but the
   user cannot.  If access is denied, this should log an error message
   and return FALSE.  */
extern boolean fsysdep_access P((const char *zfile));

/* See whether the daemon has access to a file.  This is called by
   uucp and uux when a file is queued up for transfer without being
   copied into the spool directory.  It is merely an early error
   check, as the daemon would of course discover the error itself when
   it tried the transfer.  If access would be denied, this should log
   an error message and return FALSE.  */
extern boolean fsysdep_daemon_access P((const char *zfile));

/* Translate a destination from system!user to a place in the public
   directory where uupick will get the file.  On Unix this produces
   system!~/receive/user/localname, and that's probably what it has to
   produce on any other system as well.  Returns NULL on a usage
   error, or otherwise returns an allocated string.  */
extern char *zsysdep_uuto P((const char *zdest));

/* Return TRUE if a pathname exists and is a directory.  */
extern boolean fsysdep_directory P((const char *zpath));

/* Walk a directory tree.  The zdir argument is the directory to walk.
   The pufn argument is a function to call on each regular file in the
   tree.  The first argument to pufn should be the full filename; the
   second argument to pufn should be the filename relative to zdir;
   the third argument to pufn should be the pinfo argument to
   usysdep_walk_tree.  The usysdep_walk_tree function should return
   FALSE on error.  */
extern boolean usysdep_walk_tree P((const char *zdir,
				    void (*pufn) P((const char *zfull,
						    const char *zrelative,
						    pointer pinfo)),
				    pointer pinfo));

/* Return the jobid of a work file, given the sequence value.  On
   error this should log an error and return NULL.  The jobid is a
   string which may be printed out and read in and passed to
   fsysdep_kill_job, etc., but is not otherwise interpreted.  The
   return value may point to a common statis buffer.  */
extern const char *zsysdep_jobid P((const struct ssysteminfo *qsys,
				    pointer pseq));

/* See whether the current user is permitted to kill jobs submitted by
   another user.  This should return TRUE if permission is granted,
   FALSE otherwise.  */
extern boolean fsysdep_privileged P((void));

/* Kill a job, given the jobid.  This should remove all associated
   files and in general eliminate the job completely.  On error it
   should log an error message and return FALSE.  */
extern boolean fsysdep_kill_job P((const char *zjobid));

/* Rejuvenate a job, given the jobid.  If possible, this should update
   the time associated with the job such that it will not be
   eliminated by uuclean or similar programs that check the creation
   time.  This should affect the return value of isysdep_work_time.
   On error it should log an error message and return FALSE.  */
extern boolean fsysdep_rejuvenate_job P((const char *zjobid));

/* Get the time a job was queued, given the sequence number.  There is
   no way to indicate error.  The return value must use the same epoch
   as isysdep_time.  */
extern long isysdep_work_time P((const struct ssysteminfo *qsys,
				 pointer pseq));

/* Get the time a file was created.  This is called by uustat on
   execution files.  There is no way to indicate error.  The return
   value must use the same epoch as isysdep_time.  */
extern long isysdep_file_time P((const char *zfile));

/* Get the size in bytes of a file.  There is no way to indicate
   error.  This is only used by uustat.  */
extern long csysdep_size P((const char *zfile));

/* Start getting status information for all systems with available
   status information.  There may be status information for unknown
   systems, which is why this series of functions is used.  The phold
   argument is used to pass information around, to possibly avoid the
   use of static variables.  On error this should log an error and
   return FALSE.  */
extern boolean fsysdep_all_status_init P((pointer *phold));

/* Get status information for the next system.  This should return the
   system name and fill in the qstat argument.  The phold argument
   will be that set by fsysdep_all_status_init.  On error this should
   log an error, set *pferr to TRUE, and return NULL.  */
extern const char *zsysdep_all_status P((pointer phold, boolean *pferr,
					 struct sstatus *qstat));

/* Free up anything allocated by fsysdep_all_status_init and
   zsysdep_all_status.  The phold argument is that set by
   fsysdep_all_status_init.  */
extern void usysdep_all_status_free P((pointer phold));

/* Display the process status of all processes holding lock files.
   This is uustat -p.  The return value is passed to usysdep_exit.  */
extern boolean fsysdep_lock_status P((void));

/* Return TRUE if the user has legitimate access to the port.  This is
   used by cu to control whether the user can open a port directly,
   rather than merely being able to dial out on it.  Opening a port
   directly allows the modem to be reprogrammed.  */
extern boolean fsysdep_port_access P((struct sport *));

/* Return whether the given port could be named by the given line.  On
   Unix, the line argument would be something like "ttyd0", and this
   function should return TRUE if the named port is "/dev/ttyd0".  */
extern boolean fsysdep_port_is_line P((struct sport *qport,
				       const char *zline));

/* Set the terminal into raw mode.  In this mode no input characters
   should be treated specially, and characters should be made
   available as they are typed.  The original terminal mode should be
   saved, so that it can be restored by fsysdep_terminal_restore.  If
   flocalecho is TRUE, then local echoing should still be done;
   otherwise echoing should be disabled.  This function returns FALSE
   on error.  */
extern boolean fsysdep_terminal_raw P((boolean flocalecho));

/* Restore the terminal back to the original setting, before
   fsysdep_terminal_raw was called.  Returns FALSE on error.  */
extern boolean fsysdep_terminal_restore P((void));

/* Read a line from the terminal.  The fsysdep_terminal_raw function
   will have been called.  This should print the zprompt argument
   (unless it is NULL) and return the line, possibly in a common
   static buffer, or NULL on error.  */
extern const char *zsysdep_terminal_line P((const char *zprompt));

/* Write a line to the terminal, ending with a newline.  This is
   basically just puts (zline, stdout), except that the terminal will
   be in raw mode, so on ASCII Unix systems the line needs to end with
   \r\n.  */
extern boolean fsysdep_terminal_puts P((const char *zline));

/* If faccept is TRUE, permit the user to generate signals from the
   terminal.  If faccept is FALSE, turn signals off again.  After
   fsysdep_terminal_raw is called, signals should be off.  Return
   FALSE on error.  */
extern boolean fsysdep_terminal_signals P((boolean faccept));

/* The cu program expects the system dependent code to handle the
   details of copying data from the communications port to the
   terminal.  This should be set up by fsysdep_cu_init, and done while
   fsysdep_cu is called.  It is permissible to do it on a continual
   basis (on Unix a subprocess handles it) so long as the copying can
   be stopped by the fsysdep_cu_copy function.

   The fsysdep_cu_init function does any system dependent
   initialization needed for this.  */
extern boolean fsysdep_cu_init P((void));

/* Copy all data from the communications port to the terminal, and all
   data from the terminal to the communications port.  Keep this up
   until the escape character *zCuvar_escape is seen.  Set *pbcmd to
   the character following the escape character; after the escape
   character, the hostname should be printed, possibly after a delay.
   If two escape characters are entered in sequence, this function
   should send a single escape character to the port, and not return.
   Returns FALSE on error.  */
extern boolean fsysdep_cu P((char *pbcmd));

/* If fcopy is TRUE, start copying data from the communications port
   to the terminal.  If fcopy is FALSE, stop copying data.  This
   function may be called several times during a cu session.  It
   should return FALSE on error.  */
extern boolean fsysdep_cu_copy P((boolean fcopy));

/* Stop copying data from the communications port to the terminal, and
   generally clean up after fsysdep_cu_init and fsysdep_cu.  Returns
   FALSE on error.  */
extern boolean fsysdep_cu_finish P((void));

/* Run a shell command.  If zcmd is NULL, or *zcmd == '\0', just
   start up a shell.  The second argument is one of the following
   values.  This should return FALSE on error.  */
enum tshell_cmd
{
  /* Attach stdin and stdout to the terminal.  */
  SHELL_NORMAL,
  /* Attach stdout to the communications port, stdin to the terminal.  */
  SHELL_STDOUT_TO_PORT,
  /* Attach stdin to the communications port, stdout to the terminal.  */
  SHELL_STDIN_FROM_PORT,
  /* Attach both stdin and stdout to the communications port.  */
  SHELL_STDIO_ON_PORT
};

extern boolean fsysdep_shell P((const char *zcmd,
				enum tshell_cmd tcmd));

/* Change directory.  If zdir is NULL, or *zdir == '\0', change to the
   user's home directory.  Return FALSE on error.  */
extern boolean fsysdep_chdir P((const char *zdir));

/* Suspend the current process.  This is only expected to work on Unix
   versions that support SIGTSTP.  In general, people can just shell
   out.  */
extern boolean fsysdep_suspend P((void));

/* Start getting files for uupick.  The zsystem argument may be NULL
   to get files from all systems, or it may specify a particular
   system.  This returns FALSE on error.  */
extern boolean fsysdep_uupick_init P((const char *zsystem));

/* Get the next file for uupick.  This returns the basic file name.
   It sets *pzfull to the full name, and *pzfrom to the name of the
   system which sent this file over.  The zsystem argument should be
   the same as the argument to fsysdep_uupick_init.  This returns NULL
   when all files been returned.  */
extern const char *zsysdep_uupick P((const char *zsystem,
				     const char **pzfrom,
				     const char **pzfull));

/* Clean up after getting files for uupick.  */
extern boolean fsysdep_uupick_free P((const char *zsystem));

/* Remove a directory and all the files in it.  */
extern boolean fsysdep_rmdir P((const char *zdir));

#endif /* ! defined (SYSTEM_H) */
