/*
 * Path Functions
 *
 * Many of this functions are in SHLWAPI.DLL also
 *
 */
#include <string.h>
#include <ctype.h>
#include "debugtools.h"
#include "winnls.h"
#include "winversion.h"
#include "winreg.h"

#include "shlobj.h"
#include "shell32_main.h"
#include "windef.h"
#include "options.h"
#include "wine/undocshell.h"
#include "wine/unicode.h"
#include "shlwapi.h"

DEFAULT_DEBUG_CHANNEL(shell);

#define isSlash(x) ((x)=='\\' || (x)=='/')
/*
	########## Combining and Constructing paths ##########
*/

/*************************************************************************
 * PathAppendA		[SHLWAPI.@]
 * 
 * NOTES
 *  concat path lpszPath2 onto lpszPath1
 *
 * FIXME
 *  the resulting path is also canonicalized
 */
BOOL WINAPI PathAppendA(
	LPSTR lpszPath1,
	LPCSTR lpszPath2) 
{
	TRACE("%s %s\n",lpszPath1, lpszPath2);
	while (lpszPath2[0]=='\\') lpszPath2++;
	PathCombineA(lpszPath1,lpszPath1,lpszPath2);
	return TRUE;
}

/*************************************************************************
 * PathAppendW		[SHLWAPI.@]
 */
BOOL WINAPI PathAppendW(
	LPWSTR lpszPath1,
	LPCWSTR lpszPath2) 
{
	TRACE("%s %s\n",debugstr_w(lpszPath1), debugstr_w(lpszPath2));
	while (lpszPath2[0]=='\\') lpszPath2++;
	PathCombineW(lpszPath1,lpszPath1,lpszPath2);
	return TRUE;
}

/*************************************************************************
 * PathAppendAW		[SHELL32.36]
 */
BOOL WINAPI PathAppendAW(
	LPVOID lpszPath1,
	LPCVOID lpszPath2)
{
	if (VERSION_OsIsUnicode())
	  return PathAppendW(lpszPath1, lpszPath2);
	return PathAppendA(lpszPath1, lpszPath2);
}

/*************************************************************************
 * PathCombineA		[SHLWAPI.@]
 * 
 * NOTES
 *  if lpszFile='.' skip it
 *  szDest can be equal to lpszFile. Thats why we use sTemp
 *
 * FIXME
 *  the resulting path is also canonicalized
 */
LPSTR WINAPI PathCombineA(
	LPSTR szDest,
	LPCSTR lpszDir,
	LPCSTR lpszFile) 
{
	char sTemp[MAX_PATH];
	TRACE("%p %p->%s %p->%s\n",szDest, lpszDir, lpszDir, lpszFile, lpszFile);
	
	
	if (!lpszFile || !lpszFile[0] || (lpszFile[0]=='.' && !lpszFile[1]) ) 
	{
	  strcpy(szDest,lpszDir);
	  return szDest;
	}

	/*  if lpszFile is a complete path don't care about lpszDir */
	if (PathGetDriveNumberA(lpszFile) != -1)
	{
	  strcpy(szDest,lpszFile);
	}
	else if (lpszFile[0] == '\\' )
	{
	  strcpy(sTemp,lpszDir);
	  PathStripToRootA(sTemp);
	  strcat(sTemp,lpszFile);
	  strcpy(szDest,sTemp);
	}
	else
	{
	  strcpy(sTemp,lpszDir);
	  PathAddBackslashA(sTemp);
	  strcat(sTemp,lpszFile);
	  strcpy(szDest,sTemp);
	}
	return szDest;
}

/*************************************************************************
 * PathCombineW		 [SHLWAPI.@]
 */
LPWSTR WINAPI PathCombineW(
	LPWSTR szDest,
	LPCWSTR lpszDir,
	LPCWSTR lpszFile) 
{
	WCHAR sTemp[MAX_PATH];
	TRACE("%p %p->%s %p->%s\n",szDest, lpszDir, debugstr_w(lpszDir),
			 lpszFile, debugstr_w(lpszFile));
	
	
	if (!lpszFile || !lpszFile[0] || (lpszFile[0]==(WCHAR)'.' && !lpszFile[1]) ) 
	{
          strcpyW(szDest,lpszDir);
	  return szDest;
	}

	/*  if lpszFile is a complete path don't care about lpszDir */
	if (PathGetDriveNumberW(lpszFile) != -1)
	{
            strcpyW(szDest,lpszFile);
	}
	else if (lpszFile[0] == (WCHAR)'\\' )
	{
	  strcpyW(sTemp,lpszDir);
	  PathStripToRootW(sTemp);
	  strcatW(sTemp,lpszFile);
	  strcpyW(szDest,sTemp);
	}
	else
	{
	  strcpyW(sTemp,lpszDir);
	  PathAddBackslashW(sTemp);
	  strcatW(sTemp,lpszFile);
	  strcpyW(szDest,sTemp);
	}
	return szDest;
}

/*************************************************************************
 * PathCombineAW	 [SHELL32.37]
 */
LPVOID WINAPI PathCombineAW(
	LPVOID szDest,
	LPCVOID lpszDir,
	LPCVOID lpszFile) 
{
	if (VERSION_OsIsUnicode())
	  return PathCombineW( szDest, lpszDir, lpszFile );
	return PathCombineA( szDest, lpszDir, lpszFile );
}

/*************************************************************************
 * PathAddBackslashA	[SHLWAPI.@]
 *
 * NOTES
 *     append \ if there is none
 */
LPSTR WINAPI PathAddBackslashA(LPSTR lpszPath)
{
	int len;
	TRACE("%p->%s\n",lpszPath,lpszPath);

	len = strlen(lpszPath);
	if (len && lpszPath[len-1]!='\\') 
	{
	  lpszPath[len]  = '\\';
	  lpszPath[len+1]= 0x00;
	  return lpszPath+len+1;
	}
	return lpszPath+len;
}

/*************************************************************************
 * PathAddBackslashW	[SHLWAPI.@]
 */
LPWSTR WINAPI PathAddBackslashW(LPWSTR lpszPath)
{
	int len;
	TRACE("%p->%s\n",lpszPath,debugstr_w(lpszPath));

	len = strlenW(lpszPath);
	if (len && lpszPath[len-1]!=(WCHAR)'\\') 
	{
	  lpszPath[len]  = (WCHAR)'\\';
	  lpszPath[len+1]= 0x00;
	  return lpszPath+len+1;
	}
	return lpszPath+len;
}

/*************************************************************************
 * PathAddBackslashAW		[SHELL32.32]
 */
LPVOID WINAPI PathAddBackslashAW(LPVOID lpszPath)
{
	if(VERSION_OsIsUnicode())
	  return PathAddBackslashW(lpszPath);
	return PathAddBackslashA(lpszPath);
}

/*************************************************************************
 * PathBuildRootA		[SHLWAPI.@]
 */
LPSTR WINAPI PathBuildRootA(LPSTR lpszPath, int drive) 
{
	TRACE("%p %i\n",lpszPath, drive);

	strcpy(lpszPath,"A:\\");
	lpszPath[0]+=drive;
	return lpszPath;
}

/*************************************************************************
 * PathBuildRootW		[SHLWAPI.@]
 */
LPWSTR WINAPI PathBuildRootW(LPWSTR lpszPath, int drive) 
{
	TRACE("%p %i\n",debugstr_w(lpszPath), drive);

	lstrcpyAtoW(lpszPath,"A:\\");
	lpszPath[0]+=drive;
	return lpszPath;
}

/*************************************************************************
 * PathBuildRootAW		[SHELL32.30]
 */
LPVOID WINAPI PathBuildRootAW(LPVOID lpszPath, int drive)
{
	if(VERSION_OsIsUnicode())
	  return PathBuildRootW(lpszPath, drive);
	return PathBuildRootA(lpszPath, drive);
}

/*
	Extracting Component Parts
*/

/*************************************************************************
 * PathFindFileNameA	[SHLWAPI.@]
 */
LPSTR WINAPI PathFindFileNameA(LPCSTR lpszPath)
{
	LPCSTR lastSlash = lpszPath;

	TRACE("%s\n",lpszPath);
	while (*lpszPath) 
	{
	  if ( isSlash(lpszPath[0]) && lpszPath[1])
	      lastSlash = lpszPath+1;
	  lpszPath = CharNextA(lpszPath);
	}
	return (LPSTR)lastSlash;

}

/*************************************************************************
 * PathFindFileNameW	[SHLWAPI.@]
 */
LPWSTR WINAPI PathFindFileNameW(LPCWSTR lpszPath)
{
	LPCWSTR wslash;
	wslash = lpszPath;

	TRACE("%s\n",debugstr_w(wslash));
	while (lpszPath[0]) 
	{
	  if (((lpszPath[0]=='\\') || (lpszPath[0]==':')) && lpszPath[1] && lpszPath[1]!='\\')
	    wslash = lpszPath+1;
	  lpszPath = CharNextW(lpszPath);
	}
	return (LPWSTR)wslash;	
}

/*************************************************************************
 * PathFindFileNameAW	[SHELL32.34]
 */
LPVOID WINAPI PathFindFileNameAW(LPCVOID lpszPath)
{
	if(VERSION_OsIsUnicode())
	  return PathFindFileNameW(lpszPath);
	return PathFindFileNameA(lpszPath);
}

/*************************************************************************
 * PathFindExtensionA	[SHLWAPI.@]
 *
 * NOTES
 *     returns pointer to last . in last lpszPath component or at \0.
 */

LPSTR WINAPI PathFindExtensionA(LPCSTR lpszPath) 
{
	LPCSTR   lastpoint = NULL;

	TRACE("%p %s\n",lpszPath,lpszPath);

	while (*lpszPath) 
	{
	  if (*lpszPath=='\\'||*lpszPath==' ')
	    lastpoint=NULL;
	  if (*lpszPath=='.')
	    lastpoint=lpszPath;
	  lpszPath = CharNextA(lpszPath);
	}
	return (LPSTR)(lastpoint?lastpoint:lpszPath);
}

/*************************************************************************
 * PathFindExtensionW	[SHLWAPI.@]
 */
