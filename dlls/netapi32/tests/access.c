/*
 * Copyright 2002 Andriy Palamarchuk
 *
 * Conformance test of the access functions.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>

#include <windef.h>
#include <winbase.h>
#include <winerror.h>
#include <lmaccess.h>
#include <lmerr.h>
#include <lmapibuf.h>

#include "wine/test.h"

static WCHAR user_name[UNLEN + 1];
static WCHAR computer_name[MAX_COMPUTERNAME_LENGTH + 1];

static const WCHAR sNonexistentUser[] = {'N','o','n','e','x','i','s','t','e','n','t',' ',
                                'U','s','e','r',0};
static WCHAR sTooLongName[] = {'T','h','i','s',' ','i','s',' ','a',' ','b','a','d',
    ' ','u','s','e','r','n','a','m','e',0};
static WCHAR sTooLongPassword[] = {'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h','a','b','c','d','e','f','g','h',
    'a', 0};

static WCHAR sTestUserName[] = {'t', 'e', 's', 't', 'u', 's', 'e', 'r', 0};
static WCHAR sTestUserOldPass[] = {'O', 'l', 'd', 'P', 'a', 's', 's', 'W', '0', 'r', 'd', 'S', 'e', 't', '!', '~', 0};
static const WCHAR sBadNetPath[] = {'\\','\\','B','a',' ',' ','p','a','t','h',0};
static const WCHAR sInvalidName[] = {'\\',0};
static const WCHAR sInvalidName2[] = {'\\','\\',0};
static const WCHAR sEmptyStr[] = { 0 };

static NET_API_STATUS (WINAPI *pNetApiBufferFree)(LPVOID);
static NET_API_STATUS (WINAPI *pNetApiBufferSize)(LPVOID,LPDWORD);
static NET_API_STATUS (WINAPI *pNetQueryDisplayInformation)(LPWSTR,DWORD,DWORD,DWORD,DWORD,LPDWORD,PVOID*);
static NET_API_STATUS (WINAPI *pNetUserGetInfo)(LPCWSTR,LPCWSTR,DWORD,LPBYTE*);
static NET_API_STATUS (WINAPI *pNetUserModalsGet)(LPCWSTR,DWORD,LPBYTE*);
static NET_API_STATUS (WINAPI *pNetUserAdd)(LPCWSTR,DWORD,LPBYTE,LPDWORD);
static NET_API_STATUS (WINAPI *pNetUserDel)(LPCWSTR,LPCWSTR);
static NET_API_STATUS (WINAPI *pNetLocalGroupGetInfo)(LPCWSTR,LPCWSTR,DWORD,LPBYTE*);
static NET_API_STATUS (WINAPI *pNetLocalGroupGetMembers)(LPCWSTR,LPCWSTR,DWORD,LPBYTE*,DWORD,LPDWORD,LPDWORD,PDWORD_PTR);
static DWORD (WINAPI *pDavGetHTTPFromUNCPath)(LPCWSTR,LPWSTR,LPDWORD);
static DWORD (WINAPI *pDavGetUNCFromHTTPPath)(LPCWSTR,LPWSTR,LPDWORD);

static BOOL init_access_tests(void)
{
    DWORD dwSize;
    BOOL rc;

    user_name[0] = 0;
    dwSize = ARRAY_SIZE(user_name);
    rc=GetUserNameW(user_name, &dwSize);
    if (rc==FALSE && GetLastError()==ERROR_CALL_NOT_IMPLEMENTED)
    {
        win_skip("GetUserNameW is not available.\n");
        return FALSE;
    }
    ok(rc, "User Name Retrieved\n");

    computer_name[0] = 0;
    dwSize = ARRAY_SIZE(computer_name);
    ok(GetComputerNameW(computer_name, &dwSize), "Computer Name Retrieved\n");
    return TRUE;
}

static NET_API_STATUS create_test_user(void)
{
    USER_INFO_1 usri;

    usri.usri1_name = sTestUserName;
    usri.usri1_password = sTestUserOldPass;
    usri.usri1_priv = USER_PRIV_USER;
    usri.usri1_home_dir = NULL;
    usri.usri1_comment = NULL;
    usri.usri1_flags = UF_SCRIPT;
    usri.usri1_script_path = NULL;

    return pNetUserAdd(NULL, 1, (LPBYTE)&usri, NULL);
}

static NET_API_STATUS delete_test_user(void)
{
    return pNetUserDel(NULL, sTestUserName);
}

static void run_usergetinfo_tests(void)
{
    NET_API_STATUS rc;
    PUSER_INFO_0 ui0 = NULL;
    PUSER_INFO_10 ui10 = NULL;
    DWORD dwSize;

    if((rc = create_test_user()) != NERR_Success )
    {
        skip("Skipping usergetinfo_tests, create_test_user failed: 0x%08x\n", rc);
        return;
    }

    /* Level 0 */
    rc=pNetUserGetInfo(NULL, sTestUserName, 0, (LPBYTE *)&ui0);
    ok(rc == NERR_Success, "NetUserGetInfo level 0 failed: 0x%08x.\n", rc);
    ok(!lstrcmpW(sTestUserName, ui0->usri0_name),"Username mismatch for level 0.\n");
    pNetApiBufferSize(ui0, &dwSize);
    ok(dwSize >= (sizeof(USER_INFO_0) +
                  (lstrlenW(ui0->usri0_name) + 1) * sizeof(WCHAR)),
       "Is allocated with NetApiBufferAllocate\n");

    /* Level 10 */
    rc=pNetUserGetInfo(NULL, sTestUserName, 10, (LPBYTE *)&ui10);
    ok(rc == NERR_Success, "NetUserGetInfo level 10 failed: 0x%08x.\n", rc);
    ok(!lstrcmpW(sTestUserName, ui10->usri10_name), "Username mismatch for level 10.\n");
    pNetApiBufferSize(ui10, &dwSize);
    ok(dwSize >= (sizeof(USER_INFO_10) +
                  (lstrlenW(ui10->usri10_name) + 1 +
                   lstrlenW(ui10->usri10_comment) + 1 +
                   lstrlenW(ui10->usri10_usr_comment) + 1 +
                   lstrlenW(ui10->usri10_full_name) + 1) * sizeof(WCHAR)),
       "Is allocated with NetApiBufferAllocate\n");

    pNetApiBufferFree(ui0);
    pNetApiBufferFree(ui10);

    /* NetUserGetInfo should always work for the current user. */
    rc=pNetUserGetInfo(NULL, user_name, 0, (LPBYTE*)&ui0);
    ok(rc == NERR_Success, "NetUsetGetInfo for current user failed: 0x%08x.\n", rc);
    pNetApiBufferFree(ui0);

    /* errors handling */
    rc=pNetUserGetInfo(NULL, sTestUserName, 10000, (LPBYTE *)&ui0);
    ok(rc == ERROR_INVALID_LEVEL,"Invalid Level: rc=%d\n",rc);
    rc=pNetUserGetInfo(NULL, sNonexistentUser, 0, (LPBYTE *)&ui0);
    ok(rc == NERR_UserNotFound,"Invalid User Name: rc=%d\n",rc);
    todo_wine {
        /* FIXME - Currently Wine can't verify whether the network path is good or bad */
        rc=pNetUserGetInfo(sBadNetPath, sTestUserName, 0, (LPBYTE *)&ui0);
        ok(rc == ERROR_BAD_NETPATH ||
           rc == ERROR_NETWORK_UNREACHABLE ||
           rc == RPC_S_SERVER_UNAVAILABLE ||
           rc == NERR_WkstaNotStarted || /* workstation service not running */
           rc == RPC_S_INVALID_NET_ADDR, /* Some Win7 */
           "Bad Network Path: rc=%d\n",rc);
    }
    rc=pNetUserGetInfo(sEmptyStr, sTestUserName, 0, (LPBYTE *)&ui0);
    ok(rc == ERROR_BAD_NETPATH || rc == NERR_Success,
       "Bad Network Path: rc=%d\n",rc);
    rc=pNetUserGetInfo(sInvalidName, sTestUserName, 0, (LPBYTE *)&ui0);
    ok(rc == ERROR_INVALID_NAME || rc == ERROR_INVALID_HANDLE,"Invalid Server Name: rc=%d\n",rc);
    rc=pNetUserGetInfo(sInvalidName2, sTestUserName, 0, (LPBYTE *)&ui0);
    ok(rc == ERROR_INVALID_NAME || rc == ERROR_INVALID_HANDLE,"Invalid Server Name: rc=%d\n",rc);

    if(delete_test_user() != NERR_Success)
        trace("Deleting the test user failed. You might have to manually delete it.\n");
}

