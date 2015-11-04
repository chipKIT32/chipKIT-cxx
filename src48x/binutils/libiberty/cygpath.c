/* Support Cygwin paths under MinGW.
   Copyright (C) 2006 Free Software Foundation, Inc.
   Written by CodeSourcery.

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or modify it
under the terms of the GNU Library General Public License as published
by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If not, write
to the Free Software Foundation, Inc., 51 Franklin Street - Fifth
Floor, Boston, MA 02110-1301, USA.  */

#include <windows.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>
#include <process.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "libiberty.h"

/* If non-zero, we have attempted to use cygpath.  CYGPATH_PEX may
   still be NULL, if cygpath is unavailable.  */
static int cygpath_initialized;

/* If non-NULL, an instance of cygpath connected via a pipe.  */
static struct pex_obj *cygpath_pex;

/* The input to cygpath.  */
static FILE *cygpath_in;

/* The output from cygpath.  */
static FILE *cygpath_out;

/* If non-NULL, a file to which path translations are logged.  */
static FILE *cygpath_log;

/* Record MESSAGE in the CYGPATH_LOG.  MESSAGE is a format string,
   which is expected to have a single "%s" field, to be replaced by
   ARG.  */
static void
cygpath_log_msg_arg (const char *message, const char *arg)
{
  if (!cygpath_log)
    return;
  fprintf (cygpath_log, "[%d] cygpath: ", _getpid ());
  fprintf (cygpath_log, message, arg);
  fprintf (cygpath_log, "\n");
  fflush (cygpath_log);
}

/* Record MESSAGE in the CYGPATH_LOG.  */
static void
cygpath_log_msg (const char *message)
{
  cygpath_log_msg_arg ("%s", message);
}

/* An error has occured.  Add the MESSAGE to the CYGPATH_LOG, noting
   the cause of the error based on errno.  */
static void
cygpath_perror (const char *message)
{
  if (!cygpath_log)
    return;
  fprintf (cygpath_log, "[%d] cygpath: error: %s: %s\n",
	   _getpid(), message, strerror (errno));
  fflush (cygpath_log);
}

/* Closes CYGPATH_PEX and frees all associated
   resoures.  */
static void
cygpath_close (void)
{
  /* Free resources.  */
  if (cygpath_out)
    {
      fclose (cygpath_out);
      cygpath_out = NULL;
    }
  if (cygpath_in)
    {
      fclose (cygpath_in);
      cygpath_in = NULL;
    }
  if (cygpath_pex)
    {
      pex_free (cygpath_pex);
      cygpath_pex = NULL;
    }
  if (cygpath_log)
    {
      cygpath_log_msg ("end");
      cygpath_log = NULL;
    }
}

/* CYG_PATH is a pointer to a Cygwin path.  This function converts the
   Cygwin path to a Windows path, storing the result in
   WIN32_PATH.  Returns true if the conversion was successful; false
   otherwise.  */
