# Wine Thai API

Wine with support for Thai API from Windows 3.0/3.1/95/98/ME Thai Edition.

# Thai API status

    * = to be implement
    x = don't implement

| Status      | API         | Function                 | Note                         |
| ----------- |:----------- | :----------------------- | :--------------------------- |
| stub        | KEYBOARD.8  | TkEnable                 | Win 3.0a Thai + Win 3.1 Thai |
| stub        | KEYBOARD.9  | TkDisable                | Win 3.0a Thai + Win 3.1 Thai |
| implemented | KEYBOARD.10 | IsThaiExist              | Win 3.0a Thai + Win 3.1 Thai |
| implemented | KEYBOARD.11 | GetThaiStatus            | Win 3.0a Thai is GetThaiStatus, Win 3.1 Thai is WEP |
| implemented | KEYBOARD.12 | SetThaiStatus            | Win 3.0a Thai is SetThaiStatus, Win 3.1 Thai is NotifyLanguage |
| stub        | KEYBOARD.13 | SetCheckSequence         | Win 3.0a Thai is SetCheckSequence, Win 3.1 Thai is ___EXPORTEDSTUB |
| implemented | KEYBOARD.15 | SetThaiKbdLayout         | |
| implemented | KEYBOARD.16 | GetThaiKbdLayout         | |
| implemented | KEYBOARD.17 | lstrKbdThaiToEng         | |
| implemented | KEYBOARD.18 | lstrKbdEngToThai         | |
| implemented | KEYBOARD.19 | lstrNumThaiToArabic      | |
| implemented | KEYBOARD.20 | lstrNumArabicToThai      | |
|x stub       | KEYBOARD.21 | SetBahtKey               | |
|x stub       | KEYBOARD.23 | SetThaiCapsLockEffect    | |
| implemented | GDI.490     | IsThaiFont               | Conflict with CloseEnhMetafile, required by Microsoft Word 6.0 Thai Edition |
|x stub       | GDI.491     | IsInvalidDisplaySeqBeep  | Conflict with CopyEnhMetafile |
|x stub       | GDI.492     | SetInvalidDisplaySeqBeep | Conflict with CreateEnhMetafile |
| semi-stub   | USER.474    | GetTaskInterface         | |
| stub        | USER.475    | GetKeyboardLanguage      | Conflict with SetScrollInfo |
| stub        | USER.476    | SetKeyboardLanguage      | Conflict with GetScrollInfo, **FIXME: Properly implement this function** |
| stub        | USER.477    | SetLanguageToggleKey     | Conflict with GetKeyboardLayoutName |
|* stub       | USER.485    | FindThaiWordBreak        | use cttex, libthai, etc.? |
| implemented | USER.486    | ThaiCharType             | |
|x stub       | USER.487    | IsInvalidKbdSeqBeep      | |
|x stub       | USER.488    | SetInvalidKbdSeqBeep     | |
|* stub       | USER.489    | IsThaiCharCombinable     | |
|* stub       | USER.490    | ThaiStrValid             | use IsThaiCharCombinable to check? |
|* stub       | USER.491    | ValidateThaiStr          | |
| implemented | USER.492    | CountThaiCharType        | |
| implemented | USER.493    | lstrDayName              | |
| implemented | USER.494    | lstrMonthName            | |
| implemented | USER.495    | lstrYearNumber           | |
| implemented | USER.496    | IsThaiKbdSeqCheck        | |
| implemented | USER.497    | SetThaiKbdSeqCheck       | |
| semi-stub   | USER.498    | SetTaskInterface         | |
| stub        | LPKTHAI.DLL | | Microsoft Office 97 Thai Edition Language Switcher check for present of this dll. |
| x           | THAIDRV.32  | ThaiExist                | Win 3.0a Thai, call IsThaiExist |
| x           | THAIDRV.33  | CheckSequence            | Win 3.0a Thai, call SetCheckSequence |
| x           | THAIDRV.34  | KeyboardStatus           | Win 3.0a Thai, call GetThaiStatus and SetThaiStatus, then post a message |

### Note:
- Other than TkEnable, TkDisable, IsThaiExist, GetThaiStatus, SetThaiStatus
and SetCheckSequence function in keyboard.drv, all other Thai API functions
in GDI and USER is not exist in Windows 3.0a Thai Edition.
- In order for Thai functions to function correctly, you have to add this
entry in win.ini in your wine prefix, must use TIS620 character encoding.

    [Thai]
    iThaiDate=1
    sLongDate=dddd'ที่ 'd' 'MMMM' พ.ศ. 'yyyy

- On Windows 9x Thai Edition, there is a program called MIGRATE.EXE that load Win95 Thai API dll
(FTLX041E.DLL and LPKTHAI.DLL) and patch it into running GDI.EXE and USER.EXE.

### Will not implement:
- Baht to Mai-eak Mai-to key