LPWSTR WINAPI PathFindExtensionW(LPCWSTR lpszPath) 
{
	LPCWSTR   lastpoint = NULL;

	TRACE("(%p %s)\n",lpszPath,debugstr_w(lpszPath));

	while (*lpszPath)
	{
	  if (*lpszPath==(WCHAR)'\\'||*lpszPath==(WCHAR)' ')
	    lastpoint=NULL;
	  if (*lpszPath==(WCHAR)'.')
	    lastpoint=lpszPath;
	  lpszPath = CharNextW(lpszPath);
	}
	return (LPWSTR)(lastpoint?lastpoint:lpszPath);
}

/*************************************************************************
 * PathFindExtensionAW		[SHELL32.31]
 */
LPVOID WINAPI PathFindExtensionAW(LPCVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathFindExtensionW(lpszPath);
	return PathFindExtensionA(lpszPath);

}

/*************************************************************************
 * PathGetExtensionA		[internal]
 *
 * NOTES
 *  exported by ordinal
 *  return value points to the first char after the dot
 */
LPSTR WINAPI PathGetExtensionA(LPCSTR lpszPath)
{
	TRACE("(%s)\n",lpszPath);

	lpszPath = PathFindExtensionA(lpszPath);
	return (LPSTR)(*lpszPath?(lpszPath+1):lpszPath);
}

/*************************************************************************
 * PathGetExtensionW		[internal]
 */
LPWSTR WINAPI PathGetExtensionW(LPCWSTR lpszPath)
{
	TRACE("(%s)\n",debugstr_w(lpszPath));

	lpszPath = PathFindExtensionW(lpszPath);
	return (LPWSTR)(*lpszPath?(lpszPath+1):lpszPath);
}

/*************************************************************************
 * PathGetExtensionAW		[SHELL32.158]
 */
LPVOID WINAPI PathGetExtensionAW(LPCVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathGetExtensionW(lpszPath);
	return PathGetExtensionA(lpszPath);
}

/*************************************************************************
 * PathGetArgsA		[SHLWAPI.@]
 *
 * NOTES
 *     look for next arg in string. handle "quoted" strings
 *     returns pointer to argument *AFTER* the space. Or to the \0.
 *
 * FIXME
 *     quoting by '\'
 */
LPSTR WINAPI PathGetArgsA(LPCSTR lpszPath) 
{
	BOOL	qflag = FALSE;

	TRACE("%s\n",lpszPath);

	while (*lpszPath) 
	{
	  if ((*lpszPath==' ') && !qflag)
	    return (LPSTR)lpszPath+1;
	  if (*lpszPath=='"')
	    qflag=!qflag;
	  lpszPath = CharNextA(lpszPath);
	}
	return (LPSTR)lpszPath;
}

/*************************************************************************
 * PathGetArgsW		[SHLWAPI.@]
 */
LPWSTR WINAPI PathGetArgsW(LPCWSTR lpszPath) 
{
	BOOL	qflag = FALSE;

	TRACE("%s\n",debugstr_w(lpszPath));

	while (*lpszPath) 
	{
	  if ((*lpszPath==' ') && !qflag)
	    return (LPWSTR)lpszPath+1;
	  if (*lpszPath=='"')
	    qflag=!qflag;
	  lpszPath = CharNextW(lpszPath);
	}
	return (LPWSTR)lpszPath;
}

/*************************************************************************
 * PathGetArgsAW	[SHELL32.52]
 */
LPVOID WINAPI PathGetArgsAW(LPVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathGetArgsW(lpszPath);
	return PathGetArgsA(lpszPath);
}

/*************************************************************************
 * PathGetDriveNumberA	[SHLWAPI.@]
 */
int WINAPI PathGetDriveNumberA(LPCSTR lpszPath)
{
	int chr = tolower(lpszPath[0]);
	
	TRACE ("%s\n",debugstr_a(lpszPath));

	if (!lpszPath || lpszPath[1]!=':' || chr < 'a' || chr > 'z') return -1;
	return tolower(lpszPath[0]) - 'a' ;
}

/*************************************************************************
 * PathGetDriveNumberW	[SHLWAPI.@]
 */
int WINAPI PathGetDriveNumberW(LPCWSTR lpszPath)
{
	int chr = tolowerW(lpszPath[0]);
	
	TRACE ("%s\n",debugstr_w(lpszPath));

	if (!lpszPath || lpszPath[1]!=':' || chr < 'a' || chr > 'z') return -1;
	return tolowerW(lpszPath[0]) - 'a' ;
}

/*************************************************************************
 * PathGetDriveNumber	[SHELL32.57]
 */
int WINAPI PathGetDriveNumberAW(LPVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathGetDriveNumberW(lpszPath);
	return PathGetDriveNumberA(lpszPath);
}

/*************************************************************************
 * PathRemoveFileSpecA	[SHLWAPI.@]
 * 
 * NOTES
 *     truncates passed argument to a valid path
 *     returns if the string was modified or not.
 *     "\foo\xx\foo"-> "\foo\xx"
 *     "\" -> "\"
 *     "a:\foo"	-> "a:\"
 */
BOOL WINAPI PathRemoveFileSpecA(LPSTR lpszPath)
{
	LPSTR cutplace = lpszPath;
	BOOL ret = FALSE;
	
	TRACE("%s\n",lpszPath);

	if(lpszPath)
	{
	  while (*lpszPath == '\\') cutplace = ++lpszPath;
	  
	  while (*lpszPath)
	  {
	    if(lpszPath[0] == '\\') cutplace = lpszPath;
	  
	    if(lpszPath[0] == ':')
	    {
	      cutplace = lpszPath + 1;
	      if (lpszPath[1] == '\\') cutplace++;
	      lpszPath++;
	    }
	    lpszPath = CharNextA(lpszPath);
	    if (!lpszPath) break;
	  }
	  
	  ret = (*cutplace!='\0');
	  *cutplace = '\0';
	}
	return ret;
}

/*************************************************************************
 * PathRemoveFileSpecW	[SHLWAPI.@]
 */
BOOL WINAPI PathRemoveFileSpecW(LPWSTR lpszPath)
{
	LPWSTR cutplace = lpszPath;
	BOOL ret = FALSE;

	TRACE("%s\n",debugstr_w(lpszPath));

	if(lpszPath)
	{
	  while (*lpszPath == '\\') cutplace = ++lpszPath;
	  
	  while (*lpszPath)
	  {
	    if(lpszPath[0] == '\\') cutplace = lpszPath;
	  
	    if(lpszPath[0] == ':')
	    {
	      cutplace = lpszPath + 1;
	      if (lpszPath[1] == '\\') cutplace++;
	      lpszPath++;
	    }
	    lpszPath = CharNextW(lpszPath);
	    if (!lpszPath) break;
	  }
	  
	  ret = (*cutplace!='\0');
	  *cutplace = '\0';
	}
	return ret;
}

/*************************************************************************
 * PathRemoveFileSpec [SHELL32.35]
 */
BOOL WINAPI PathRemoveFileSpecAW(LPVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathRemoveFileSpecW(lpszPath);
	return PathRemoveFileSpecA(lpszPath);
}

/*************************************************************************
 * PathStripPathA	[SHELLWAPI.@]
 * 
 * NOTES
 *  removes the path from the beginning of a filename
 */
void WINAPI PathStripPathA(LPSTR lpszPath)
{
	LPSTR lpszFileName = PathFindFileNameA(lpszPath);

	TRACE("%s\n", lpszPath);

	if(lpszFileName)
	  RtlMoveMemory(lpszPath, lpszFileName, strlen(lpszFileName)+1); 
}

/*************************************************************************
 * PathStripPathW	[SHELLWAPI.@]
 */
void WINAPI PathStripPathW(LPWSTR lpszPath)
{
	LPWSTR lpszFileName = PathFindFileNameW(lpszPath);

	TRACE("%s\n", debugstr_w(lpszPath));
	if(lpszFileName)
	  RtlMoveMemory(lpszPath, lpszFileName, (lstrlenW(lpszFileName)+1)*sizeof(WCHAR)); 
}

/*************************************************************************
 * PathStripPathAW	[SHELL32.38]
 */
void WINAPI PathStripPathAW(LPVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathStripPathW(lpszPath);
	return PathStripPathA(lpszPath);
}

/*************************************************************************
 * PathStripToRootA	[SHLWAPI.@]
 */
BOOL WINAPI PathStripToRootA(LPSTR lpszPath)
{
	TRACE("%s\n", lpszPath);

	if (!lpszPath) return FALSE;
	while(!PathIsRootA(lpszPath))
	  if (!PathRemoveFileSpecA(lpszPath)) return FALSE;
	return TRUE;
}

/*************************************************************************
 * PathStripToRootW	[SHLWAPI.@]
 */
BOOL WINAPI PathStripToRootW(LPWSTR lpszPath)
{
	TRACE("%s\n", debugstr_w(lpszPath));

	if (!lpszPath) return FALSE;
	while(!PathIsRootW(lpszPath))
	  if (!PathRemoveFileSpecW(lpszPath)) return FALSE;
	return TRUE;
}

/*************************************************************************
 * PathStripToRootAW	[SHELL32.50]
 */
BOOL WINAPI PathStripToRootAW(LPVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathStripToRootW(lpszPath);
	return PathStripToRootA(lpszPath);
}

/*************************************************************************
 * PathRemoveArgsA	[SHLWAPI.@]
 *
 */
void WINAPI PathRemoveArgsA(LPSTR lpszPath)
{
	TRACE("%s\n",lpszPath);
	
	if(lpszPath)
	{
	  LPSTR lpszArgs = PathGetArgsA(lpszPath);
	  if (!*lpszArgs)
	  {
	    LPSTR lpszLastChar = CharPrevA(lpszPath, lpszArgs);
	    if(*lpszLastChar==' ') *lpszLastChar = '\0';
	  }
	}
}

/*************************************************************************
 * PathRemoveArgsW	[SHLWAPI.@]
 */
void WINAPI PathRemoveArgsW(LPWSTR lpszPath)
{
	TRACE("%s\n", debugstr_w(lpszPath));

	if(lpszPath)
	{
	  LPWSTR lpszArgs = PathGetArgsW(lpszPath);
	  if (!*lpszArgs)
	  {
	    LPWSTR lpszLastChar = CharPrevW(lpszPath, lpszArgs);
	    if(*lpszLastChar==' ') *lpszLastChar = '\0';
	  }
	}
}

/*************************************************************************
 * PathRemoveArgsAW	[SHELL32.251]
 */
void WINAPI PathRemoveArgsAW(LPVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  PathRemoveArgsW(lpszPath);
	PathRemoveArgsA(lpszPath);
}