int
cygpath (const char *cyg_path, char win32_path[MAX_PATH + 1])
{
  bool ok;
  bool retrying;
  
  /* Special-case the empty path.  cygpath cannot handle the empty
     path correctly.  It ignores the empty line, waiting for a
     non-empty line, which in turn causes an application using this
     function to appear stuck.  */
  if (cyg_path[0] == '\0')
    {
      win32_path[0] = '\0';
      return true;
    }
  
  retrying = false;

 retry:
  if (!cygpath_initialized) 
    {
      const char *argv[] = { "cygpath", "-w", "-f", "-", NULL };
      const char *cygpath_path;
      const char *log;
      int err;

      /* If we are unable to invoke cygpath, we do not want to try
	 again.  So, we set the initialized flag at this point; if
	 errors occur during the invocation, it will remain set.  */
      cygpath_initialized = 1;
      /* Check to see if the user wants cygpath support.  */
      cygpath_path = getenv ("CYGPATH");
      if (!cygpath_path)
	/* The user doesn't need to support Cygwin paths.  */
	goto error;
      /* If debugging, open the log file.  */
      log = getenv ("CSL_DEBUG_CYGPATH");
      if (log && log[0])
	{
	  /* The log file is opened for "append" so that multiple
	     processes (perhaps invoked from "make") can share it.  */
	  cygpath_log = fopen (log, "a");
	  if (cygpath_log)
	    cygpath_log_msg ("begin");
	}
      /* If the environment variable is set to a non-empty string, use
	 that string as the path to cygpath.  */ 
      if (cygpath_path[0] != '\0')
	argv[0] = cygpath_path;
      /* Create the pex object.  */
      cygpath_pex = pex_init (PEX_SEARCH | PEX_USE_PIPES, 
			      "cygpath", NULL);
      if (!cygpath_pex)
	goto error;
      /* Get the FILE we will use to write to the child.  */
      cygpath_in = pex_input_pipe (cygpath_pex, /*binary=*/0);
      if (!cygpath_in)
	goto error;
      /* Start the child process.  */
      if (pex_run (cygpath_pex, PEX_SEARCH | PEX_USE_PIPES, 
		   argv[0], (char**) argv, 
		   NULL, NULL,
		   &err) != NULL)
	goto error;
      /* Get the FILE we will use to read from the child.  */
      cygpath_out = pex_read_output (cygpath_pex, /*binary=*/1);
      if (!cygpath_out)
	goto error;
    }
  else if (!cygpath_pex) 
    /* We previously tried to use cygpath, but something went wrong.  */
    return false;

  /* Write CYG_PATH to the child, on a line by itself.  */
  cygpath_log_msg_arg ("-> %s", cyg_path);
  if (fprintf (cygpath_in, "%s\n", cyg_path) < 0)
    {
      cygpath_perror ("write failed");
      goto error;
    }
  /* Flush the output.  (We cannot set the stream into line-buffered
     mode with setvbuf because Windows treats _IOLBF as a synonym for
     _IOFBF.)  */
  if (fflush (cygpath_in))
    cygpath_perror ("flush failed");
  /* Read the output.  */
  ok = true;
  while (1)
    {
      size_t pathlen;
      if (!fgets (win32_path, MAX_PATH, cygpath_out))
	{
	  if (ferror (cygpath_out))
	    cygpath_perror ("read failed");
	  else
	    {
	      cygpath_log_msg ("error: EOF");
	      /* Unfortunately, cygpath sometimes crashes for no
		 apparent reason.  We give it two chances... */
	      if (!retrying)
		{
		  retrying = true;
		  cygpath_log_msg ("retrying");
		  cygpath_close ();
		  cygpath_initialized = 0;
		  goto retry;
		}
	      else
		{
		  /* Give up, but in case the error is specific to
		     this path, reopen at the next request.  */
		  cygpath_close ();
		  cygpath_initialized = 0;
		}
	    }
	  goto error;
	}
      pathlen = strlen (win32_path);
      if (pathlen == 0 && ok)
	/* This isn't a well-formed response from cygpath.  */
	goto error;
      if (win32_path[pathlen - 1] == '\n')
	{
	  win32_path[pathlen - 1] = '\0';
	  cygpath_log_msg_arg ("<- %s", win32_path);
	  break;
	}
      /* We didn't reach the end of the line.  There's no point in
	 trying to use this output, since we know the length of
	 paths are limited to MAX_PATH characters, but we read the
	 entire line so that we are still in sync with
	 cygpath.  */
      ok = false;
      if (cygpath_log)
	cygpath_log_msg_arg ("error: invalid response: %s",
			     win32_path);
    }
  
  return ok;
  
 error:
  cygpath_close();
  return false;
}

typedef struct
{
  DWORD dwRVAFunctionNameList;
  DWORD dwUseless1;
  DWORD dwUseless2;
  DWORD dwRVAModuleName;
  DWORD dwRVAFunctionAddressList;
} IMAGE_IMPORT_MODULE_DIRECTORY, *PIMAGE_IMPORT_MODULE_DIRECTORY;

#define IMAGE_DIRECTORY_ENTRY_IMPORT 1

/* Returns the handle for the MVCRT DLL, or NULL if it is not
   available.  */