/* Checks Level 1 of NetQueryDisplayInformation */
static void run_querydisplayinformation1_tests(void)
{
    PNET_DISPLAY_USER Buffer, rec;
    DWORD Result, EntryCount;
    DWORD i = 0;
    BOOL hasAdmin = FALSE;
    BOOL hasGuest = FALSE;

    do
    {
        Result = pNetQueryDisplayInformation(
            NULL, 1, i, 1000, MAX_PREFERRED_LENGTH, &EntryCount,
            (PVOID *)&Buffer);

        ok((Result == ERROR_SUCCESS) || (Result == ERROR_MORE_DATA),
           "Information Retrieved\n");
        rec = Buffer;
        for(; EntryCount > 0; EntryCount--)
        {
            if (rec->usri1_user_id == DOMAIN_USER_RID_ADMIN)
            {
                ok(!hasAdmin, "One admin user\n");
                ok(rec->usri1_flags & UF_SCRIPT, "UF_SCRIPT flag is set\n");
                ok(rec->usri1_flags & UF_NORMAL_ACCOUNT, "UF_NORMAL_ACCOUNT flag is set\n");
                hasAdmin = TRUE;
            }
            else if (rec->usri1_user_id == DOMAIN_USER_RID_GUEST)
            {
                ok(!hasGuest, "One guest record\n");
                ok(rec->usri1_flags & UF_SCRIPT, "UF_SCRIPT flag is set\n");
                ok(rec->usri1_flags & UF_NORMAL_ACCOUNT, "UF_NORMAL_ACCOUNT flag is set\n");
                hasGuest = TRUE;
            }

            i = rec->usri1_next_index;
            rec++;
        }

        pNetApiBufferFree(Buffer);
    } while (Result == ERROR_MORE_DATA);

    ok(hasAdmin, "Doesn't have 'Administrator' account\n");
}