/*************************************************************************
 * PathRemoveExtensionA		[SHLWAPI.@]
 */
void WINAPI PathRemoveExtensionA(LPSTR lpszPath)
{
	LPSTR lpszExtension = PathFindExtensionA(lpszPath);

	TRACE("%s\n", lpszPath);

	if (lpszExtension) *lpszExtension='\0';
}

/*************************************************************************
 * PathRemoveExtensionW		[SHLWAPI.@]
 */
void WINAPI PathRemoveExtensionW(LPWSTR lpszPath)
{
	LPWSTR lpszExtension = PathFindExtensionW(lpszPath);

	TRACE("%s\n", debugstr_w(lpszPath));

	if (lpszExtension) *lpszExtension='\0';
}

/*************************************************************************
 * PathRemoveExtensionAW	[SHELL32.250]
 */
void WINAPI PathRemoveExtensionAW(LPVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathRemoveExtensionW(lpszPath);
	return PathRemoveExtensionA(lpszPath);
}

/*************************************************************************
 * PathRemoveBackslashA	[SHLWAPI.@]
 *
 * If the path ends in a backslash it is replaced by a NULL
 * and the address of the NULL is returned
 * Otherwise 
 * the address of the last character is returned.
 *
 * FIXME
 *  "c:\": keep backslash
 */
LPSTR WINAPI PathRemoveBackslashA( LPSTR lpszPath )
{
	int len;
	LPSTR szTemp = NULL;
	
	if(lpszPath)
	{
	  len = strlen(lpszPath);
	  szTemp = CharPrevA(lpszPath, lpszPath+len);
	  if (! PathIsRootA(lpszPath))
	  {
	    if (*szTemp == '\\') *szTemp = '\0';
	  }
	}
	return szTemp;
}

/*************************************************************************
 * PathRemoveBackslashW	[SHLWAPI.@]
 */
LPWSTR WINAPI PathRemoveBackslashW( LPWSTR lpszPath )
{
	int len;
	LPWSTR szTemp = NULL;
	
	if(lpszPath)
	{
	  len = lstrlenW(lpszPath);
	  szTemp = CharPrevW(lpszPath, lpszPath+len);
	  if (! PathIsRootW(lpszPath))
	  {
	    if (*szTemp == '\\') *szTemp = '\0';
	  }
	}
	return szTemp;
}

/*
	Path Manipulations
*/

/*************************************************************************
 * PathGetShortPathA [internal]
 */
LPSTR WINAPI PathGetShortPathA(LPSTR lpszPath)
{
	FIXME("%s stub\n", lpszPath);
	return NULL;
}

/*************************************************************************
 * PathGetShortPathW [internal]
 */
LPWSTR WINAPI PathGetShortPathW(LPWSTR lpszPath)
{
	FIXME("%s stub\n", debugstr_w(lpszPath));
	return NULL;
}

/*************************************************************************
 * PathGetShortPathAW [SHELL32.92]
 */
LPVOID WINAPI PathGetShortPathAW(LPVOID lpszPath)
{
	if(VERSION_OsIsUnicode())
	  return PathGetShortPathW(lpszPath);
	return PathGetShortPathA(lpszPath);
}

/*************************************************************************
 * PathRemoveBlanksA [SHLWAPI.@]
 * 
 * NOTES
 *     remove spaces from beginning and end of passed string
 */
void WINAPI PathRemoveBlanksA(LPSTR str)
{
	LPSTR x = str;

	TRACE("%s\n",str);

	if(str)
	{
	  while (*x==' ') x = CharNextA(x);
	  if (x!=str) strcpy(str,x);
	  x=str+strlen(str)-1;
	  while (*x==' ') x = CharPrevA(str, x);
	  if (*x==' ') *x='\0';
	}
}

/*************************************************************************
 * PathRemoveBlanksW [SHLWAPI.@]
 */
void WINAPI PathRemoveBlanksW(LPWSTR str)
{
	LPWSTR x = str;

	TRACE("%s\n",debugstr_w(str));

	if(str)
	{
	  while (*x==' ') x = CharNextW(x);
	  if (x!=str) lstrcpyW(str,x);
	  x=str+lstrlenW(str)-1;
	  while (*x==' ') x = CharPrevW(str, x);
	  if (*x==' ') *x='\0';
	}
}

/*************************************************************************
 * PathRemoveBlanksAW [SHELL32.33]
 */
void WINAPI PathRemoveBlanksAW(LPVOID str)
{
	if(VERSION_OsIsUnicode())
	  PathRemoveBlanksW(str);
	PathRemoveBlanksA(str);
}

/*************************************************************************
 * PathQuoteSpacesA [SHLWAPI.@]
 * 
 */
LPSTR WINAPI PathQuoteSpacesA(LPSTR lpszPath)
{
	TRACE("%s\n",lpszPath);

	if(StrChrA(lpszPath,' '))
	{
	  int len = strlen(lpszPath);
	  RtlMoveMemory(lpszPath+1, lpszPath, len);
	  *(lpszPath++) = '"';
	  lpszPath += len;
	  *(lpszPath++) = '"';
	  *(lpszPath) = '\0';
	  return --lpszPath;
	}
	return 0;
}

/*************************************************************************
 * PathQuoteSpacesW [SHLWAPI.@]
 */
LPWSTR WINAPI PathQuoteSpacesW(LPWSTR lpszPath)
{
	TRACE("%s\n",debugstr_w(lpszPath));

	if(StrChrW(lpszPath,' '))
	{
	  int len = lstrlenW(lpszPath);
	  RtlMoveMemory(lpszPath+1, lpszPath, len*sizeof(WCHAR));
	  *(lpszPath++) = '"';
	  lpszPath += len;
	  *(lpszPath++) = '"';
	  *(lpszPath) = '\0';
	  return --lpszPath;
	}
	return 0;
}

/*************************************************************************
 * PathQuoteSpacesAW [SHELL32.55]
 */
LPVOID WINAPI PathQuoteSpacesAW (LPVOID lpszPath)
{
	if(VERSION_OsIsUnicode())
	  return PathQuoteSpacesW(lpszPath);
	return PathQuoteSpacesA(lpszPath);
}

/*************************************************************************
 * PathUnquoteSpacesA [SHLWAPI.@]
 * 
 * NOTES
 *     unquote string (remove ")
 */
VOID WINAPI PathUnquoteSpacesA(LPSTR str) 
{
	DWORD len = lstrlenA(str);

	TRACE("%s\n",str);

	if (*str!='"')
	  return;
	if (str[len-1]!='"')
	  return;
	str[len-1]='\0';
	lstrcpyA(str,str+1);
	return;
}

/*************************************************************************
 * PathUnquoteSpacesW [SHLWAPI.@]
 */
VOID WINAPI PathUnquoteSpacesW(LPWSTR str) 
{
	DWORD len = strlenW(str);

	TRACE("%s\n",debugstr_w(str));

	if (*str!='"')
	  return;
	if (str[len-1]!='"')
	  return;
	str[len-1]='\0';
	strcpyW(str,str+1);
	return;
}

/*************************************************************************
 * PathUnquoteSpacesAW [SHELL32.56]
 */
VOID WINAPI PathUnquoteSpacesAW(LPVOID str) 
{
	if(VERSION_OsIsUnicode())
	  PathUnquoteSpacesW(str);
	else
	  PathUnquoteSpacesA(str);
}

/*************************************************************************
 * PathParseIconLocationA	[SHLWAPI.@]
 */
int WINAPI PathParseIconLocationA(LPSTR lpszPath)
{
	LPSTR lpstrComma = strchr(lpszPath, ',');
	
	FIXME("%s stub\n", debugstr_a(lpszPath));

	if (lpstrComma && lpstrComma[1])
	{
	  lpstrComma[0]='\0';
/*	  return atoi(&lpstrComma[1]);	FIXME */
	}
	
	PathUnquoteSpacesA(lpszPath);
	return 0;
}

/*************************************************************************
 * PathParseIconLocationW	[SHLWAPI.@]
 */
int WINAPI PathParseIconLocationW(LPWSTR lpszPath)
{
	LPWSTR lpstrComma = strchrW(lpszPath, ',');
	
	FIXME("%s stub\n", debugstr_w(lpszPath));

	if (lpstrComma && lpstrComma[1])
	{
	  lpstrComma[0]='\0';
/*	  return _wtoi(&lpstrComma[1]);	FIXME */
	}
	PathUnquoteSpacesW(lpszPath);
	return 0;
}

/*************************************************************************
 * PathParseIconLocationAW	[SHELL32.249]
 */
int WINAPI PathParseIconLocationAW (LPVOID lpszPath)
{
	if(VERSION_OsIsUnicode())
	  return PathParseIconLocationW(lpszPath);
	return PathParseIconLocationA(lpszPath);
}

/*
	########## Path Testing ##########
*/
/*************************************************************************
 * PathIsUNCA		[SHLWAPI.@]
 * 
 * NOTES
 *     PathIsUNC(char*path);
 */
BOOL WINAPI PathIsUNCA(LPCSTR lpszPath) 
{
	TRACE("%s\n",lpszPath);

	return (lpszPath && (lpszPath[0]=='\\') && (lpszPath[1]=='\\'));
}

/*************************************************************************
 * PathIsUNCW		[SHLWAPI.@]
 */
BOOL WINAPI PathIsUNCW(LPCWSTR lpszPath) 
{
	TRACE("%s\n",debugstr_w(lpszPath));

	return (lpszPath && (lpszPath[0]=='\\') && (lpszPath[1]=='\\'));
}

/*************************************************************************
 * PathIsUNCAW		[SHELL32.39]
 */
BOOL WINAPI PathIsUNCAW (LPCVOID lpszPath)
{
	if (VERSION_OsIsUnicode())
	  return PathIsUNCW( lpszPath );
	return PathIsUNCA( lpszPath );  
}

/*************************************************************************
 *  PathIsRelativeA	[SHLWAPI.@]
 */
BOOL WINAPI PathIsRelativeA (LPCSTR lpszPath)
{
	TRACE("lpszPath=%s\n",lpszPath);

	return (lpszPath && (lpszPath[0]!='\\' && lpszPath[1]!=':'));
}

/*************************************************************************
 *  PathIsRelativeW	[SHLWAPI.@]
 */