static HMODULE
msvcrt_dll (void)
{
  static HMODULE dll = (HMODULE)(-1);
  PIMAGE_OPTIONAL_HEADER opth;
  PIMAGE_IMPORT_MODULE_DIRECTORY impdir;
  HANDLE image;

  /* After we call GetModuleHandle, DLL will be either a valid handle
     or NULL, so this check ensures that we only try to load the
     library once.  */
  if (dll != (HMODULE)(-1))
    return dll;

  dll = NULL;

  /* Check which C runtime is loaded by this executable.  Do that by
     looking at the import directory for DLLs the EXE links to that
     looks like msvcr*.  This catches e.g., `msvcr90.dll' and
     `msvcrt.dll'.  */

  image = GetModuleHandle (NULL);

  /* Get to the PE optional header.  */
  opth = (PIMAGE_OPTIONAL_HEADER)
    ((DWORD) image + ((PIMAGE_DOS_HEADER) image)->e_lfanew
     + sizeof (DWORD) + sizeof (IMAGE_FILE_HEADER));
  impdir = (PIMAGE_IMPORT_MODULE_DIRECTORY)
    (opth->ImageBase
     + opth->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

  for (; impdir->dwRVAModuleName; impdir++)
    {
      char *dllname;

      dllname = (char *) opth->ImageBase + impdir->dwRVAModuleName;
      if (strncasecmp (dllname, "msvcr", sizeof ("msvcr") - 1) == 0)
	{
	  dll = GetModuleHandle (dllname);
	  if (dll == NULL)
	    {
	      if (cygpath_log)
		cygpath_log_msg ("error: GetModuleHandle failed");
	    }
	  break;
	}
    }

  return dll;
}

/* Call the underlying MSVCRT fopen with PATH and MODE, and return
   what it returns.  */
static FILE *
msvcrt_fopen (const char *path, const char *mode)
{
  typedef FILE *(fopen_type)(const char *path, 
			     const char *mode);

  static fopen_type *f = NULL;

  /* Get the address of "fopen".  */
  if (!f) 
    {
      HMODULE dll = msvcrt_dll ();
      if (!dll)
	{
	  errno = ENOSYS;
	  return NULL;
	}
      f = (fopen_type *) GetProcAddress (dll, "fopen");
      if (!f)
	{
	  errno = ENOSYS;
	  return NULL;
	}
    }

  /* Call fopen.  */
  return (*f)(path, mode);
}

FILE *
fopen (const char *path, const char *mode)
{
  FILE *f;
  char win32_path[MAX_PATH + 1];

  /* Assume PATH is a Windows path.  */
  f = msvcrt_fopen (path, mode);
  if (f || errno != ENOENT)
    return f;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (path, win32_path))
    f = msvcrt_fopen (win32_path, mode);
  return f;
}

int 
open (const char *path, int oflag, ...)
{
  int fd;
  char win32_path[MAX_PATH + 1];
  int pmode = 0;

  if ((oflag & _O_CREAT))
    {
      va_list ap;
      va_start (ap, oflag);
      pmode = va_arg (ap, int); 
      va_end (ap);
    }

  /* Assume PATH is a Windows path.  */
  fd = _open (path, oflag, pmode);
  if (fd != -1 || errno != ENOENT)
    return fd;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (path, win32_path))
    fd = _open (win32_path, oflag, pmode);
  return fd;
}

/* Call the underlying MSVCRT stat with PATH and MODE, and return
   what it returns.  */
int
msvcrt_stat (const char *path, struct stat *buffer)
{
  typedef int *(stat_type)(const char *path,
			   struct stat *buffer);

  static stat_type *f = NULL;

  /* Get the address of "stat".  */
  if (!f)
    {
      HMODULE dll = msvcrt_dll ();
      if (!dll)
	{
	  errno = ENOSYS;
	  return NULL;
	}
      if (sizeof (time_t) == 8)
	f = (stat_type *) GetProcAddress (dll, "_stat64i32");
      else
	{
	  f = (stat_type *) GetProcAddress (dll, "_stat32");
	  if (!f)
	    f = (stat_type *) GetProcAddress (dll, "_stat");
	}
      if (!f)
	{
	  errno = ENOSYS;
	  return NULL;
	}
    }

  /* Call fopen.  */
  return (*f)(path, buffer);
}

int
stat (const char *path, struct stat *buffer)
{
  int r;
  char win32_path[MAX_PATH + 1];

  /* Assume PATH is a Windows path.  */
  r = msvcrt_stat (path, (struct _stat *) buffer);
  if (r != -1 || errno != ENOENT)
    return r;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (path, win32_path))
    r = msvcrt_stat (win32_path, (struct _stat *) buffer);
  return r;
}

int
access (const char *path, int mode)
{
  int r;
  char win32_path[MAX_PATH + 1];

#ifdef _WIN32
  /* Some GNU tools mistakenly defined X_OK to 1 on Windows.  */
  mode = mode & ~1;
#endif
  /* Assume PATH is a Windows path.  */
  r = _access (path, mode);
  if (r != -1 || errno != ENOENT)
    return r;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (path, win32_path))
    r = _access (win32_path, mode);
  return r;
}

/* Given the WINDOWS_CODE (typically the result of GetLastError), set
   ERRNO to the corresponding error code.  If there is no obvious
   correspondence, ERRNO will be set to EACCES.  */