static void run_usermodalsget_tests(void)
{
    NET_API_STATUS rc;
    USER_MODALS_INFO_2 * umi2 = NULL;

    rc = pNetUserModalsGet(NULL, 2, (LPBYTE *)&umi2);
    ok(rc == ERROR_SUCCESS, "NetUserModalsGet failed, rc = %d\n", rc);

    if (umi2)
        pNetApiBufferFree(umi2);
}

static void run_userhandling_tests(void)
{
    NET_API_STATUS ret;
    USER_INFO_1 usri;

    usri.usri1_priv = USER_PRIV_USER;
    usri.usri1_home_dir = NULL;
    usri.usri1_comment = NULL;
    usri.usri1_flags = UF_SCRIPT;
    usri.usri1_script_path = NULL;

    usri.usri1_name = sTooLongName;
    usri.usri1_password = sTestUserOldPass;

    ret = pNetUserAdd(NULL, 1, (LPBYTE)&usri, NULL);
    if (ret == NERR_Success || ret == NERR_UserExists)
    {
        /* Windows NT4 does create the user. Delete the user and also if it already existed
         * due to a previous test run on NT4.
         */
        trace("We are on NT4, we have to delete the user with the too long username\n");
        ret = pNetUserDel(NULL, sTooLongName);
        ok(ret == NERR_Success, "Deleting the user failed : %d\n", ret);
    }
    else if (ret == ERROR_ACCESS_DENIED)
    {
        skip("not enough permissions to add a user\n");
        return;
    }
    else
        ok(ret == NERR_BadUsername ||
           broken(ret == NERR_PasswordTooShort), /* NT4 */
           "Adding user with too long username returned 0x%08x\n", ret);

    usri.usri1_name = sTestUserName;
    usri.usri1_password = sTooLongPassword;

    ret = pNetUserAdd(NULL, 1, (LPBYTE)&usri, NULL);
    ok(ret == NERR_PasswordTooShort || ret == ERROR_ACCESS_DENIED /* Win2003 */,
       "Adding user with too long password returned 0x%08x\n", ret);

    usri.usri1_name = sTooLongName;
    usri.usri1_password = sTooLongPassword;

    ret = pNetUserAdd(NULL, 1, (LPBYTE)&usri, NULL);
    /* NT4 doesn't have a problem with the username so it will report the too long password
     * as the error. NERR_PasswordTooShort is reported for all kind of password related errors.
     */
    ok(ret == NERR_BadUsername || ret == NERR_PasswordTooShort,
            "Adding user with too long username/password returned 0x%08x\n", ret);

    usri.usri1_name = sTestUserName;
    usri.usri1_password = sTestUserOldPass;

    ret = pNetUserAdd(NULL, 5, (LPBYTE)&usri, NULL);
    ok(ret == ERROR_INVALID_LEVEL, "Adding user with level 5 returned 0x%08x\n", ret);

    ret = pNetUserAdd(NULL, 1, (LPBYTE)&usri, NULL);
    if(ret == ERROR_ACCESS_DENIED)
    {
        skip("Insufficient permissions to add users. Skipping test.\n");
        return;
    }
    if(ret == NERR_UserExists)
    {
        skip("User already exists, skipping test to not mess up the system\n");
        return;
    }

    ok(ret == NERR_Success ||
       broken(ret == NERR_PasswordTooShort), /* NT4 */
       "Adding user failed with error 0x%08x\n", ret);
    if(ret != NERR_Success)
        return;

    /* On Windows XP (and newer), calling NetUserChangePassword with a NULL
     * domainname parameter creates a user home directory, iff the machine is
     * not member of a domain.
     * Using \\127.0.0.1 as domain name does not work on standalone machines
     * either, unless the ForceGuest option in the registry is turned off.
     * So let's not test NetUserChangePassword for now.
     */

    ret = pNetUserDel(NULL, sTestUserName);
    ok(ret == NERR_Success, "Deleting the user failed.\n");

    ret = pNetUserDel(NULL, sTestUserName);
    ok(ret == NERR_UserNotFound, "Deleting a nonexistent user returned 0x%08x\n",ret);
}