BOOL WINAPI PathIsRelativeW (LPCWSTR lpszPath)
{
	TRACE("lpszPath=%s\n",debugstr_w(lpszPath));

	return (lpszPath && (lpszPath[0]!='\\' && lpszPath[1]!=':'));
}

/*************************************************************************
 *  PathIsRelativeAW	[SHELL32.40]
 */
BOOL WINAPI PathIsRelativeAW (LPCVOID lpszPath)
{
	if (VERSION_OsIsUnicode())
	  return PathIsRelativeW( lpszPath );
	return PathIsRelativeA( lpszPath );  
}

/*************************************************************************
 * PathIsRootA		[SHLWAPI.@]
 *
 * notes
 *  TRUE if the path points to a root directory
 */
BOOL WINAPI PathIsRootA(LPCSTR lpszPath)
{
	TRACE("%s\n",lpszPath);

	/* X:\ */
	if (lpszPath[1]==':' && lpszPath[2]=='\\' && lpszPath[3]=='\0')
	  return TRUE;

	/* "\" */
	if (lpszPath[0]=='\\' && lpszPath[1]=='\0')
	  return TRUE;

	/* UNC "\\<computer>\<share>" */
	if (lpszPath[0]=='\\' && lpszPath[1]=='\\')		
	{
	  int foundbackslash = 0;
	  lpszPath += 2;
	  while (*lpszPath)
	  {
	    if (*lpszPath=='\\') foundbackslash++;
	    lpszPath = CharNextA(lpszPath);
	  }
	  if (foundbackslash <= 1)
	    return TRUE;
	}
	return FALSE;
}

/*************************************************************************
 * PathIsRootW		[SHLWAPI.@]
 */
BOOL WINAPI PathIsRootW(LPCWSTR lpszPath) 
{
	TRACE("%s\n",debugstr_w(lpszPath));

	/* X:\ */
	if (lpszPath[1]==':' && lpszPath[2]=='\\' && lpszPath[3]=='\0')
	  return TRUE;

	/* "\" */
	if (lpszPath[0]=='\\' && lpszPath[1]=='\0')
	  return TRUE;

	/* UNC "\\<computer>\<share>" */
	if (lpszPath[0]=='\\' && lpszPath[1]=='\\')		
	{
	  int foundbackslash = 0;
	  lpszPath += 2;
	  while (*lpszPath)
	  {
	    if (*lpszPath=='\\') foundbackslash++;
	    lpszPath = CharNextW(lpszPath);
	  }
	  if (foundbackslash <= 1)
	    return TRUE;
	}
	return FALSE;

}

/*************************************************************************
 * PathIsRootAW		[SHELL32.29]
 */
BOOL WINAPI PathIsRootAW(LPCVOID lpszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathIsRootW(lpszPath);
	return PathIsRootA(lpszPath);
}

/*************************************************************************
 *  PathIsExeA		[internal]
 */
BOOL WINAPI PathIsExeA (LPCSTR lpszPath)
{
	LPCSTR lpszExtension = PathGetExtensionA(lpszPath);
	int i = 0;
	static char * lpszExtensions[6] = {"exe", "com", "pid", "cmd", "bat", NULL };
	
	TRACE("path=%s\n",lpszPath);

	for(i=0; lpszExtensions[i]; i++)
	  if (!strcasecmp(lpszExtension,lpszExtensions[i])) return TRUE;
	  
	return FALSE;
}

/*************************************************************************
 *  PathIsExeW		[internal]
 */
BOOL WINAPI PathIsExeW (LPCWSTR lpszPath)
{
	LPCWSTR lpszExtension = PathGetExtensionW(lpszPath);
	int i = 0;
	static WCHAR lpszExtensions[6][4] =
	  {{'e','x','e','\0'}, {'c','o','m','\0'}, {'p','i','d','\0'},
	   {'c','m','d','\0'}, {'b','a','t','\0'}, {'\0'} };
	
	TRACE("path=%s\n",debugstr_w(lpszPath));

	for(i=0; lpszExtensions[i]; i++)
	  if (!strcmpiW(lpszExtension,lpszExtensions[i])) return TRUE;
	  
	return FALSE;
}

/*************************************************************************
 *  PathIsExeAW		[SHELL32.43]
 */
BOOL WINAPI PathIsExeAW (LPCVOID path)
{
	if (VERSION_OsIsUnicode())
	  return PathIsExeW (path);
	return PathIsExeA(path);
}

/*************************************************************************
 * PathIsDirectoryA	[SHLWAPI.@]
 */
BOOL WINAPI PathIsDirectoryA(LPCSTR lpszPath)
{
	DWORD dwAttr;

	TRACE("%s\n", debugstr_a(lpszPath));

	dwAttr = GetFileAttributesA(lpszPath);
	return  (dwAttr != -1) ? dwAttr & FILE_ATTRIBUTE_DIRECTORY : 0;
}

/*************************************************************************
 * PathIsDirectoryW	[SHLWAPI.@]
 */
BOOL WINAPI PathIsDirectoryW(LPCWSTR lpszPath)
{
	DWORD dwAttr;
	
	TRACE("%s\n", debugstr_w(lpszPath));

	dwAttr = GetFileAttributesW(lpszPath);
	return  (dwAttr != -1) ? dwAttr & FILE_ATTRIBUTE_DIRECTORY : 0;
}

/*************************************************************************
 * PathIsDirectoryAW	[SHELL32.159]
 */
BOOL WINAPI PathIsDirectoryAW (LPCVOID lpszPath)
{
	if (VERSION_OsIsUnicode())
	  return PathIsDirectoryW (lpszPath);
	return PathIsDirectoryA (lpszPath);
}

/*************************************************************************
 * PathFileExistsA	[SHLWAPI.@]
 * 
 * NOTES
 *     file_exists(char *fn);
 */
BOOL WINAPI PathFileExistsA(LPCSTR lpszPath) 
{
	TRACE("%s\n",lpszPath);
	return  (GetFileAttributesA(lpszPath)!=-1);
}

/*************************************************************************
 * PathFileExistsW	[SHLWAPI.@]
 */
BOOL WINAPI PathFileExistsW(LPCWSTR lpszPath) 
{
	TRACE("%s\n",debugstr_w(lpszPath));
	return  (GetFileAttributesW(lpszPath)!=-1);
}

/*************************************************************************
 * PathFileExistsAW	[SHELL32.45]
 */ 
BOOL WINAPI PathFileExistsAW (LPCVOID lpszPath)
{
	if (VERSION_OsIsUnicode())
	  return PathFileExistsW (lpszPath);
	return PathFileExistsA (lpszPath);
}

/*************************************************************************
 * PathMatchSingleMaskA	[internal]
 * 
 * NOTES
 *     internal (used by PathMatchSpec)
 */
static BOOL PathMatchSingleMaskA(LPCSTR name, LPCSTR mask)
{
	while (*name && *mask && *mask!=';') 
	{
	  if (*mask=='*') 
	  {
	    do 
	    {
	      if (PathMatchSingleMaskA(name,mask+1)) return 1;  /* try substrings */
	    } while (*name++);
	    return 0;
	  }
	  if (toupper(*mask)!=toupper(*name) && *mask!='?') return 0;
	  name = CharNextA(name);
	  mask = CharNextA(mask);
	}
	if (!*name) 
	{
	  while (*mask=='*') mask++;
	  if (!*mask || *mask==';') return 1;
	}
	return 0;
}

/*************************************************************************
 * PathMatchSingleMaskW	[internal]
 */
static BOOL PathMatchSingleMaskW(LPCWSTR name, LPCWSTR mask)
{
	while (*name && *mask && *mask!=';') 
	{
	  if (*mask=='*') 
	  {
	    do 
	    {
	      if (PathMatchSingleMaskW(name,mask+1)) return 1;  /* try substrings */
	    } while (*name++);
	    return 0;
	  }
	  if (toupperW(*mask)!=toupperW(*name) && *mask!='?') return 0;
	  name = CharNextW(name);
	  mask = CharNextW(mask);
	}
	if (!*name) 
	{
	  while (*mask=='*') mask++;
	  if (!*mask || *mask==';') return 1;
	}
	return 0;
}
/*************************************************************************
 * PathMatchSpecA	[SHLWAPI.@]
 * 
 * NOTES
 *     used from COMDLG32
 */
BOOL WINAPI PathMatchSpecA(LPCSTR name, LPCSTR mask) 
{
	TRACE("%s %s\n",name,mask);

	if (!lstrcmpA( mask, "*.*" )) return 1;   /* we don't require a period */

	while (*mask) 
	{
	  if (PathMatchSingleMaskA(name,mask)) return 1;    /* helper function */
	  while (*mask && *mask!=';') mask = CharNextA(mask);
	  if (*mask==';') 
	  {
	    mask++;
	    while (*mask==' ') mask++;      /*  masks may be separated by "; " */
	  }
	}
	return 0;
}

/*************************************************************************
 * PathMatchSpecW	[SHLWAPI.@]
 */
BOOL WINAPI PathMatchSpecW(LPCWSTR name, LPCWSTR mask) 
{
	WCHAR stemp[4];
	TRACE("%s %s\n",debugstr_w(name),debugstr_w(mask));

	lstrcpyAtoW(stemp,"*.*");	
	if (!lstrcmpW( mask, stemp )) return 1;   /* we don't require a period */

	while (*mask) 
	{
	  if (PathMatchSingleMaskW(name,mask)) return 1;    /* helper function */
	  while (*mask && *mask!=';') mask = CharNextW(mask);
	  if (*mask==';') 
	  {
	    mask++;
	    while (*mask==' ') mask++;       /* masks may be separated by "; " */
	  }
	}
	return 0;
}

/*************************************************************************
 * PathMatchSpecAW	[SHELL32.46]
 */
BOOL WINAPI PathMatchSpecAW(LPVOID name, LPVOID mask) 
{
	if (VERSION_OsIsUnicode())
	  return PathMatchSpecW( name, mask );
	return PathMatchSpecA( name, mask );
}

/*************************************************************************
 * PathIsSameRootA	[SHLWAPI.@]
 *
 * FIXME
 *  what to do with "\path" ??
 */