static void
set_errno_from_windows_code (DWORD windows_code)
{
  int mapping[][2] = {
    {ERROR_ACCESS_DENIED, EACCES},
    {ERROR_ACCOUNT_DISABLED, EACCES},
    {ERROR_ACCOUNT_RESTRICTION, EACCES},
    {ERROR_ALREADY_ASSIGNED, EBUSY},
    {ERROR_ALREADY_EXISTS, EEXIST},
    {ERROR_ARITHMETIC_OVERFLOW, ERANGE},
    {ERROR_BAD_COMMAND, EIO},
    {ERROR_BAD_DEVICE, ENODEV},
    {ERROR_BAD_DRIVER_LEVEL, ENXIO},
    {ERROR_BAD_EXE_FORMAT, ENOEXEC},
    {ERROR_BAD_FORMAT, ENOEXEC},
    {ERROR_BAD_LENGTH, EINVAL},
    {ERROR_BAD_PATHNAME, ENOENT},
    {ERROR_BAD_PIPE, EPIPE},
    {ERROR_BAD_UNIT, ENODEV},
    {ERROR_BAD_USERNAME, EINVAL},
    {ERROR_BROKEN_PIPE, EPIPE},
    {ERROR_BUFFER_OVERFLOW, ENOMEM},
    {ERROR_BUSY, EBUSY},
    {ERROR_BUSY_DRIVE, EBUSY},
    {ERROR_CALL_NOT_IMPLEMENTED, ENOSYS},
    {ERROR_CRC, EIO},
    {ERROR_CURRENT_DIRECTORY, EINVAL},
    {ERROR_DEVICE_IN_USE, EBUSY},
    {ERROR_DIR_NOT_EMPTY, EEXIST},
    {ERROR_DIRECTORY, ENOENT},
    {ERROR_DISK_CHANGE, EIO},
    {ERROR_DISK_FULL, ENOSPC},
    {ERROR_DRIVE_LOCKED, EBUSY},
    {ERROR_ENVVAR_NOT_FOUND, EINVAL},
    {ERROR_EXE_MARKED_INVALID, ENOEXEC},
    {ERROR_FILE_EXISTS, EEXIST},
    {ERROR_FILE_INVALID, ENODEV},
    {ERROR_FILE_NOT_FOUND, ENOENT},
    {ERROR_FILENAME_EXCED_RANGE, ENAMETOOLONG},
    {ERROR_GEN_FAILURE, EIO},
    {ERROR_HANDLE_DISK_FULL, ENOSPC},
    {ERROR_INSUFFICIENT_BUFFER,  ENOMEM},
    {ERROR_INVALID_ACCESS, EINVAL},
    {ERROR_INVALID_ADDRESS, EFAULT},
    {ERROR_INVALID_BLOCK, EFAULT},
    {ERROR_INVALID_DATA, EINVAL},
    {ERROR_INVALID_DRIVE, ENODEV},
    {ERROR_INVALID_EXE_SIGNATURE,  ENOEXEC},
    {ERROR_INVALID_FLAGS, EINVAL},
    {ERROR_INVALID_FUNCTION,  ENOSYS},
    {ERROR_INVALID_HANDLE, EBADF},
    {ERROR_INVALID_LOGON_HOURS,  EACCES},
    {ERROR_INVALID_NAME, ENOENT},
    {ERROR_INVALID_OWNER, EINVAL},
    {ERROR_INVALID_PARAMETER, EINVAL},
    {ERROR_INVALID_PASSWORD, EPERM},
    {ERROR_INVALID_PRIMARY_GROUP, EINVAL},
    {ERROR_INVALID_SIGNAL_NUMBER, EINVAL},
    {ERROR_INVALID_TARGET_HANDLE, EIO},
    {ERROR_INVALID_WORKSTATION, EACCES},
    {ERROR_IO_DEVICE, EIO},
    {ERROR_IO_INCOMPLETE, EINTR},
    {ERROR_LOCKED, EBUSY},
    {ERROR_LOGON_FAILURE, EACCES},
    {ERROR_MAPPED_ALIGNMENT, EINVAL},
    {ERROR_META_EXPANSION_TOO_LONG, E2BIG},
    {ERROR_MORE_DATA, EPIPE},
    {ERROR_NEGATIVE_SEEK, ESPIPE},
    {ERROR_NO_DATA, EPIPE},
    {ERROR_NO_MORE_SEARCH_HANDLES, EIO},
    {ERROR_NO_PROC_SLOTS, EAGAIN},
    {ERROR_NO_SUCH_PRIVILEGE, EACCES},
    {ERROR_NOACCESS, EFAULT},
    {ERROR_NONE_MAPPED, EINVAL},
    {ERROR_NOT_ENOUGH_MEMORY, ENOMEM},
    {ERROR_NOT_READY, ENODEV},
    {ERROR_NOT_SAME_DEVICE, EXDEV},
    {ERROR_OPEN_FAILED, EIO},
    {ERROR_OPERATION_ABORTED, EINTR},
    {ERROR_OUTOFMEMORY,  ENOMEM},
    {ERROR_PASSWORD_EXPIRED, EACCES},
    {ERROR_PATH_BUSY,  EBUSY},
    {ERROR_PATH_NOT_FOUND, ENOTDIR},
    {ERROR_PIPE_BUSY, EBUSY},
    {ERROR_PIPE_CONNECTED, EPIPE},
    {ERROR_PIPE_LISTENING, EPIPE},
    {ERROR_PIPE_NOT_CONNECTED, EPIPE},
    {ERROR_PRIVILEGE_NOT_HELD, EACCES},
    {ERROR_READ_FAULT, EIO},
    {ERROR_SEEK, ESPIPE},
    {ERROR_SEEK_ON_DEVICE, ESPIPE},
    {ERROR_SHARING_BUFFER_EXCEEDED, ENFILE},
    {ERROR_STACK_OVERFLOW, ENOMEM},
    {ERROR_SWAPERROR, ENOENT},
    {ERROR_TOO_MANY_MODULES, EMFILE},
    {ERROR_TOO_MANY_OPEN_FILES, EMFILE},
    {ERROR_UNRECOGNIZED_MEDIA,  ENXIO},
    {ERROR_UNRECOGNIZED_VOLUME,  ENODEV},
    {ERROR_WAIT_NO_CHILDREN,  ECHILD},
    {ERROR_WRITE_FAULT, EIO},
    {ERROR_WRITE_PROTECT, EROFS}
/*  MinGW does not define ETXTBSY as yet.  
    {ERROR_LOCK_VIOLATION, ETXTBSY},
    {ERROR_SHARING_VIOLATION, ETXTBSY}, 
*/
  };

  size_t i;

  for (i = 0; i < sizeof (mapping)/sizeof (mapping[0]); ++i)
    if (mapping[i][0] == windows_code)
      {
	errno = mapping[i][1];
	return;
      }

  /* Unrecognized error. Use EACCESS to have some error code,
     not misleading "No error" thing.  */
  errno = EACCES;      
}