static void run_localgroupgetinfo_tests(void)
{
    NET_API_STATUS status;
    static const WCHAR admins[] = {'A','d','m','i','n','i','s','t','r','a','t','o','r','s',0};
    PLOCALGROUP_INFO_1 lgi = NULL;
    PLOCALGROUP_MEMBERS_INFO_3 buffer = NULL;
    DWORD entries_read = 0, total_entries =0;
    int i;

    status = pNetLocalGroupGetInfo(NULL, admins, 1, (LPBYTE *)&lgi);
    ok(status == NERR_Success || broken(status == NERR_GroupNotFound),
       "NetLocalGroupGetInfo unexpectedly returned %d\n", status);
    if (status != NERR_Success) return;

    trace("Local groupname:%s\n", wine_dbgstr_w( lgi->lgrpi1_name));
    trace("Comment: %s\n", wine_dbgstr_w( lgi->lgrpi1_comment));

    pNetApiBufferFree(lgi);

    status = pNetLocalGroupGetMembers(NULL, admins, 3, (LPBYTE *)&buffer, MAX_PREFERRED_LENGTH, &entries_read, &total_entries, NULL);
    ok(status == NERR_Success, "NetLocalGroupGetMembers unexpectedly returned %d\n", status);
    ok(entries_read > 0 && total_entries > 0, "Amount of entries is unexpectedly 0\n");

    for(i=0;i<entries_read;i++)
        trace("domain and name: %s\n", wine_dbgstr_w(buffer[i].lgrmi3_domainandname));

    pNetApiBufferFree(buffer);
}