### Todo:
- Create stub FindThaiWordBreak function for lpkthai.dll16, currently it didn't build into dll file.
- Implement IsThaiCharCombinable
- Implement ThaiStrValid
- Implement ValidateThaiStr
- Implement FindThaiWordBreak, use cttex or libthai or ...
- Process message `WM_LANGUAGE_CHANGE` in user.exe16/message.c:DefWindowProc16 to change keyboard language. Keyboard language is changed to the one that was given in wParam.
- Win 3.1 Thai use 0xEF to sperate English and Thai text on menu and captions,
modify the display text function to support this behavior?
example: [english str][0xEF][thai str]
- SetKeyboardLanguage, use change keyboard language function in Win32
- Modify ToAscii to support Thai input
- Fix display string crash with Thai text
- Add Thai bitmap system font
- In ThaiCharType16 function, consider using lookup table
- Investigate Thai API usage in Windows 95 Thai Edition, Windows NT 4.0 Thai

### List of software that use Thai API:

- Borland dBase 5.0 for Windows Thai Edition?
- Borland Quattro Pro for Windows Thai Edition?
- Borland Paradox 4.5 for Windows Thai Edition?
- CU-Writer for Windows (CWW77)

        CWW.EXE
        API:
            KEYBOARD.15     SetThaiKbdLayout
            KEYBOARD.16     GetThaiKbdLayout
            KEYBOARD.17     lstrKbdThaiToEng
            USER.475        GetKeyboardLanguage
            USER.476        SetKeyboardLanguage
            USER.477        SetLanguageToggleKey
            USER.497        SetThaiKbdSeqCheck
        bugs
            - ESC menu not working, 002f:fixme:hook:SetWindowsHookEx16 hook type 1 broken in Win16
            - Can't type Thai language, it type as English instead
        CWWPRINT.EXE
        API:
            KEYBOARD.16     GetThaiKbdLayout
            KEYBOARD.17     lstrKbdThaiToEng
            USER.475        GetKeyboardLanguage
            USER.476        SetKeyboardLanguage
            USER.477        SetLanguageToggleKey
            USER.496        IsThaiKbdSeqCheck
            USER.497        SetThaiKbdSeqCheck

- iRC ZeWrite Professional for Windows 1.0

        API:
            KEYBOARD.17     lstrKbdThaiToEng
            KEYBOARD.18     lstrKbdEngToThai
            USER.485        FindThaiWordBreak
            USER.486        ThaiCharType
            USER.497        SetThaiKbdSeqCheck

- Lotus Approach 3.0 Thai?
- Lotus ccMail 2.1 Thai?
- Lotus SmartSuite Release 2.0 Thai?
- Lotus SmartSuite Release 3.0 Thai?
- Microsoft Excel 5.0a Thai Edition

        API:
            USER.475        GetKeyboardLanguage
            USER.476        SetKeyboardLanguage
            USER.485        FindThaiWordBreak
            USER.489        IsThaiCharCombinable

- Microsoft Office 4.2 Standard and Microsoft Office 4.3 Professional Thai Edition

        Only Excel 5.0a, Word 6.0a is using Thai API.

- Microsoft Office 95 Standard and Microsoft Office 95 Professional Thai Edition

        WINDOWS/MSAPPS/TEXTCONV/THAWRD32.CNV
        WINDOWS/SYSTEM/MSO95.DLL
        MSACCESS.EXE
        POWERPNT.EXE
        WINWORD.EXE
        API:
            FTLX041E.DLL    FindThaiWordBreak

- Microsoft Office 97 Professional Thai Edition

        Language Switcher require `LPKTHAI.DLL` to switch to Thai language.

- Microsoft Mail for Windows 3.02 (from Windows for Workgroup 3.1 Thai Edition)

        API:
            KEYBOARD.17     lstrKbdThaiToEng
            KEYBOARD.18     lstrKbdEngToThai
            USER.474        GetTaskInterface
            USER.485        FindThaiWordBreak
            USER.493        lstrDayName
            USER.494        lstrMonthName
            USER.495        lstrYearNumber
            USER.498        SetTaskInterface

- Microsoft Schedule+ for Windows 1.0 (from Windows for Workgroup 3.1 Thai Edition)

        API:
            KEYBOARD.17     lstrKbdThaiToEng
            KEYBOARD.18     lstrKbdEngToThai
            USER.474        GetTaskInterface
            USER.485        FindThaiWordBreak

- Microsoft Write 3.1 Thai Edition (TWRITE.EXE) from Microsoft Windows 3.1 Thai Edition

        API:
            USER.485        FindThaiWordBreak

- Microsoft Word 6.0a Thai Edition

        API:
            GDI.490         IsThaiFont

- Notepad (NOTEPAD.EXE) from Microsoft Windows 3.1 Thai Edition

        API:
            USER.474        GetTaskInterface
            USER.498        SetTaskInterface