BOOL WINAPI PathIsSameRootA(LPCSTR lpszPath1, LPCSTR lpszPath2)
{
	TRACE("%s %s\n", lpszPath1, lpszPath2);
	
	if (PathIsRelativeA(lpszPath1) || PathIsRelativeA(lpszPath2)) return FALSE;

	/* usual path */
	if ( toupper(lpszPath1[0])==toupper(lpszPath2[0]) &&
	     lpszPath1[1]==':' && lpszPath2[1]==':' &&
	     lpszPath1[2]=='\\' && lpszPath2[2]=='\\')
	  return TRUE;

	/* UNC */
	if (lpszPath1[0]=='\\' && lpszPath2[0]=='\\' &&
	    lpszPath1[1]=='\\' && lpszPath2[1]=='\\')
	{
	  int pos=2, bsfound=0;
	  while (lpszPath1[pos] && lpszPath2[pos] &&
	        (lpszPath1[pos] == lpszPath2[pos]))
	  {
	    if (lpszPath1[pos]=='\\') bsfound++;
	    if (bsfound == 2) return TRUE;
	    pos++; /* fixme: use CharNext*/
	  }
	  return (lpszPath1[pos] == lpszPath2[pos]);
	}
	return FALSE;
}

/*************************************************************************
 * PathIsSameRootW	[SHLWAPI.@]
 */
BOOL WINAPI PathIsSameRootW(LPCWSTR lpszPath1, LPCWSTR lpszPath2)
{
	TRACE("%s %s\n", debugstr_w(lpszPath1), debugstr_w(lpszPath2));
	
	if (PathIsRelativeW(lpszPath1) || PathIsRelativeW(lpszPath2)) return FALSE;

	/* usual path */
	if ( toupperW(lpszPath1[0])==toupperW(lpszPath2[0]) &&
	     lpszPath1[1]==':' && lpszPath2[1]==':' &&
	     lpszPath1[2]=='\\' && lpszPath2[2]=='\\')
	  return TRUE;

	/* UNC */
	if (lpszPath1[0]=='\\' && lpszPath2[0]=='\\' &&
	    lpszPath1[1]=='\\' && lpszPath2[1]=='\\')
	{
	  int pos=2, bsfound=0;
	  while (lpszPath1[pos] && lpszPath2[pos] &&
	        (lpszPath1[pos] == lpszPath2[pos]))
	  {
	    if (lpszPath1[pos]=='\\') bsfound++;
	    if (bsfound == 2) return TRUE;
	    pos++;/* fixme: use CharNext*/
	  }
	  return (lpszPath1[pos] == lpszPath2[pos]);
	}
	return FALSE;
}

/*************************************************************************
 * PathIsSameRootAW	[SHELL32.650]
 */
BOOL WINAPI PathIsSameRootAW(LPCVOID lpszPath1, LPCVOID lpszPath2)
{
	if (VERSION_OsIsUnicode())
	  return PathIsSameRootW(lpszPath1, lpszPath2);
	return PathIsSameRootA(lpszPath1, lpszPath2);
}

/*************************************************************************
 * PathIsURLA
 */
BOOL WINAPI PathIsURLA(LPCSTR lpstrPath)
{
	LPSTR lpstrRes;
	int iSize, i=0;
	static LPSTR SupportedProtocol[] = 
	  {"http","https","ftp","gopher","file","mailto",NULL};

	if(!lpstrPath) return FALSE;

	/* get protocol        */
	lpstrRes = strchr(lpstrPath,':');
	if(!lpstrRes) return FALSE;
	iSize = lpstrRes - lpstrPath;

	while(SupportedProtocol[i])
	{
	  if (iSize == strlen(SupportedProtocol[i]))
	    if(!strncasecmp(lpstrPath, SupportedProtocol[i], iSize))
	      return TRUE;
	  i++;
	}

	return FALSE;
}  

/*************************************************************************
 * PathIsURLW
 */
BOOL WINAPI PathIsURLW(LPCWSTR lpstrPath)
{
	LPWSTR lpstrRes;
	int iSize, i=0;
	static WCHAR SupportedProtocol[7][7] = 
	  {{'h','t','t','p','\0'},{'h','t','t','p','s','\0'},{'f','t','p','\0'},
	  {'g','o','p','h','e','r','\0'},{'f','i','l','e','\0'},
	  {'m','a','i','l','t','o','\0'},{0}};

	if(!lpstrPath) return FALSE;

	/* get protocol        */
	lpstrRes = strchrW(lpstrPath,':');
	if(!lpstrRes) return FALSE;
	iSize = lpstrRes - lpstrPath;

	while(SupportedProtocol[i])
	{
	  if (iSize == strlenW(SupportedProtocol[i]))
	    if(!strncmpiW(lpstrPath, SupportedProtocol[i], iSize))
	      return TRUE;
	  i++;
	}

	return FALSE;
}  

/*************************************************************************
 * IsLFNDriveA		[SHELL32.119]
 * 
 * NOTES
 *     exported by ordinal Name
 */
BOOL WINAPI IsLFNDriveA(LPCSTR lpszPath) 
{
    DWORD	fnlen;

    if (!GetVolumeInformationA(lpszPath,NULL,0,NULL,&fnlen,NULL,NULL,0))
	return FALSE;
    return fnlen>12;
}

/*************************************************************************
 *	PathIsContentTypeA
 */
BOOL WINAPI PathIsContentTypeA(LPCSTR pszPath, LPCSTR pszContentType)
{
	FIXME("%s %s\n", pszPath, pszContentType);
	return FALSE;
}

/*************************************************************************
 *	PathIsContentTypeW
 */
BOOL WINAPI PathIsContentTypeW(LPCWSTR pszPath, LPCWSTR pszContentType)
{
	FIXME("%s %s\n", debugstr_w(pszPath), debugstr_w(pszContentType));
	return FALSE;
}

/*************************************************************************
 *	PathIsFileSpecA
 */
BOOL WINAPI PathIsFileSpecA(LPCSTR pszPath)
{
	FIXME("%s\n", pszPath);
	return FALSE;
}

/*************************************************************************
 *	PathIsFileSpecW
 */
BOOL WINAPI PathIsFileSpecW(LPCWSTR pszPath)
{
	FIXME("%s\n", debugstr_w(pszPath));
	return FALSE;
}

/*************************************************************************
 *	PathIsPrefixA
 */
BOOL WINAPI PathIsPrefixA(LPCSTR pszPrefix, LPCSTR pszPath)
{
	FIXME("%s %s\n", pszPrefix, pszPath);
	return FALSE;
}

/*************************************************************************
 *	PathIsPrefixW
 */
BOOL WINAPI PathIsPrefixW(LPCWSTR pszPrefix, LPCWSTR pszPath)
{
	FIXME("%s %s\n", debugstr_w(pszPrefix), debugstr_w(pszPath));
	return FALSE;
}

/*************************************************************************
 *	PathIsSystemFolderA
 */
BOOL WINAPI PathIsSystemFolderA(LPCSTR pszPath, DWORD dwAttrb)
{
	FIXME("%s 0x%08lx\n", pszPath, dwAttrb);
	return FALSE;
}

/*************************************************************************
 *	PathIsSystemFolderW
 */
BOOL WINAPI PathIsSystemFolderW(LPCWSTR pszPath, DWORD dwAttrb)
{
	FIXME("%s 0x%08lx\n", debugstr_w(pszPath), dwAttrb);
	return FALSE;
}

/*************************************************************************
 *	PathIsUNCServerA
 */
BOOL WINAPI PathIsUNCServerA(
	LPCSTR pszPath)
{
	FIXME("%s\n", pszPath);
	return FALSE;
}

/*************************************************************************
 *	PathIsUNCServerW
 */
BOOL WINAPI PathIsUNCServerW(
	LPCWSTR pszPath)
{
	FIXME("%s\n", debugstr_w(pszPath));
	return FALSE;
}

/*************************************************************************
 *	PathIsUNCServerShareA
 */
BOOL WINAPI PathIsUNCServerShareA(
	LPCSTR pszPath)
{
	FIXME("%s\n", pszPath);
	return FALSE;
}

/*************************************************************************
 *	PathIsUNCServerShareW
 */
BOOL WINAPI PathIsUNCServerShareW(
	LPCWSTR pszPath)
{
	FIXME("%s\n", debugstr_w(pszPath));
	return FALSE;
}

/*
	########## Creating Something Unique ##########
*/
/*************************************************************************
 * PathMakeUniqueNameA	[internal]
 */
BOOL WINAPI PathMakeUniqueNameA(
	LPSTR lpszBuffer,
	DWORD dwBuffSize, 
	LPCSTR lpszShortName,
	LPCSTR lpszLongName,
	LPCSTR lpszPathName)
{
	FIXME("%p %lu %s %s %s stub\n",
	 lpszBuffer, dwBuffSize, debugstr_a(lpszShortName),
	 debugstr_a(lpszLongName), debugstr_a(lpszPathName));
	return TRUE;
}

/*************************************************************************
 * PathMakeUniqueNameW	[internal]
 */
BOOL WINAPI PathMakeUniqueNameW(
	LPWSTR lpszBuffer,
	DWORD dwBuffSize, 
	LPCWSTR lpszShortName,
	LPCWSTR lpszLongName,
	LPCWSTR lpszPathName)
{
	FIXME("%p %lu %s %s %s stub\n",
	 lpszBuffer, dwBuffSize, debugstr_w(lpszShortName),
	 debugstr_w(lpszLongName), debugstr_w(lpszPathName));
	return TRUE;
}

/*************************************************************************
 * PathMakeUniqueNameAW	[SHELL32.47]
 */
BOOL WINAPI PathMakeUniqueNameAW(
	LPVOID lpszBuffer,
	DWORD dwBuffSize, 
	LPCVOID lpszShortName,
	LPCVOID lpszLongName,
	LPCVOID lpszPathName)
{
	if (VERSION_OsIsUnicode())
	  return PathMakeUniqueNameW(lpszBuffer,dwBuffSize, lpszShortName,lpszLongName,lpszPathName);
	return PathMakeUniqueNameA(lpszBuffer,dwBuffSize, lpszShortName,lpszLongName,lpszPathName);
}

/*************************************************************************
 * PathYetAnotherMakeUniqueNameA [SHELL32.75]
 * 
 * NOTES
 *     exported by ordinal
 */
BOOL WINAPI PathYetAnotherMakeUniqueNameA(
	LPSTR lpszBuffer,
	LPCSTR lpszPathName,
	LPCSTR lpszShortName,
	LPCSTR lpszLongName)
{
    FIXME("(%p,%p, %p ,%p):stub.\n",
     lpszBuffer, lpszPathName, lpszShortName, lpszLongName);
    return TRUE;
}