int rename (const char *oldpath, const char *newpath)
{
  BOOL r;
  int oldpath_converted = 0;
  char win32_oldpath[MAX_PATH + 1];
  char win32_newpath[MAX_PATH + 1];

  /* Older versions of the cygpath program called FindFirstFile, but
     not FindClose.  As a result, a long-running cygpath program ends
     up leaking these handles, and, as a result, the Windows kernel
     will not let us remove or rename things in directories.  Therefore,
     we kill the child cygpath program now.

     The defect in cygpath was corrected by this patch:

       http://cygwin.com/ml/cygwin-patches/2007-q1/msg00033.html

     but older versions of cygpath will be in use for the forseeable
     future.  */

  cygpath_close ();
  cygpath_initialized = 0;

  /* Assume all paths are Windows paths.  */
  r = MoveFileEx (oldpath, newpath, MOVEFILE_REPLACE_EXISTING);
  if (r)
      return 0;
  else if (GetLastError () != ERROR_PATH_NOT_FOUND)
    goto error;

  /* Perhaps the old path is a cygwin path?  */
  if (cygpath (oldpath, win32_oldpath))
    {
      oldpath_converted = 1;
      r = MoveFileEx (win32_oldpath, newpath, MOVEFILE_REPLACE_EXISTING);      
      if (r)
          return 0;
      else if (GetLastError () != ERROR_PATH_NOT_FOUND)
          goto error;
    }

  /* Perhaps the new path is a cygwin path?  */
  if (cygpath (newpath, win32_newpath))
    {
      r = MoveFileEx (oldpath_converted ? win32_oldpath : oldpath,
		      win32_newpath, MOVEFILE_REPLACE_EXISTING);
      if (r == TRUE)
	return 0;
    }
error:
  set_errno_from_windows_code (GetLastError ());
  return -1;      
}

int remove (const char *pathname)
{
  int r;
  char win32_path[MAX_PATH + 1];

  cygpath_close ();
  cygpath_initialized = 0;

  /* Assume PATH is a Windows path.  */
  r = _unlink (pathname);
  if (r != -1 || errno != ENOENT)
    return r;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (pathname, win32_path))
    r = _unlink (win32_path);
  return r;
}

int unlink(const char *pathname)
{
    return remove (pathname);
}

int
chdir (const char *path)
{
  int ret;
  char win32_path[MAX_PATH + 1];

  /* Assume PATH is a Windows path.  */
  ret = _chdir (path);
  if (ret != -1 || errno != ENOENT)
    return ret;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (path, win32_path))
    ret = _chdir (win32_path);
  return ret;
}