static void test_DavGetHTTPFromUNCPath(void)
{
    static const struct
    {
        const WCHAR *path;
        DWORD ret;
        const WCHAR *ret_path;
        DWORD broken_ret; /* < Win10 1709 */
        BOOL todo;
    }
    tests[] =
    {
        {L"",                       ERROR_BAD_NET_NAME, NULL, ERROR_INVALID_PARAMETER, TRUE},
        {L"c:\\",                   ERROR_BAD_NET_NAME, NULL, ERROR_INVALID_PARAMETER, TRUE},
        {L"\\\\",                   ERROR_BAD_NET_NAME, NULL, ERROR_INVALID_PARAMETER, TRUE},
        {L"\\a\\b",                 ERROR_BAD_NET_NAME, NULL, ERROR_INVALID_PARAMETER, TRUE},
        {L"\\\\a",                  ERROR_SUCCESS, L"http://a"},
        {L"\\\\a\\",                ERROR_SUCCESS, L"http://a"},
        {L"\\\\a\\b",               ERROR_SUCCESS, L"http://a/b"},
        {L"\\\\a\\b\\",             ERROR_SUCCESS, L"http://a/b"},
        {L"\\\\a\\b\\c",            ERROR_SUCCESS, L"http://a/b/c"},
        {L"\\\\a@SSL\\b",           ERROR_SUCCESS, L"https://a/b"},
        {L"\\\\a@ssl\\b",           ERROR_SUCCESS, L"https://a/b"},
        {L"\\\\a@tls\\b",           ERROR_INVALID_PARAMETER},
        {L"\\\\a@SSL@443\\b",       ERROR_SUCCESS, L"https://a/b"},
        {L"\\\\a@SSL@80\\b",        ERROR_SUCCESS, L"https://a:80/b"},
        {L"\\\\a@80@SSL\\b",        ERROR_INVALID_PARAMETER},
        {L"\\\\a@80\\b",            ERROR_SUCCESS, L"http://a/b"},
        {L"\\\\a@8080\\b",          ERROR_SUCCESS, L"http://a:8080/b"},
        {L"\\\\a\\b/",              ERROR_SUCCESS, L"http://a/b"},
        {L"\\\\a/b",                ERROR_SUCCESS, L"http://a/b"},
        {L"\\\\a.\\b",              ERROR_SUCCESS, L"http://a./b"},
        {L"\\\\.a\\b",              ERROR_SUCCESS, L"http://.a/b"},
        {L"//a/b",                  ERROR_SUCCESS, L"http://a/b", ERROR_INVALID_PARAMETER, TRUE},
        {L"\\\\a\\\\",              ERROR_BAD_NET_NAME, NULL, ERROR_SUCCESS, TRUE},
        {L"\\\\\\a\\",              ERROR_BAD_NET_NAME, NULL, ERROR_SUCCESS, TRUE},
        {L"\\\\a\\b\\\\",           ERROR_BAD_NET_NAME, NULL, ERROR_SUCCESS, TRUE},
        {L"\\\\.\\a",               ERROR_BAD_NET_NAME, NULL, ERROR_SUCCESS, TRUE},
        {L"\\\\a\\b:",              ERROR_BAD_NET_NAME, NULL, ERROR_SUCCESS, TRUE},
    };
    WCHAR buf[MAX_PATH];
    DWORD i, ret, size;

    if (!pDavGetHTTPFromUNCPath)
    {
        win_skip( "DavGetHTTPFromUNCPath is missing\n" );
        return;
    }

    if (0) /* crashes on Windows */
    {
        ret = pDavGetHTTPFromUNCPath(NULL, NULL, NULL);
        ok(ret == ERROR_INVALID_PARAMETER, "got %u\n", ret);

        size = 0;
        ret = pDavGetHTTPFromUNCPath(L"", buf, &size);
        ok(ret == ERROR_INVALID_PARAMETER, "got %u\n", ret);

        ret = pDavGetHTTPFromUNCPath(L"\\\\a\\b", buf, NULL);
        ok(ret == ERROR_INVALID_PARAMETER, "got %u\n", ret);
    }

    ret = pDavGetHTTPFromUNCPath(L"", buf, NULL);
    ok(ret == ERROR_INVALID_PARAMETER, "got %u\n", ret);

    size = 0;
    ret = pDavGetHTTPFromUNCPath(L"", NULL, &size);
    ok(ret == ERROR_INVALID_PARAMETER || ret == ERROR_BAD_NET_NAME /* Win10 1709+ */, "got %u\n", ret);

    size = 0;
    ret = pDavGetHTTPFromUNCPath(L"\\\\a\\b", NULL, &size);
    ok(ret == ERROR_INSUFFICIENT_BUFFER, "got %u\n", ret);

    buf[0] = 0;
    size = 0;
    ret = pDavGetHTTPFromUNCPath(L"\\\\a\\b", buf, &size);
    ok(ret == ERROR_INSUFFICIENT_BUFFER, "got %u\n", ret);
    ok(size == 11, "got %u\n", size);

    for (i = 0; i < ARRAY_SIZE(tests); i++)
    {
        buf[0] = 0;
        size = ARRAY_SIZE(buf);
        ret = pDavGetHTTPFromUNCPath( tests[i].path, buf, &size );
        todo_wine_if (tests[i].todo)
            ok(ret == tests[i].ret || broken(ret == tests[i].broken_ret),
                    "%u: expected %u got %u\n", i, tests[i].ret, ret);
        if (!ret)
        {
            if (tests[i].ret_path)
                ok(!wcscmp(buf, tests[i].ret_path), "%u: expected %s got %s\n",
                        i, wine_dbgstr_w(tests[i].ret_path), wine_dbgstr_w(buf));
            ok(size == wcslen(buf) + 1, "%u: got %u\n", i, size);
        }
        else
            ok(size == ARRAY_SIZE(buf), "%u: wrong size %u\n", i, size);
    }
}