/*
	########## cleaning and resolving paths ##########
 */

/*************************************************************************
 * PathFindOnPathA	[SHELL32.145]
 */
BOOL WINAPI PathFindOnPathA(LPSTR sFile, LPCSTR sOtherDirs)
{
	FIXME("%s %s\n",sFile, sOtherDirs);
	return FALSE;
}

/*************************************************************************
 * PathFindOnPathW	[SHELL32]
 */
BOOL WINAPI PathFindOnPathW(LPWSTR sFile, LPCWSTR sOtherDirs)
{
	FIXME("%s %s\n",debugstr_w(sFile), debugstr_w(sOtherDirs));
	return FALSE;
}

/*************************************************************************
 * PathFindOnPathAW	[SHELL32]
 */
BOOL WINAPI PathFindOnPathAW(LPVOID sFile, LPCVOID sOtherDirs)
{
	if (VERSION_OsIsUnicode())
	  return PathFindOnPathW(sFile, sOtherDirs);
	return PathFindOnPathA(sFile, sOtherDirs);
}

/*************************************************************************
 * PathCleanupSpecA	[SHELL32.171]
 */
DWORD WINAPI PathCleanupSpecA(LPSTR x, LPSTR y)
{
	FIXME("(%p %s, %p %s) stub\n",x,debugstr_a(x),y,debugstr_a(y));
	return TRUE;
}

/*************************************************************************
 * PathCleanupSpecA	[SHELL32]
 */
DWORD WINAPI PathCleanupSpecW(LPWSTR x, LPWSTR y)
{
	FIXME("(%p %s, %p %s) stub\n",x,debugstr_w(x),y,debugstr_w(y));
	return TRUE;
}

/*************************************************************************
 * PathCleanupSpecAW	[SHELL32]
 */
DWORD WINAPI PathCleanupSpecAW (LPVOID x, LPVOID y)
{
	if (VERSION_OsIsUnicode())
	  return PathCleanupSpecW(x,y);
	return PathCleanupSpecA(x,y);
}

/*************************************************************************
 * PathQualifyA		[SHELL32]
 */
BOOL WINAPI PathQualifyA(LPCSTR pszPath) 
{
	FIXME("%s\n",pszPath);
	return 0;
}

/*************************************************************************
 * PathQualifyW		[SHELL32]
 */
BOOL WINAPI PathQualifyW(LPCWSTR pszPath) 
{
	FIXME("%s\n",debugstr_w(pszPath));
	return 0;
}

/*************************************************************************
 * PathQualifyAW	[SHELL32]
 */
BOOL WINAPI PathQualifyAW(LPCVOID pszPath) 
{
	if (VERSION_OsIsUnicode())
	  return PathQualifyW(pszPath);
	return PathQualifyA(pszPath);
}

/*************************************************************************
 * PathResolveA [SHELL32.51]
 */
BOOL WINAPI PathResolveA(
	LPSTR lpszPath,
	LPCSTR *alpszPaths, 
	DWORD dwFlags)
{
	FIXME("(%s,%p,0x%08lx),stub!\n",
	  lpszPath, *alpszPaths, dwFlags);
	return 0;
}

/*************************************************************************
 * PathResolveW [SHELL32]
 */
BOOL WINAPI PathResolveW(
	LPWSTR lpszPath,
	LPCWSTR *alpszPaths, 
	DWORD dwFlags)
{
	FIXME("(%s,%p,0x%08lx),stub!\n",
	  debugstr_w(lpszPath), debugstr_w(*alpszPaths), dwFlags);
	return 0;
}

/*************************************************************************
 * PathResolveAW [SHELL32]
 */
BOOL WINAPI PathResolveAW(
	LPVOID lpszPath,
	LPCVOID *alpszPaths, 
	DWORD dwFlags)
{
	if (VERSION_OsIsUnicode())
	  return PathResolveW(lpszPath, (LPCWSTR*)alpszPaths, dwFlags);
	return PathResolveA(lpszPath, (LPCSTR*)alpszPaths, dwFlags);
}

/*************************************************************************
*	PathProcessCommandA	[SHELL32.653]
*/
HRESULT WINAPI PathProcessCommandA (
	LPCSTR lpszPath,
	LPSTR lpszBuff,
	DWORD dwBuffSize,
	DWORD dwFlags)
{
	FIXME("%s %p 0x%04lx 0x%04lx stub\n",
	lpszPath, lpszBuff, dwBuffSize, dwFlags);
	lstrcpyA(lpszBuff, lpszPath);
	return 0;
}

/*************************************************************************
*	PathProcessCommandW
*/
HRESULT WINAPI PathProcessCommandW (
	LPCWSTR lpszPath,
	LPWSTR lpszBuff,
	DWORD dwBuffSize,
	DWORD dwFlags)
{
	FIXME("(%s, %p, 0x%04lx, 0x%04lx) stub\n",
	debugstr_w(lpszPath), lpszBuff, dwBuffSize, dwFlags);
	lstrcpyW(lpszBuff, lpszPath);
	return 0;
}

/*************************************************************************
*	PathProcessCommandAW
*/
HRESULT WINAPI PathProcessCommandAW (
	LPCVOID lpszPath,
	LPVOID lpszBuff,
	DWORD dwBuffSize,
	DWORD dwFlags)
{
	if (VERSION_OsIsUnicode())
	  return PathProcessCommandW(lpszPath, lpszBuff, dwBuffSize, dwFlags);
	return PathProcessCommandA(lpszPath, lpszBuff, dwBuffSize, dwFlags);
}

/*************************************************************************
 *	PathCompactPathExA
 */
BOOL WINAPI PathCompactPathExA(
	LPSTR pszOut,
	LPCSTR pszSrc,
	UINT cchMax,
	DWORD dwFlags)
{
	FIXME("%p %s 0x%08x 0x%08lx\n", pszOut, pszSrc, cchMax, dwFlags);
	return FALSE;
}

/*************************************************************************
 *	PathCompactPathExW
 */
BOOL WINAPI PathCompactPathExW(
	LPWSTR pszOut,
	LPCWSTR pszSrc,
	UINT cchMax,
	DWORD dwFlags)
{
	FIXME("%p %s 0x%08x 0x%08lx\n", pszOut, debugstr_w(pszSrc), cchMax, dwFlags);
	return FALSE;
}

/*
	########## special ##########
*/

/*************************************************************************
 * PathSetDlgItemPathA
 *
 * NOTES
 *  use PathCompactPath to make sure, the path fits into the control
 */
BOOL WINAPI PathSetDlgItemPathA(HWND hDlg, int id, LPCSTR pszPath) 
{	TRACE("%x %x %s\n",hDlg, id, pszPath);
	return SetDlgItemTextA(hDlg, id, pszPath);
}

/*************************************************************************
 * PathSetDlgItemPathW
 */
BOOL WINAPI PathSetDlgItemPathW(HWND hDlg, int id, LPCWSTR pszPath) 
{	TRACE("%x %x %s\n",hDlg, id, debugstr_w(pszPath));
	return SetDlgItemTextW(hDlg, id, pszPath);
}

/*************************************************************************
 * PathSetDlgItemPathAW
 */
BOOL WINAPI PathSetDlgItemPathAW(HWND hDlg, int id, LPCVOID pszPath) 
{	if (VERSION_OsIsUnicode())
	  return PathSetDlgItemPathW(hDlg, id, pszPath);
	return PathSetDlgItemPathA(hDlg, id, pszPath);
}


/*************************************************************************
 * SHGetSpecialFolderPathA [SHELL32.175]
 * 
 * converts csidl to path
 */
 
static char * szSHFolders = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
static char * szSHUserFolders = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders";
#if 0
static char * szEnvUserProfile = "%USERPROFILE%";
static char * szEnvSystemRoot = "%SYSTEMROOT%";
#endif

BOOL WINAPI SHGetSpecialFolderPathA (
	HWND hwndOwner,
	LPSTR szPath,
	DWORD csidl,
	BOOL bCreate)
{
	CHAR	szValueName[MAX_PATH], szDefaultPath[MAX_PATH];
	HKEY	hRootKey, hKey;
	BOOL	bRelative = TRUE;
	DWORD	dwType, dwDisp, dwPathLen = MAX_PATH;

	TRACE("0x%04x,%p,csidl=%lu,0x%04x\n", hwndOwner,szPath,csidl,bCreate);

	/* build default values */
	switch(csidl)
	{
	  case CSIDL_APPDATA:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy (szValueName, "AppData");
	    strcpy (szDefaultPath, "AppData");
	    break;

	  case CSIDL_COOKIES:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy (szValueName, "Cookies");
	    strcpy(szDefaultPath, "Cookies");
	    break;

	  case CSIDL_DESKTOPDIRECTORY:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "Desktop");
	    strcpy(szDefaultPath, "Desktop");
	    break;

	  case CSIDL_COMMON_DESKTOPDIRECTORY:
	    hRootKey = HKEY_LOCAL_MACHINE;
	    strcpy(szValueName, "Common Desktop");
	    strcpy(szDefaultPath, "Desktop");
	    break;

	  case CSIDL_FAVORITES:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "Favorites");
	    strcpy(szDefaultPath, "Favorites");
	    break;

	  case CSIDL_FONTS:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "Fonts");
	    strcpy(szDefaultPath, "Fonts");
	    break;

	  case CSIDL_HISTORY:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "History");
	    strcpy(szDefaultPath, "History");
	    break;

	  case CSIDL_NETHOOD:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "NetHood");
	    strcpy(szDefaultPath, "NetHood");
	    break;

	  case CSIDL_INTERNET_CACHE:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "Cache");
	    strcpy(szDefaultPath, "Temporary Internet Files");
	    break;

	  case CSIDL_PERSONAL:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "Personal");
	    strcpy(szDefaultPath, "My Own Files");
	    bRelative = FALSE;
	    break;

	  case CSIDL_PRINTHOOD:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "PrintHood");
	    strcpy(szDefaultPath, "PrintHood");
	    break;

	  case CSIDL_PROGRAMS:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "Programs");
	    strcpy(szDefaultPath, "StartMenu\\Programs");
	    break;

	  case CSIDL_COMMON_PROGRAMS:
	    hRootKey = HKEY_LOCAL_MACHINE;
	    strcpy(szValueName, "Common Programs");
	    strcpy(szDefaultPath, "");
	    break;

	  case CSIDL_RECENT:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "Recent");
	    strcpy(szDefaultPath, "Recent");
	    break;

	  case CSIDL_SENDTO:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "SendTo");
	    strcpy(szDefaultPath, "SendTo");
	    break;

	  case CSIDL_STARTMENU:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "StartMenu");
	    strcpy(szDefaultPath, "StartMenu");
	    break;

	  case CSIDL_COMMON_STARTMENU:
	    hRootKey = HKEY_LOCAL_MACHINE;
	    strcpy(szValueName, "Common StartMenu");
	    strcpy(szDefaultPath, "StartMenu");
	    break;

	  case CSIDL_STARTUP:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "Startup");
	    strcpy(szDefaultPath, "StartMenu\\Programs\\Startup");
	    break;

	  case CSIDL_COMMON_STARTUP:
	    hRootKey = HKEY_LOCAL_MACHINE;
	    strcpy(szValueName, "Common Startup");
	    strcpy(szDefaultPath, "StartMenu\\Programs\\Startup");
	    break;

	  case CSIDL_TEMPLATES:
	    hRootKey = HKEY_CURRENT_USER;
	    strcpy(szValueName, "Templates");
	    strcpy(szDefaultPath, "ShellNew");
	    break;

	  default:
	    ERR("folder unknown or not allowed\n");
	    return FALSE;
	}

	/* user shell folders */
	if (RegCreateKeyExA(hRootKey,szSHUserFolders,0,NULL,0,KEY_ALL_ACCESS,NULL,&hKey,&dwDisp)) return FALSE;

	if (RegQueryValueExA(hKey,szValueName,NULL,&dwType,(LPBYTE)szPath,&dwPathLen))
	{
	  RegCloseKey(hKey);

	  /* shell folders */
	  if (RegCreateKeyExA(hRootKey,szSHFolders,0,NULL,0,KEY_ALL_ACCESS,NULL,&hKey,&dwDisp)) return FALSE;

	  if (RegQueryValueExA(hKey,szValueName,NULL,&dwType,(LPBYTE)szPath,&dwPathLen))
	  {

	    /* value not existing */
	    if (bRelative)
	    {
	      GetWindowsDirectoryA(szPath, MAX_PATH);
	      PathAddBackslashA(szPath);
	      strcat(szPath, szDefaultPath);
	    }
	    else
	    {
	      strcpy(szPath, "C:\\");	/* fixme ??? */
	      strcat(szPath, szDefaultPath);
	    }
	    RegSetValueExA(hKey,szValueName,0,REG_SZ,(LPBYTE)szPath,strlen(szPath)+1);
	  }
	}
	RegCloseKey(hKey);

	/* expand paths like %USERPROFILE% */
	if (dwType == REG_EXPAND_SZ)
	{
	  ExpandEnvironmentStringsA(szPath, szDefaultPath, MAX_PATH);
	  strcpy(szPath, szDefaultPath);
	}

	/* if we don't care about existing directorys we are ready */
	if(csidl & CSIDL_FLAG_DONT_VERIFY) return TRUE;

	if (PathFileExistsA(szPath)) return TRUE;

	/* not existing but we not allowed to create it */
	if (!bCreate) return FALSE;
	
	if (!CreateDirectoryA(szPath,NULL))
	{
	  ERR("Failed to create directory '%s'.\n", szPath);
	  return FALSE;
	}

	MESSAGE("Created not existing system directory '%s'\n", szPath);
	return TRUE;
}

/*************************************************************************
 * SHGetSpecialFolderPathW
 */
BOOL WINAPI SHGetSpecialFolderPathW (
	HWND hwndOwner,
	LPWSTR szPath,
	DWORD csidl,
	BOOL bCreate)
{
	char szTemp[MAX_PATH];
	
	if (SHGetSpecialFolderPathA(hwndOwner, szTemp, csidl, bCreate))
	{
	  lstrcpynAtoW(szPath, szTemp, MAX_PATH);
	}

	TRACE("0x%04x,%p,csidl=%lu,0x%04x\n", hwndOwner,szPath,csidl,bCreate);

	return TRUE;
}

/*************************************************************************
 * SHGetSpecialFolderPathAW
 */
BOOL WINAPI SHGetSpecialFolderPathAW (
	HWND hwndOwner,
	LPVOID szPath,
	DWORD csidl,
	BOOL bCreate)

{
	if (VERSION_OsIsUnicode())
	  return SHGetSpecialFolderPathW (hwndOwner, szPath, csidl, bCreate);
	return SHGetSpecialFolderPathA (hwndOwner, szPath, csidl, bCreate);
}

/*************************************************************************
 * SHGetFolderPathA			[SHFOLDER.@]
 */
HRESULT WINAPI SHGetFolderPathA(
	HWND hwndOwner,
	int nFolder,
	HANDLE hToken,	/* FIXME: get paths for specific user */
	DWORD dwFlags,	/* FIXME: SHGFP_TYPE_CURRENT|SHGFP_TYPE_DEFAULT */
	LPSTR pszPath)
{
	return (SHGetSpecialFolderPathA(
		hwndOwner,
		pszPath,
		CSIDL_FOLDER_MASK & nFolder,
		CSIDL_FLAG_CREATE & nFolder )) ? S_OK : E_FAIL;
}

/*************************************************************************
 * SHGetFolderPathW			[SHFOLDER.@]
 */
HRESULT WINAPI SHGetFolderPathW(
	HWND hwndOwner,
	int nFolder,
	HANDLE hToken,
	DWORD dwFlags,
	LPWSTR pszPath)
{
	return (SHGetSpecialFolderPathW(
		hwndOwner,
		pszPath,
		CSIDL_FOLDER_MASK & nFolder,
		CSIDL_FLAG_CREATE & nFolder )) ? S_OK : E_FAIL;
}

/*************************************************************************
 * PathCanonicalizeA
 *
 *  FIXME
 *   returnvalue, use CharNext
 */
 
BOOL WINAPI PathCanonicalizeA(LPSTR pszBuf, LPCSTR pszPath)
{
	int OffsetMin = 0, OffsetSrc = 0, OffsetDst = 0, LenSrc = strlen(pszPath);
	BOOL bModifyed = FALSE;

	TRACE("%p %s\n", pszBuf, pszPath);
	
	pszBuf[OffsetDst]='\0';

	/* keep the root of the path */
	if( LenSrc && (pszPath[OffsetSrc]=='\\'))
	{
	  pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; OffsetMin++; LenSrc--;
	}
	else if ( (LenSrc >= 2) && (pszPath[OffsetSrc+1] == ':'))
	{
	  pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; OffsetMin++; LenSrc--;
	  pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; OffsetMin++; LenSrc--;
	  if (LenSrc && (pszPath[OffsetSrc] == '\\'))
	  {
	    pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; OffsetMin++; LenSrc--;
	    if (LenSrc == 1 && pszPath[OffsetSrc]=='.')
	    {
	      /* C:\. */
	      OffsetSrc++; LenSrc--; bModifyed = TRUE;
	    } 
	    else if (LenSrc == 2 && pszPath[OffsetSrc]=='.' && pszPath[OffsetSrc+1]=='.')
	    {
	      /* C:\.. */
	      OffsetSrc+=2; LenSrc-=2; bModifyed = TRUE;
	    } 
          }
	}
	
	/* ".\" at the beginning of the path */
	if (LenSrc >= 2 && pszPath[OffsetSrc]=='.' && pszPath[OffsetSrc+1]=='\\')
	{
	  OffsetSrc+=2; LenSrc-=2; bModifyed = TRUE;
	} 
	
	while ( LenSrc )
	{
	  if((LenSrc>=3) && (pszPath[OffsetSrc]=='\\') && (pszPath[OffsetSrc+1]=='.') && (pszPath[OffsetSrc+2]=='.'))
	  {
	    /* "\.." found, go one deeper */
	    while((OffsetDst > OffsetMin) && (pszBuf[OffsetDst]!='\\')) OffsetDst--;
	    OffsetSrc += 3; LenSrc -= 3; bModifyed = TRUE;
	    if(OffsetDst == OffsetMin && pszPath[OffsetSrc]=='\\') OffsetSrc++;
	    pszBuf[OffsetDst] = '\0';			/* important for \..\.. */
	  }
	  else if(LenSrc>=2 && pszPath[OffsetSrc]=='\\' && pszPath[OffsetSrc+1]=='.' )
	  {
	    /* "\." found, skip it */
	    OffsetSrc += 2; LenSrc-=2; bModifyed = TRUE;
	  }
	  else
	  {
	    pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; LenSrc--;
	  }
	}
	pszBuf[OffsetDst] = '\0';
	TRACE("-- %s %u\n", pszBuf, bModifyed);
	return bModifyed;
}


/*************************************************************************
 * PathCanonicalizeW
 *
 *  FIXME
 *   returnvalue, use CharNext
 */
BOOL WINAPI PathCanonicalizeW(LPWSTR pszBuf, LPCWSTR pszPath)
{
	int OffsetMin = 0, OffsetSrc = 0, OffsetDst = 0, LenSrc = lstrlenW(pszPath);
	BOOL bModifyed = FALSE;

	TRACE("%p %s\n", pszBuf, debugstr_w(pszPath));
	
	pszBuf[OffsetDst]='\0';

	/* keep the root of the path */
	if( LenSrc && (pszPath[OffsetSrc]=='\\'))
	{
	  pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; OffsetMin++; LenSrc--;
	}
	else if ( (LenSrc >= 2) && (pszPath[OffsetSrc+1] == ':'))
	{
	  pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; OffsetMin++; LenSrc--;
	  pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; OffsetMin++; LenSrc--;
	  if (LenSrc && (pszPath[OffsetSrc] == '\\'))
	  {
	    pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; OffsetMin++; LenSrc--;
	    if (LenSrc == 1 && pszPath[OffsetSrc]=='.')
	    {
	      /* C:\. */
	      OffsetSrc++; LenSrc--; bModifyed = TRUE;
	    } 
	    else if (LenSrc == 2 && pszPath[OffsetSrc]=='.' && pszPath[OffsetSrc+1]=='.')
	    {
	      /* C:\.. */
	      OffsetSrc+=2; LenSrc-=2; bModifyed = TRUE;
	    } 
          }
	}
	
	/* ".\" at the beginning of the path */
	if (LenSrc >= 2 && pszPath[OffsetSrc]=='.' && pszPath[OffsetSrc+1]=='\\')
	{
	  OffsetSrc+=2; LenSrc-=2; bModifyed = TRUE;
	} 
	
	while ( LenSrc )
	{
	  if((LenSrc>=3) && (pszPath[OffsetSrc]=='\\') && (pszPath[OffsetSrc+1]=='.') && (pszPath[OffsetSrc+2]=='.'))
	  {
	    /* "\.." found, go one deeper */
	    while((OffsetDst > OffsetMin) && (pszBuf[OffsetDst]!='\\')) OffsetDst--;
	    OffsetSrc += 3; LenSrc -= 3; bModifyed = TRUE;
	    if(OffsetDst == OffsetMin && pszPath[OffsetSrc]=='\\') OffsetSrc++;
	    pszBuf[OffsetDst] = '\0';			/* important for \..\.. */
	  }
	  else if(LenSrc>=2 && pszPath[OffsetSrc]=='\\' && pszPath[OffsetSrc+1]=='.' )
	  {
	    /* "\." found, skip it */
	    OffsetSrc += 2; LenSrc-=2; bModifyed = TRUE;
	  }
	  else
	  {
	    pszBuf[OffsetDst++] = pszPath[OffsetSrc++]; LenSrc--;
	  }
	}
	pszBuf[OffsetDst] = '\0';
	TRACE("-- %s %u\n", debugstr_w(pszBuf), bModifyed);
	return bModifyed;
}