static void test_DavGetUNCFromHTTPPath(void)
{
    static const struct
    {
        const WCHAR *path;
        DWORD ret;
        const WCHAR *ret_path;
        DWORD broken_ret; /* < Win10 1709 */
        BOOL todo;
    }
    tests[] =
    {
        {L"",                       ERROR_INVALID_PARAMETER},
        {L"http://server/path",     ERROR_SUCCESS, L"\\\\server\\DavWWWRoot\\path"},
        {L"https://host/path",      ERROR_SUCCESS, L"\\\\host@SSL\\DavWWWRoot\\path"},
        {L"\\\\server",             ERROR_INVALID_PARAMETER},
        {L"\\\\server\\path",       ERROR_INVALID_PARAMETER},
        {L"\\\\http://server/path", ERROR_INVALID_PARAMETER},
        {L"http://",                ERROR_BAD_NETPATH, NULL, ERROR_BAD_NET_NAME, TRUE},
        {L"http:",                  ERROR_BAD_NET_NAME, NULL, ERROR_INVALID_PARAMETER, TRUE},
        {L"http",                   ERROR_INVALID_PARAMETER},
        {L"http:server",            ERROR_BAD_NET_NAME, NULL, ERROR_INVALID_PARAMETER, TRUE},
        {L"http://server:80",       ERROR_SUCCESS, L"\\\\server\\DavWWWRoot"},
        {L"http://server:81",       ERROR_SUCCESS, L"\\\\server@81\\DavWWWRoot"},
        {L"https://server:80",      ERROR_SUCCESS, L"\\\\server@SSL@80\\DavWWWRoot"},
        {L"HTTP://server/path",     ERROR_SUCCESS, L"\\\\server\\DavWWWRoot\\path"},
        {L"http://server:65537",    ERROR_BAD_NETPATH, NULL, ERROR_SUCCESS, TRUE},
        {L"http://server/path/",    ERROR_SUCCESS, L"\\\\server\\DavWWWRoot\\path"},
        {L"http://server/path//",   ERROR_SUCCESS, L"\\\\server\\DavWWWRoot\\path", ERROR_BAD_NET_NAME, TRUE},
        {L"http://server:/path",    ERROR_BAD_NETPATH, NULL, ERROR_SUCCESS, TRUE},
        {L"http://server",          ERROR_SUCCESS, L"\\\\server\\DavWWWRoot"},
        {L"https://server:443",     ERROR_SUCCESS, L"\\\\server@SSL\\DavWWWRoot"},
    };
    WCHAR buf[MAX_PATH];
    DWORD i, ret, size;

    if (!pDavGetUNCFromHTTPPath)
    {
        win_skip( "DavGetUNCFromHTTPPath is missing\n" );
        return;
    }

    if (0) /* crashes on Windows */
    {
        ret = pDavGetUNCFromHTTPPath(NULL, NULL, NULL);
        ok(ret == ERROR_INVALID_PARAMETER, "got %u\n", ret);

        ret = pDavGetUNCFromHTTPPath(L"http://server/path", buf, NULL);
        ok(ret == ERROR_INVALID_PARAMETER, "got %u\n", ret);
    }

    ret = pDavGetUNCFromHTTPPath(L"", buf, NULL);
    ok(ret == ERROR_INVALID_PARAMETER, "got %u\n", ret);

    size = 0;
    ret = pDavGetUNCFromHTTPPath(L"", NULL, &size);
    ok(ret == ERROR_INVALID_PARAMETER, "got %u\n", ret);

    buf[0] = 0;
    size = 0;
    ret = pDavGetUNCFromHTTPPath(L"", buf, &size);
    ok(ret == ERROR_INVALID_PARAMETER, "got %u\n", ret);

    size = 0;
    ret = pDavGetUNCFromHTTPPath(L"http://server/path", NULL, &size);
    ok( ret == ERROR_INSUFFICIENT_BUFFER, "got %u\n", ret );

    buf[0] = 0;
    size = 0;
    ret = pDavGetUNCFromHTTPPath(L"http://server/path", buf, &size);
    ok(ret == ERROR_INSUFFICIENT_BUFFER, "got %u\n", ret);
    ok(size == 25, "got %u\n", size);

    for (i = 0; i < ARRAY_SIZE(tests); i++)
    {
        buf[0] = 0;
        size = ARRAY_SIZE(buf);
        ret = pDavGetUNCFromHTTPPath( tests[i].path, buf, &size );
        todo_wine_if (tests[i].todo)
            ok(ret == tests[i].ret || broken(ret == tests[i].broken_ret),
                    "%u: expected %u got %u\n", i, tests[i].ret, ret);
        if (!ret)
        {
            if (tests[i].ret_path)
                ok(!wcscmp(buf, tests[i].ret_path), "%u: expected %s got %s\n",
                        i, wine_dbgstr_w(tests[i].ret_path), wine_dbgstr_w(buf));
            ok(size == wcslen(buf) + 1, "%u: got %u\n", i, size);
        }
        else
            ok(size == ARRAY_SIZE(buf), "%u: wrong size %u\n", i, size);
    }
}

START_TEST(access)
{
    HMODULE hnetapi32=LoadLibraryA("netapi32.dll");

    pNetApiBufferFree=(void*)GetProcAddress(hnetapi32,"NetApiBufferFree");
    pNetApiBufferSize=(void*)GetProcAddress(hnetapi32,"NetApiBufferSize");
    pNetQueryDisplayInformation=(void*)GetProcAddress(hnetapi32,"NetQueryDisplayInformation");
    pNetUserGetInfo=(void*)GetProcAddress(hnetapi32,"NetUserGetInfo");
    pNetUserModalsGet=(void*)GetProcAddress(hnetapi32,"NetUserModalsGet");
    pNetUserAdd=(void*)GetProcAddress(hnetapi32, "NetUserAdd");
    pNetUserDel=(void*)GetProcAddress(hnetapi32, "NetUserDel");
    pNetLocalGroupGetInfo=(void*)GetProcAddress(hnetapi32, "NetLocalGroupGetInfo");
    pNetLocalGroupGetMembers=(void*)GetProcAddress(hnetapi32, "NetLocalGroupGetMembers");
    pDavGetHTTPFromUNCPath = (void*)GetProcAddress(hnetapi32, "DavGetHTTPFromUNCPath");
    pDavGetUNCFromHTTPPath = (void*)GetProcAddress(hnetapi32, "DavGetUNCFromHTTPPath");

    /* These functions were introduced with NT. It's safe to assume that
     * if one is not available, none are.
     */
    if (!pNetApiBufferFree) {
        win_skip("Needed functions are not available\n");
        FreeLibrary(hnetapi32);
        return;
    }

    if (init_access_tests()) {
        run_userhandling_tests();
        run_usergetinfo_tests();
        run_querydisplayinformation1_tests();
        run_usermodalsget_tests();
        run_localgroupgetinfo_tests();
    }

    test_DavGetHTTPFromUNCPath();
    test_DavGetUNCFromHTTPPath();
    FreeLibrary(hnetapi32);
}