/*************************************************************************
 * PathFindNextComponentA
 *
 * NOTES
 * special cases:
 *	""              null
 *	aa              "" (pointer to traling NULL)
 *	aa\             "" (pointer to traling NULL)
 *	aa\\            "" (pointer to traling NULL)
 *	aa\\bb          bb
 *	aa\\\bb         \bb
 *	c:\aa\          "aa\"
 *	\\aa            aa
 *	\\aa\b          aa\b
*/
LPSTR WINAPI PathFindNextComponentA(LPCSTR pszPath)
{
	LPSTR pos;

	TRACE("%s\n", pszPath);

	if(!pszPath || !*pszPath) return NULL;
	if(!(pos = StrChrA(pszPath, '\\')))
	  return (LPSTR) pszPath + strlen(pszPath);
	pos++;
	if(pos[0] == '\\') pos++;
	return pos;
}

/*************************************************************************
 * PathFindNextComponentW
 */
LPWSTR WINAPI PathFindNextComponentW(LPCWSTR pszPath)
{
	LPWSTR pos;

	TRACE("%s\n", debugstr_w(pszPath));
	
	if(!pszPath || !*pszPath) return NULL;
	if (!(pos = StrChrW(pszPath, '\\')))
	  return (LPWSTR) pszPath + lstrlenW(pszPath);
	pos++;
	if(pos[0] == '\\') pos++;
	return pos;
}

/*************************************************************************
 * PathAddExtensionA
 *
 * NOTES
 *  it adds never a dot
 */
 
BOOL WINAPI PathAddExtensionA(
	LPSTR  pszPath,
	LPCSTR pszExtension)
{
	if (*pszPath)
	{
	  if (*(PathFindExtensionA(pszPath))) return FALSE;

	  if (!pszExtension || *pszExtension=='\0')
	    strcat(pszPath, "exe");
	  else
	    strcat(pszPath, pszExtension);
	}

	return TRUE;
}

/*************************************************************************
 *	PathAddExtensionW
 */
BOOL WINAPI PathAddExtensionW(
	LPWSTR  pszPath,
	LPCWSTR pszExtension)
{
	static const WCHAR ext[] = { 'e','x','e',0 };

	if (*pszPath)
	{
	  if (*(PathFindExtensionW(pszPath))) return FALSE;

	  if (!pszExtension || *pszExtension=='\0')
	    lstrcatW(pszPath, ext);
	  else
	    lstrcatW(pszPath, pszExtension);
	}
	return TRUE;

}

/*************************************************************************
 *	PathMakePrettyA
 */
BOOL WINAPI PathMakePrettyA(
	LPSTR lpPath)
{
	FIXME("%s\n", lpPath);
	return TRUE;
}

/*************************************************************************
 *	PathMakePrettyW
 */
BOOL WINAPI PathMakePrettyW(
	LPWSTR lpPath)
{
	FIXME("%s\n", debugstr_w(lpPath));
	return TRUE;

}

/*************************************************************************
 *	PathCommonPrefixA
 */
int WINAPI PathCommonPrefixA(
	LPCSTR pszFile1,
	LPCSTR pszFile2,
	LPSTR achPath)
{
	FIXME("%s %s %p\n", pszFile1, pszFile2, achPath);
	return 0;
}

/*************************************************************************
 *	PathCommonPrefixW
 */
int WINAPI PathCommonPrefixW(
	LPCWSTR pszFile1,
	LPCWSTR pszFile2,
	LPWSTR achPath)
{
	FIXME("%s %s %p\n", debugstr_w(pszFile1), debugstr_w(pszFile2),achPath );
	return 0;
}

/*************************************************************************
 *	PathCompactPathA
 */
BOOL WINAPI PathCompactPathA(HDC hDC, LPSTR pszPath, UINT dx)
{
	FIXME("0x%08x %s 0x%08x\n", hDC, pszPath, dx);
	return FALSE;
}

/*************************************************************************
 *	PathCompactPathW
 */
BOOL WINAPI PathCompactPathW(HDC hDC, LPWSTR pszPath, UINT dx)
{
	FIXME("0x%08x %s 0x%08x\n", hDC, debugstr_w(pszPath), dx);
	return FALSE;
}

/*************************************************************************
 *	PathGetCharTypeA
 */
UINT WINAPI PathGetCharTypeA(UCHAR ch)
{
	FIXME("%c\n", ch);
	return 0;
}

/*************************************************************************
 *	PathGetCharTypeW
 */
UINT WINAPI PathGetCharTypeW(WCHAR ch)
{
	FIXME("%c\n", ch);
	return 0;
}

/*************************************************************************
 *	PathMakeSystemFolderA
 */
BOOL WINAPI PathMakeSystemFolderA(LPCSTR pszPath)
{
	FIXME("%s\n", pszPath);
	return FALSE;
}

/*************************************************************************
 *	PathMakeSystemFolderW
 */
BOOL WINAPI PathMakeSystemFolderW(LPCWSTR pszPath)
{
	FIXME("%s\n", debugstr_w(pszPath));
	return FALSE;
}

/*************************************************************************
 *	PathRenameExtensionA
 */
BOOL WINAPI PathRenameExtensionA(LPSTR pszPath, LPCSTR pszExt)
{
	FIXME("%s %s\n", pszPath, pszExt);
	return FALSE;
}

/*************************************************************************
 *	PathRenameExtensionW
 */
BOOL WINAPI PathRenameExtensionW(LPWSTR pszPath, LPCWSTR pszExt)
{
	FIXME("%s %s\n", debugstr_w(pszPath), debugstr_w(pszExt));
	return FALSE;
}

/*************************************************************************
 *	PathSearchAndQualifyA
 */
BOOL WINAPI PathSearchAndQualifyA(
	LPCSTR pszPath,
	LPSTR pszBuf,
	UINT cchBuf)
{
	FIXME("%s %s 0x%08x\n", pszPath, pszBuf, cchBuf);
	return FALSE;
}

/*************************************************************************
 *	PathSearchAndQualifyW
 */
BOOL WINAPI PathSearchAndQualifyW(
	LPCWSTR pszPath,
	LPWSTR pszBuf,
	UINT cchBuf)
{
	FIXME("%s %s 0x%08x\n", debugstr_w(pszPath), debugstr_w(pszBuf), cchBuf);
	return FALSE;
}

/*************************************************************************
 *	PathSkipRootA
 */
LPSTR WINAPI PathSkipRootA(LPCSTR pszPath)
{
	FIXME("%s\n", pszPath);
	return (LPSTR)pszPath;
}

/*************************************************************************
 *	PathSkipRootW
 */
LPWSTR WINAPI PathSkipRootW(LPCWSTR pszPath)
{
	FIXME("%s\n", debugstr_w(pszPath));
	return (LPWSTR)pszPath;
}

/*************************************************************************
 *	PathCreateFromUrlA
 */
HRESULT WINAPI PathCreateFromUrlA(
	LPCSTR pszUrl,
	LPSTR pszPath,
	LPDWORD pcchPath,
	DWORD dwFlags)
{
	FIXME("%s %p %p 0x%08lx\n",
	  pszUrl, pszPath, pcchPath, dwFlags);
	return S_OK;
}

/*************************************************************************
 *	PathCreateFromUrlW
 */
HRESULT WINAPI PathCreateFromUrlW(
	LPCWSTR pszUrl,
	LPWSTR pszPath,
	LPDWORD pcchPath,
	DWORD dwFlags)
{
	FIXME("%s %p %p 0x%08lx\n",
	  debugstr_w(pszUrl), pszPath, pcchPath, dwFlags);
	return S_OK;
}

/*************************************************************************
 *	PathRelativePathToA
 */
BOOL WINAPI PathRelativePathToA(
	LPSTR pszPath,
	LPCSTR pszFrom,
	DWORD dwAttrFrom,
	LPCSTR pszTo,
	DWORD dwAttrTo)
{
	FIXME("%s %s 0x%08lx %s 0x%08lx\n",
	  pszPath, pszFrom, dwAttrFrom, pszTo, dwAttrTo);
	return FALSE;
}

/*************************************************************************
 *	PathRelativePathToW
 */
BOOL WINAPI PathRelativePathToW(
	LPWSTR pszPath,
	LPCWSTR pszFrom,
	DWORD dwAttrFrom,
	LPCWSTR pszTo,
	DWORD dwAttrTo)
{
	FIXME("%s %s 0x%08lx %s 0x%08lx\n",
	  debugstr_w(pszPath), debugstr_w(pszFrom), dwAttrFrom, debugstr_w(pszTo), dwAttrTo);
	return FALSE;
}

/*************************************************************************
 *	PathUnmakeSystemFolderA
 */
BOOL WINAPI PathUnmakeSystemFolderA(LPCSTR pszPath)
{
	FIXME("%s\n", pszPath);
	return FALSE;
}

/*************************************************************************
 *	PathUnmakeSystemFolderW
 */
BOOL WINAPI PathUnmakeSystemFolderW(LPCWSTR pszPath)
{
	FIXME("%s\n", debugstr_w(pszPath));
	return FALSE;
}
