#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <shellapi.h>
#include <richedit.h>
#include <shlobj.h>
#include "ConvFunc.h"
#include "StackFunc.h"
#include "StrFunc.h"
#include "AkelPad.h"
#include "Edit.h"


//Include conversion functions
#define xatoiA
#define xatoiW
#define xuitoaA
#define xuitoaW
#define hex2decA
#define hex2decW
#include "ConvFunc.h"

//Include stack functions
#define StackElement
#define StackInsert
#define StackMove
#define StackDelete
#define StackClear
#include "StackFunc.h"

//Include string functions
#define xstrcmpnA
#define xstrcmpnW
#include "StrFunc.h"

//Process
HANDLE hHeap;
HANDLE hMutex=0;

//WinMain
HINSTANCE hInstance;
char *lpCmdLine;
wchar_t *wlpCmdLine;
DWORD dwCmdShow;
STARTUPINFOA lpStartupInfo;
STARTUPINFOW lpStartupInfoW;
BOOL bNotepadCommandLine=TRUE;

//Versions
BOOL bOldWindows;
BOOL bOldRichEdit;
BOOL bOldComctl32;

//Buffers
char buf[BUFFER_SIZE];
wchar_t wbuf[BUFFER_SIZE];
char buf2[BUFFER_SIZE];
wchar_t wbuf2[BUFFER_SIZE];
unsigned char pcTranslateBuffer[TRANSLATE_BUFFER_SIZE];

//Language
char szLangModule[MAX_PATH]="";
wchar_t wszLangModule[MAX_PATH]=L"";
int nLangModules=0;
HMODULE hLangLib;
DWORD dwLangSystem;
DWORD dwLangCodepageRecognition;

//Procedures
HSTACK hMainProcStack={0};
HSTACK hMainProcRetStack={0};
HSTACK hEditProcStack={0};
HSTACK hEditProcRetStack={0};
HSTACK hFrameProcStack={0};
HSTACK hFrameProcRetStack={0};
WNDPROC lpfnMainProcA;
WNDPROC lpfnMainProcW;
WNDPROCRET lpfnMainProcRetA;
WNDPROCRET lpfnMainProcRetW;
WNDPROC lpfnFrameProcA;
WNDPROC lpfnFrameProcW;
WNDPROCRET lpfnFrameProcRetA;
WNDPROCRET lpfnFrameProcRetW;
WNDPROC lpfnEditProcA;
WNDPROC lpfnEditProcW;
WNDPROCRET lpfnEditProcRetA;
WNDPROCRET lpfnEditProcRetW;

//Plugins
HSTACK hPluginsStack={0};
HSTACK hPluginListStack={0};
HSTACK hHandlesStack={0};
HHOOK hHookPlugins;
HWND hWndHotkey;
BOOL bSavePluginsStackOnExit=FALSE;

//INI
HSTACK hIniStack={0};
char szIniFile[MAX_PATH];
wchar_t wszIniFile[MAX_PATH];
int nSaveSettings=SS_REGISTRY;

//Main Window
HWND hMainWnd;
HWND hWndEdit=NULL;
HWND hDummyWindow;
HWND hStatus;
HWND hDlgModeless=NULL;
RECT rcMainWindowRestored={CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT};
DWORD dwMainStyle=0;
DWORD dwLastMainSize=0;
int nStatusHeight=0;
HACCEL hGlobalAccel;
HACCEL hMainAccel;
HICON hMainIcon;
HICON hIconEmpty;
HCURSOR hCursorDragMove;
HMENU hMainMenu;
HMENU hPopupMenu;
HMENU hPopupEdit;
HMENU hPopupView;
HMENU hPopupCodepage;
HMENU hPopupHeadline;
HMENU hMenuRecentFiles=NULL;
HMENU hMenuLanguage=NULL;
BOOL bMenuPopupCodepage=TRUE;
BOOL bMenuRecentFiles=FALSE;
BOOL bMenuLanguage=FALSE;
BOOL bMainOnStartFinish=FALSE;

//Docks
HDOCK hDocksStack={0};
NSIZE nsSize;
WNDPROC OldDockProc=NULL;

//Codepages
int *lpCodepageList=NULL;
int nCodepageListLen=0;
BOOL bDefaultBOM;
BOOL bCurrentBOM;
int nCurrentCodePage;
int nDefaultCodePage;
int nAnsiCodePage;
int nOemCodePage;
DWORD dwCodepageRecognitionBuffer=DETECT_CODEPAGE_SIZE;
BOOL bRecodeDlg;

//Recent files
char szCurrentFile[MAX_PATH]="";
wchar_t wszCurrentFile[MAX_PATH]=L"";
char (*lpszRecentNames)[MAX_PATH]=NULL;
wchar_t (*lpwszRecentNames)[MAX_PATH]=NULL;
DWORD *lpdwRecentPositions=NULL;
DWORD *lpdwRecentCodepages=NULL;
int nRecentFiles=RECENTFILES_AMOUNT;
BOOL bSavePositions=TRUE;
BOOL bSaveCodepages=TRUE;

//Open/Save dialog
OPENFILENAMEA ofnA={0};
OPENFILENAMEW ofnW={0};
char szFileBuffer[FILELIST_BUFFER_SIZE];
wchar_t wszFileBuffer[FILELIST_BUFFER_SIZE];
char szFilter[MAX_PATH];
wchar_t wszFilter[MAX_PATH];
char szHomeDir[MAX_PATH]="";
wchar_t wszHomeDir[MAX_PATH]=L"";
BOOL bAutodetect=TRUE;
BOOL bSaveDlg;
DWORD dwOfnFlags;
BOOL bOfnBOM=FALSE;
int nOfnCodePage;
int nMsgCreate=AUTOANSWER_ASK;
int nMsgBinary=AUTOANSWER_ASK;
BOOL bSaveInReadOnlyMsg=FALSE;
WNDPROC OldPreviewProc;

//Find/Replace dialog
DWORD ftflags=FR_DOWN;
BOOL bReplaceDlg;
BOOL bReplaceAllAndClose=FALSE;
int nSearchStrings=SEARCHSTRINGS_AMOUNT;
char *szFind_orig=NULL;
wchar_t *wszFind_orig=NULL;
char *szReplace_orig=NULL;
wchar_t *wszReplace_orig=NULL;
char *szFind=NULL;
wchar_t *wszFind=NULL;
char *szReplace=NULL;
wchar_t *wszReplace=NULL;
WNDPROC OldComboboxEdit;

//Options dialog
PROPSHEETHEADERA pshA={0};
PROPSHEETHEADERW pshW={0};
PROPSHEETPAGEA pspA[3]={0};
PROPSHEETPAGEW pspW[3]={0};
HHOOK hHookOptions;
BOOL bOptionsRestart;

//Font/Color
LOGFONTA lfEditFontA;
LOGFONTW lfEditFontW;
CHOOSEFONTA cfA={0};
CHOOSEFONTW cfW={0};
CHOOSECOLORA ccA={0};
CHOOSECOLORW ccW={0};
AECOLORS aecColors={0};
COLORREF crCustColors[16]={0};
BOOL bEditFontChanged=FALSE;
BOOL bColorsChanged=FALSE;

//Print
LOGFONTA lfPrintFontA;
LOGFONTW lfPrintFontW;
PAGESETUPDLGA psdPageA={0};
PAGESETUPDLGW psdPageW={0};
PRINTDLGA pdA={0};
PRINTDLGW pdW={0};
char szPrintHeader[MAX_PATH]=PRINT_HEADERA;
wchar_t wszPrintHeader[MAX_PATH]=PRINT_HEADERW;
char szPrintFooter[MAX_PATH]=PRINT_FOOTERA;
wchar_t wszPrintFooter[MAX_PATH]=PRINT_FOOTERW;
BOOL bPrintFontEnable=FALSE;
BOOL bPrintHeaderEnable=FALSE;
BOOL bPrintFooterEnable=FALSE;
BOOL bGlobalPrint=FALSE;
BOOL bPrintFontChanged=FALSE;

//Edit position
CHARRANGE chrg={0};
int nPosition=0;
int nPositionOld=0;
int nMaxChars=0x0FFFFFFF;
int nMcstep=0x0FFFFFFF;

//Edit state
BOOL bModified=FALSE;
BOOL bInsertState=FALSE;
BOOL bInsertStateChange=FALSE;
int nNewLine=NEWLINE_WIN;
BOOL bWordWrap=FALSE;
BOOL bOnTop=FALSE;
BOOL bStatusBar=TRUE;
BOOL bReadOnly=FALSE;
BOOL bSaveTime=FALSE;
BOOL bWatchFile=FALSE;
BOOL bSingleOpenFile=FALSE;
BOOL bSingleOpenProgram=TRUE;
BOOL bKeepSpace=FALSE;
int nTabStopSize=EDIT_TABSTOPS;
BOOL bTabStopAsSpaces=FALSE;
int nUndoLimit=EDIT_UNDOLIMIT;
BOOL bDetailedUndo=FALSE;
BOOL bShowURL=FALSE;
int nClickURL=2;
DWORD dwEditMargins=EDIT_MARGINS;
FILETIME ftFileTime={0};
WNDPROC OldEditProc;

//Word breaking
wchar_t wszDelimiters[DELIMITERS_SIZE]=WORD_DELIMITERSW;
BOOL bDelimitersEnable=TRUE;
BOOL bFirstWordBreak=TRUE;

//Execute
char szCommand[BUFFER_SIZE]="";
wchar_t wszCommand[BUFFER_SIZE]=L"";
char szWorkDir[MAX_PATH]="";
wchar_t wszWorkDir[MAX_PATH]=L"";
char szExeDir[MAX_PATH]="";
wchar_t wszExeDir[MAX_PATH]=L"";

//Associations
char szFileTypesOpen[MAX_PATH]=ASSOCIATE_OPENA;
wchar_t wszFileTypesOpen[MAX_PATH]=ASSOCIATE_OPENW;
char szFileTypesEdit[MAX_PATH]=ASSOCIATE_EDITA;
wchar_t wszFileTypesEdit[MAX_PATH]=ASSOCIATE_EDITW;
char szFileTypesPrint[MAX_PATH]=ASSOCIATE_PRINTA;
wchar_t wszFileTypesPrint[MAX_PATH]=ASSOCIATE_PRINTW;
DWORD dwFileTypesAssociated=0;

//OLE Drag'n'Drop
IRichEditOleCallbackVtbl REOLEVtbl={
  REOLE_QueryInterface,
  REOLE_AddRef,
  REOLE_Release,
  REOLE_GetNewStorage,
  REOLE_GetInPlaceContext,
  REOLE_ShowContainerUI,
  REOLE_QueryInsertObject,
  REOLE_DeleteObject,
  REOLE_QueryAcceptData,
  REOLE_ContextSensitiveHelp,
  REOLE_GetClipboardData,
  REOLE_GetDragDropEffect,
  REOLE_GetContextMenu
};
REOLE rl={&REOLEVtbl, 0};

//Mdi
BOOL bMDI=FALSE;
BOOL bRegMDI=FALSE;
HWND hMdiClient=NULL;
HWND hWndFrameActive=0;
HWND hWndFrameDestroyed=0;
BOOL bMdiMaximize;
BOOL bMdiNoWindows=FALSE;
BOOL bMdiReopen=FALSE;
BOOL bMdiClientRedraw=TRUE;
HWND hTab=NULL;
int nTabView=TAB_VIEW_TOP;
int nTabType=TAB_TYPE_STANDARD;
int nTabSwitch=TAB_SWITCH_NEXTPREV;
HIMAGELIST hImageList;
BOOL bTabPressed=FALSE;
BOOL bFileExitError;
DWORD dwMdiStyle=WS_MAXIMIZE;
WNDPROC OldMdiClientProc;
WNDPROC OldTabProc;
WNDFRAMEA *lpWndFrameA;
WNDFRAMEW *lpWndFrameW;

//GetProcAddress
BOOL (WINAPI *GetCPInfoExAPtr)(UINT, DWORD, LPCPINFOEXA);
BOOL (WINAPI *GetCPInfoExWPtr)(UINT, DWORD, LPCPINFOEXW);
DWORD (WINAPI *GetLongPathNameAPtr)(char *, char *, DWORD);
DWORD (WINAPI *GetLongPathNameWPtr)(wchar_t *, wchar_t *, DWORD);
UINT (WINAPI *ExtractIconExAPtr)(char *, int, HICON *, HICON *, UINT);
UINT (WINAPI *ExtractIconExWPtr)(wchar_t *, int, HICON *, HICON *, UINT);
BOOL (WINAPI *SHGetPathFromIDListAPrt)(LPCITEMIDLIST, char *);
BOOL (WINAPI *SHGetPathFromIDListWPrt)(LPCITEMIDLIST, wchar_t *);
LPITEMIDLIST (WINAPI *SHBrowseForFolderAPrt)(LPBROWSEINFOA);
LPITEMIDLIST (WINAPI *SHBrowseForFolderWPrt)(LPBROWSEINFOW);


//GCC
int main()
{
  return 0;
}

//Entry point
extern "C" void _WinMain()
{
  MSG msg;
  BOOL bMsgStatus;
  HMODULE hKernel32=0;
  HMODULE hShell32=0;
  HMODULE hRichLib=0;
  HWND hWndFriend=0;
  BOOL bExit=FALSE;
  int nMajor;
  int nMinor;
  int nRelease;
  int nBuild;
  int i;

  //Process
  hHeap=GetProcessHeap();

  //Is unicode Windows
  if (GetWindowsDirectoryW(NULL, 0))
    bOldWindows=FALSE;
  else
    bOldWindows=TRUE;

  if (bOldWindows)
  {
    WNDCLASSA wndclassA={0};
    char *pCmdLine;

    //Set default options before reading from registry
    GetObjectA(GetStockObject(SYSTEM_FONT), sizeof(LOGFONTA), &lfEditFontA);
    lfEditFontA.lfHeight=-mod(lfEditFontA.lfHeight);
    lfEditFontA.lfWidth=0;
    memcpy(&lfPrintFontA, &lfEditFontA, sizeof(LOGFONTA));
    aecColors.crBasicText=GetSysColor(COLOR_WINDOWTEXT);
    aecColors.crBasicBk=GetSysColor(COLOR_WINDOW);
    nAnsiCodePage=GetACP();
    nOemCodePage=GetOEMCP();
    nDefaultCodePage=nAnsiCodePage;
    bDefaultBOM=FALSE;
    dwLangCodepageRecognition=dwLangSystem=GetUserDefaultLangID();

    if (GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, buf, BUFFER_SIZE))
    {
      if (*buf == '0')
      {
        psdPageA.rtMargin.left=1000;
        psdPageA.rtMargin.right=1000;
        psdPageA.rtMargin.top=1000;
        psdPageA.rtMargin.bottom=1000;
        psdPageA.Flags=PSD_INHUNDREDTHSOFMILLIMETERS;
      }
      else
      {
        psdPageA.rtMargin.left=500;
        psdPageA.rtMargin.right=500;
        psdPageA.rtMargin.top=500;
        psdPageA.rtMargin.bottom=500;
        psdPageA.Flags=PSD_INTHOUSANDTHSOFINCHES;
      }
    }

    //Get program HINSTANCE
    hInstance=GetModuleHandleA(NULL);

    //Get program directory
    GetExeDirA(hInstance, szExeDir, MAX_PATH);

    //Read options
    wsprintfA(szIniFile, "%s\\AkelPad.ini", szExeDir);
    if (OpenIniA(&hIniStack, szIniFile))
      IniGetValueA(&hIniStack, "Options", "SaveSettings", INI_DWORD, (LPBYTE)&nSaveSettings, sizeof(DWORD));
    if (nSaveSettings == SS_REGISTRY)
    {
      RegReadOptionsA();
      RegRegisterPluginsHotkeysA();
    }
    else
    {
      IniReadOptionsA();
      IniRegisterPluginsHotkeysA();
    }
    StackFreeIni(&hIniStack);

    aecColors.dwFlags=AECLR_ALL;
    aecColors.crCaret=RGB(0x00, 0x00, 0x00);
    aecColors.crSelText=GetSysColor(COLOR_HIGHLIGHTTEXT);
    aecColors.crSelBk=GetSysColor(COLOR_HIGHLIGHT);
    aecColors.crActiveLineText=aecColors.crBasicText;
    aecColors.crActiveLineBk=aecColors.crBasicBk;
    aecColors.crUrlText=RGB(0x00, 0x00, 0xFF);
    if (nDefaultCodePage == CP_UNICODE_UCS2_LE || nDefaultCodePage == CP_UNICODE_UCS2_BE || nDefaultCodePage == CP_UNICODE_UTF8)
      bDefaultBOM=TRUE;
    bCurrentBOM=bDefaultBOM;
    nCurrentCodePage=nDefaultCodePage;
    bMDI=bRegMDI;
    if (!lpCodepageList) nCodepageListLen=EnumCodepageList(&lpCodepageList);

    //Normalize search flags
    ftflags=(ftflags & FR_DOWN) |
            (ftflags & FR_MATCHCASE) |
            (ftflags & FR_UP) |
            (ftflags & FR_BEGINNING) |
            (ftflags & FR_SELECTION) |
            (ftflags & FR_ESCAPESEQ) |
            (ftflags & FR_ALLFILES);

    //Get startup info
    lpStartupInfo.cb=sizeof(STARTUPINFOA);
    GetStartupInfoA(&lpStartupInfo);
    dwCmdShow=lpStartupInfo.wShowWindow;
    if (!dwCmdShow) dwCmdShow=SW_SHOWNORMAL;

    //Mutex
    if (!(lpStartupInfo.dwFlags & STARTF_NOMUTEX))
    {
      if (hMutex=CreateMutexA(NULL, TRUE, APP_MUTEXA))
      {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
          WaitForSingleObject(hMutex, INFINITE);
          ReleaseMutex(hMutex);
          CloseHandle(hMutex);
          hMutex=0;
        }
      }
    }

    //Get command line
    pCmdLine=lpCmdLine=GetCommandLineParamsA();

    //Get rich edit version
    GetFileVersionA("riched20.dll", &nMajor, &nMinor, &nRelease, &nBuild);
    if (nMajor < 5 || (nMajor == 5 && nMinor < 30))
      bOldRichEdit=TRUE;
    else
      bOldRichEdit=FALSE;

    //Get common controls version
    GetFileVersionA("comctl32.dll", &nMajor, &nMinor, &nRelease, &nBuild);
    if (nMajor < 4 || (nMajor == 4 && nMinor < 71))
      bOldComctl32=TRUE;
    else
      bOldComctl32=FALSE;

    //Get functions addresses
    hKernel32=GetModuleHandleA("kernel32.dll");
    GetCPInfoExAPtr=(BOOL (WINAPI *)(UINT, DWORD, LPCPINFOEXA))GetProcAddress(hKernel32, "GetCPInfoExA");
    GetLongPathNameAPtr=(DWORD (WINAPI *)(char *, char *, DWORD))GetProcAddress(hKernel32, "GetLongPathNameA");

    hShell32=GetModuleHandleA("shell32.dll");
    ExtractIconExAPtr=(UINT (WINAPI *)(char *, int, HICON *, HICON *, UINT))GetProcAddress(hShell32, "ExtractIconExA");
    SHGetPathFromIDListAPrt=(BOOL (WINAPI *)(LPCITEMIDLIST, char *))GetProcAddress(hShell32, "SHGetPathFromIDListA");
    SHBrowseForFolderAPrt=(LPITEMIDLIST (WINAPI *)(LPBROWSEINFOA))GetProcAddress(hShell32, "SHBrowseForFolderA");

    //Get command line arguments
    if (bMDI)
    {
      if (bSingleOpenProgram)
      {
        if (hWndFriend=FindWindowA(APP_MAIN_CLASSA, NULL))
        {
          ActivateWindow(hWndFriend);
          SendMessage(hWndFriend, AKDN_ACTIVATE, 0, 0);
          bExit=TRUE;
        }
      }
    }

    if (*pCmdLine)
    {
      while (GetCommandLineArgA(pCmdLine, buf, BUFFER_SIZE, NULL, NULL, &pCmdLine, bNotepadCommandLine))
      {
        if (buf[0] == '/')
        {
          if (!lstrcmpiA(buf, "/P"))
          {
            bGlobalPrint=TRUE;
          }
          else if (!lstrcmpiA(buf, "/C+"))
          {
            nMsgCreate=AUTOANSWER_YES;
          }
          else if (!lstrcmpiA(buf, "/C-"))
          {
            nMsgCreate=AUTOANSWER_NO;
          }
          else if (!lstrcmpiA(buf, "/B+"))
          {
            nMsgBinary=AUTOANSWER_YES;
          }
          else if (!lstrcmpiA(buf, "/B-"))
          {
            nMsgBinary=AUTOANSWER_NO;
          }
          else if (!lstrcmpiA(buf, "/X"))
          {
            bNotepadCommandLine=FALSE;
          }
          else if (!lstrcmpiA(buf, "/REASSOC"))
          {
            if (dwFileTypesAssociated & AE_OPEN)
            {
              AssociateFileTypesA(hInstance, szFileTypesOpen, AE_OPEN|AE_ASSOCIATE);
            }
            else if (dwFileTypesAssociated & AE_EDIT)
            {
              AssociateFileTypesA(hInstance, szFileTypesEdit, AE_EDIT|AE_ASSOCIATE);
            }
            else if (dwFileTypesAssociated & AE_PRINT)
            {
              AssociateFileTypesA(hInstance, szFileTypesPrint, AE_PRINT|AE_ASSOCIATE);
            }
            if (dwFileTypesAssociated) SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);
          }
          else if (!lstrcmpiA(buf, "/DEASSOC"))
          {
            if (dwFileTypesAssociated & AE_OPEN)
            {
              AssociateFileTypesA(hInstance, szFileTypesOpen, AE_OPEN|AE_DEASSOCIATE);
            }
            else if (dwFileTypesAssociated & AE_EDIT)
            {
              AssociateFileTypesA(hInstance, szFileTypesEdit, AE_EDIT|AE_DEASSOCIATE);
            }
            else if (dwFileTypesAssociated & AE_PRINT)
            {
              AssociateFileTypesA(hInstance, szFileTypesPrint, AE_PRINT|AE_DEASSOCIATE);
            }
            if (dwFileTypesAssociated) SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);
          }
          else if (!lstrcmpiA(buf, "/QUIT"))
          {
            goto Quit;
          }
          else if (!lstrcmpiA(buf, "/END"))
          {
            break;
          }
          continue;
        }
        if (!*buf) continue;
        lstrcpynA(szFileBuffer, buf, MAX_PATH);

        if (!bMDI)
        {
          if (bSingleOpenFile)
          {
            if (GetFullNameA(szFileBuffer))
            {
              if ((hWndFriend=FindWindowA(APP_SDI_CLASSA, szFileBuffer)) &&
                  (hWndFriend=GetParent(hWndFriend)))
              {
                ActivateWindow(hWndFriend);
                SendMessage(hWndFriend, AKDN_ACTIVATE, 0, 0);

                if (hWndFriend)
                {
                  if (bGlobalPrint) SendMessage(hWndFriend, AKD_SETFILEPRINT, TRUE, 0);
                  if (nMsgCreate != AUTOANSWER_ASK) SendMessage(hWndFriend, AKD_SETMSGCREATE, (WPARAM)nMsgCreate, 0);
                  if (nMsgBinary != AUTOANSWER_ASK) SendMessage(hWndFriend, AKD_SETMSGBINARY, (WPARAM)nMsgBinary, 0);

                  OpenDocumentSendA(hWndFriend, NULL, szFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, 0, TRUE);
                }
                if (!*pCmdLine) goto Quit;
                continue;
              }
            }
          }
          if (!*pCmdLine) break;

          hWndFriend=DoFileNewWindowA(STARTF_NOMUTEX);
        }
        else
        {
          if (!hWndFriend)
          {
            if (!*pCmdLine) break;
            if (hWndFriend=DoFileNewWindowA(STARTF_NOMUTEX))
              bExit=TRUE;
          }
        }

        if (hWndFriend)
        {
          if (bGlobalPrint) SendMessage(hWndFriend, AKD_SETFILEPRINT, TRUE, 0);
          if (nMsgCreate != AUTOANSWER_ASK) SendMessage(hWndFriend, AKD_SETMSGCREATE, (WPARAM)nMsgCreate, 0);
          if (nMsgBinary != AUTOANSWER_ASK) SendMessage(hWndFriend, AKD_SETMSGBINARY, (WPARAM)nMsgBinary, 0);

          OpenDocumentSendA(hWndFriend, NULL, szFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, 0, TRUE);
        }
      }
    }
    if (bExit) goto Quit;

    //Load DLL's
    hLangLib=hInstance;

    if (*szLangModule)
    {
      wsprintfA(buf, "%s\\AkelFiles\\Langs\\%s", szExeDir, szLangModule);

      if (!(hLangLib=LoadLibraryA(buf)))
      {
        hLangLib=hInstance;
        API_LoadStringA(hLangLib, MSG_ERROR_LOAD_DLL, buf, BUFFER_SIZE);
        wsprintfA(buf2, buf, szLangModule);
        MessageBoxA(NULL, buf2, APP_MAIN_TITLEA, MB_OK|MB_ICONEXCLAMATION);
      }
    }

    if (!(hRichLib=LoadLibraryA("riched20.dll")))
    {
      API_LoadStringA(hLangLib, MSG_ERROR_LOAD_DLL, buf, BUFFER_SIZE);
      wsprintfA(buf2, buf, "riched20.dll");
      MessageBoxA(NULL, buf2, APP_MAIN_TITLEA, MB_OK|MB_ICONERROR);
      goto Quit;
    }

    //GetOpenFileName dialog file filter
    API_LoadStringA(hLangLib, STR_FILE_FILTER, szFilter, MAX_PATH);
    for (i=0; szFilter[i]; ++i)
      if (szFilter[i] == '|') szFilter[i]='\0';
    szFilter[++i]='\0';

    hGlobalAccel=API_LoadAcceleratorsA(hLangLib, MAKEINTRESOURCEA(IDA_ACCEL_GLOBAL));
    hMainAccel=API_LoadAcceleratorsA(hLangLib, MAKEINTRESOURCEA(IDA_ACCEL_MAIN));
    hMainIcon=API_LoadIconA(hLangLib, MAKEINTRESOURCEA(IDI_ICON_MAIN));
    hMainMenu=API_LoadMenuA(hLangLib, MAKEINTRESOURCEA(IDM_MENU_MAIN));
    hPopupMenu=API_LoadMenuA(hLangLib, MAKEINTRESOURCEA(IDM_MENU_POPUP));
    if (bMDI)
    {
      hIconEmpty=(HICON)API_LoadImageA(hLangLib, MAKEINTRESOURCEA(IDI_ICON_EMPTY), IMAGE_ICON, 0, 0, 0);
      hCursorDragMove=(HCURSOR)API_LoadImageA(hLangLib, MAKEINTRESOURCEA(IDC_CURSOR_DRAGMOVE), IMAGE_CURSOR, 0, 0, 0);
    }

    lpfnMainProcA=MainProcA;
    lpfnMainProcRetA=NULL;
    lpfnFrameProcA=FrameProcA;
    lpfnFrameProcRetA=NULL;
    lpfnEditProcA=EditProcA;
    lpfnEditProcRetA=NULL;
    StackProcSet(&hMainProcStack, MainProcA, NULL, NULL);
    StackProcSet(&hEditProcStack, EditProcA, NULL, NULL);
    StackProcSet(&hFrameProcStack, FrameProcA, NULL, NULL);

    InitCommonControls();

    wndclassA.style        =CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
    wndclassA.lpfnWndProc  =CommonMainProcA;
    wndclassA.cbClsExtra   =0;
    wndclassA.cbWndExtra   =DLGWINDOWEXTRA;
    wndclassA.hInstance    =hInstance;
    wndclassA.hIcon        =hMainIcon;
    wndclassA.hCursor      =LoadCursor(NULL, IDC_ARROW);
    wndclassA.hbrBackground=(HBRUSH)GetStockObject(HOLLOW_BRUSH);
    wndclassA.lpszMenuName =MAKEINTRESOURCEA(IDM_MENU_MAIN);
    wndclassA.lpszClassName=APP_MAIN_CLASSA;
    if (!RegisterClassA(&wndclassA)) goto Quit;

    if (!bMDI)
    {
      wndclassA.style        =0;
      wndclassA.lpfnWndProc  =DummyProcA;
      wndclassA.cbClsExtra   =0;
      wndclassA.cbWndExtra   =DLGWINDOWEXTRA;
      wndclassA.hInstance    =hInstance;
      wndclassA.hIcon        =NULL;
      wndclassA.hCursor      =NULL;
      wndclassA.hbrBackground=NULL;
      wndclassA.lpszMenuName =NULL;
      wndclassA.lpszClassName=APP_SDI_CLASSA;
      if (!RegisterClassA(&wndclassA)) goto Quit;
    }
    else
    {
      wndclassA.style        =CS_HREDRAW|CS_VREDRAW;
      wndclassA.lpfnWndProc  =CommonFrameProcA;
      wndclassA.cbClsExtra   =0;
      wndclassA.cbWndExtra   =DLGWINDOWEXTRA;
      wndclassA.hInstance    =hInstance;
      wndclassA.hIcon        =NULL;
      wndclassA.hCursor      =NULL;
      wndclassA.hbrBackground=NULL;
      wndclassA.lpszMenuName =NULL;
      wndclassA.lpszClassName=APP_MDI_CLASSA;
      if (!RegisterClassA(&wndclassA)) goto Quit;
    }

    hMainWnd=CreateWindowA(APP_MAIN_CLASSA,             // window class name
                          APP_MAIN_TITLEA,              // window caption
                          WS_OVERLAPPEDWINDOW,          // window style
                          rcMainWindowRestored.left,    // initial x position
                          rcMainWindowRestored.top,     // initial y position
                          rcMainWindowRestored.right,   // initial x size
                          rcMainWindowRestored.bottom,  // initial y size
                          NULL,                         // parent window handle
                          hMainMenu,                    // window menu handle
                          hInstance,                    // program instance handle
                          NULL);                        // creation parameters
    if (!hMainWnd) goto Quit;

    while ((bMsgStatus=GetMessageA(&msg, NULL, 0, 0)) && bMsgStatus != -1)
    {
      if (!TranslateAccelerator(hMainWnd, hGlobalAccel, &msg))
      {
        if (!TranslateDialogA(&hDocksStack, &msg))
        {
          if (!TranslatePluginA(&msg))
          {
            if (!TranslateHotkeyA(&hPluginsStack, &msg))
            {
              if (!TranslateAccelerator(hMainWnd, hMainAccel, &msg))
              {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
              }
            }
          }
        }
      }
      if (bMainOnStartFinish)
      {
        if (GetQueueStatus(QS_ALLINPUT) == 0)
        {
          bMainOnStartFinish=FALSE;
          SendMessage(hMainWnd, AKDN_MAIN_ONSTART_IDLE, 0, 0);
        }
      }
    }
    if (bMsgStatus == -1)
    {
      API_LoadStringA(hLangLib, MSG_ERROR_IN_MESSAGE_QUEUE, buf, BUFFER_SIZE);
      MessageBoxA(NULL, buf, APP_MAIN_TITLEA, MB_OK|MB_ICONERROR);
    }
  }
  else
  {
    WNDCLASSW wndclassW={0};
    wchar_t *wpCmdLine;

    //Set default options before reading from registry
    GetObjectW(GetStockObject(SYSTEM_FONT), sizeof(LOGFONTW), &lfEditFontW);
    lfEditFontW.lfHeight=-mod(lfEditFontW.lfHeight);
    lfEditFontW.lfWidth=0;
    memcpy(&lfPrintFontW, &lfEditFontW, sizeof(LOGFONTW));
    aecColors.crBasicText=GetSysColor(COLOR_WINDOWTEXT);
    aecColors.crBasicBk=GetSysColor(COLOR_WINDOW);
    nAnsiCodePage=GetACP();
    nOemCodePage=GetOEMCP();
    nDefaultCodePage=nAnsiCodePage;
    bDefaultBOM=FALSE;
    dwLangCodepageRecognition=dwLangSystem=GetUserDefaultLangID();

    if (GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, wbuf, BUFFER_SIZE))
    {
      if (*wbuf == '0')
      {
        psdPageW.rtMargin.left=1000;
        psdPageW.rtMargin.right=1000;
        psdPageW.rtMargin.top=1000;
        psdPageW.rtMargin.bottom=1000;
        psdPageW.Flags=PSD_INHUNDREDTHSOFMILLIMETERS;
      }
      else
      {
        psdPageW.rtMargin.left=500;
        psdPageW.rtMargin.right=500;
        psdPageW.rtMargin.top=500;
        psdPageW.rtMargin.bottom=500;
        psdPageW.Flags=PSD_INTHOUSANDTHSOFINCHES;
      }
    }

    //Get program HINSTANCE
    hInstance=GetModuleHandleW(NULL);

    //Get program directory
    GetExeDirW(hInstance, wszExeDir, MAX_PATH);

    //Read options
    wsprintfW(wszIniFile, L"%s\\AkelPad.ini", wszExeDir);
    if (OpenIniW(&hIniStack, wszIniFile))
      IniGetValueW(&hIniStack, L"Options", L"SaveSettings", INI_DWORD, (LPBYTE)&nSaveSettings, sizeof(DWORD));
    if (nSaveSettings == SS_REGISTRY)
    {
      RegReadOptionsW();
      RegRegisterPluginsHotkeysW();
    }
    else
    {
      IniReadOptionsW();
      IniRegisterPluginsHotkeysW();
    }
    StackFreeIni(&hIniStack);

    aecColors.dwFlags=AECLR_ALL;
    aecColors.crCaret=RGB(0x00, 0x00, 0x00);
    aecColors.crSelText=GetSysColor(COLOR_HIGHLIGHTTEXT);
    aecColors.crSelBk=GetSysColor(COLOR_HIGHLIGHT);
    aecColors.crActiveLineText=aecColors.crBasicText;
    aecColors.crActiveLineBk=aecColors.crBasicBk;
    aecColors.crUrlText=RGB(0x00, 0x00, 0xFF);
    if (nDefaultCodePage == CP_UNICODE_UCS2_LE || nDefaultCodePage == CP_UNICODE_UCS2_BE || nDefaultCodePage == CP_UNICODE_UTF8)
      bDefaultBOM=TRUE;
    bCurrentBOM=bDefaultBOM;
    nCurrentCodePage=nDefaultCodePage;
    bMDI=bRegMDI;
    if (!lpCodepageList) nCodepageListLen=EnumCodepageList(&lpCodepageList);

    //Normalize search flags
    ftflags=(ftflags & FR_DOWN) |
            (ftflags & FR_MATCHCASE) |
            (ftflags & FR_UP) |
            (ftflags & FR_BEGINNING) |
            (ftflags & FR_SELECTION) |
            (ftflags & FR_ESCAPESEQ) |
            (ftflags & FR_ALLFILES);

    //Get startup info
    lpStartupInfoW.cb=sizeof(STARTUPINFOW);
    GetStartupInfoW(&lpStartupInfoW);
    dwCmdShow=lpStartupInfoW.wShowWindow;
    if (!dwCmdShow) dwCmdShow=SW_SHOWNORMAL;

    //Mutex
    if (!(lpStartupInfoW.dwFlags & STARTF_NOMUTEX))
    {
      if (hMutex=CreateMutexW(NULL, TRUE, APP_MUTEXW))
      {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
          WaitForSingleObject(hMutex, INFINITE);
          ReleaseMutex(hMutex);
          CloseHandle(hMutex);
          hMutex=0;
        }
      }
    }

    //Get command line
    wpCmdLine=wlpCmdLine=GetCommandLineParamsW();

    //Get rich edit version
    GetFileVersionW(L"riched20.dll", &nMajor, &nMinor, &nRelease, &nBuild);
    if (nMajor < 5 || (nMajor == 5 && nMinor < 30))
      bOldRichEdit=TRUE;
    else
      bOldRichEdit=FALSE;

    //Get common controls version
    GetFileVersionW(L"comctl32.dll", &nMajor, &nMinor, &nRelease, &nBuild);
    if (nMajor < 4 || (nMajor == 4 && nMinor < 71))
      bOldComctl32=TRUE;
    else
      bOldComctl32=FALSE;

    //Get functions addresses
    hKernel32=GetModuleHandleW(L"kernel32.dll");
    GetCPInfoExWPtr=(BOOL (WINAPI *)(UINT, DWORD, LPCPINFOEXW))GetProcAddress(hKernel32, "GetCPInfoExW");
    GetLongPathNameWPtr=(DWORD (WINAPI *)(wchar_t *, wchar_t *, DWORD))GetProcAddress(hKernel32, "GetLongPathNameW");

    hShell32=GetModuleHandleW(L"shell32.dll");
    ExtractIconExWPtr=(UINT (WINAPI *)(wchar_t *, int, HICON *, HICON *, UINT))GetProcAddress(hShell32, "ExtractIconExW");
    SHGetPathFromIDListWPrt=(BOOL (WINAPI *)(LPCITEMIDLIST, wchar_t *))GetProcAddress(hShell32, "SHGetPathFromIDListW");
    SHBrowseForFolderWPrt=(LPITEMIDLIST (WINAPI *)(LPBROWSEINFOW))GetProcAddress(hShell32, "SHBrowseForFolderW");

    //Get command line arguments
    if (bMDI)
    {
      if (bSingleOpenProgram)
      {
        if (hWndFriend=FindWindowW(APP_MAIN_CLASSW, NULL))
        {
          ActivateWindow(hWndFriend);
          SendMessage(hWndFriend, AKDN_ACTIVATE, 0, 0);
          bExit=TRUE;
        }
      }
    }

    if (*wpCmdLine)
    {
      while (GetCommandLineArgW(wpCmdLine, wbuf, BUFFER_SIZE, NULL, NULL, &wpCmdLine, bNotepadCommandLine))
      {
        if (wbuf[0] == '/')
        {
          if (!lstrcmpiW(wbuf, L"/P"))
          {
            bGlobalPrint=TRUE;
          }
          else if (!lstrcmpiW(wbuf, L"/C+"))
          {
            nMsgCreate=AUTOANSWER_YES;
          }
          else if (!lstrcmpiW(wbuf, L"/C-"))
          {
            nMsgCreate=AUTOANSWER_NO;
          }
          else if (!lstrcmpiW(wbuf, L"/B+"))
          {
            nMsgBinary=AUTOANSWER_YES;
          }
          else if (!lstrcmpiW(wbuf, L"/B-"))
          {
            nMsgBinary=AUTOANSWER_NO;
          }
          else if (!lstrcmpiW(wbuf, L"/X"))
          {
            bNotepadCommandLine=FALSE;
          }
          else if (!lstrcmpiW(wbuf, L"/REASSOC"))
          {
            if (dwFileTypesAssociated & AE_OPEN)
            {
              AssociateFileTypesW(hInstance, wszFileTypesOpen, AE_OPEN|AE_ASSOCIATE);
            }
            else if (dwFileTypesAssociated & AE_EDIT)
            {
              AssociateFileTypesW(hInstance, wszFileTypesEdit, AE_EDIT|AE_ASSOCIATE);
            }
            else if (dwFileTypesAssociated & AE_PRINT)
            {
              AssociateFileTypesW(hInstance, wszFileTypesPrint, AE_PRINT|AE_ASSOCIATE);
            }
            if (dwFileTypesAssociated) SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);
          }
          else if (!lstrcmpiW(wbuf, L"/DEASSOC"))
          {
            if (dwFileTypesAssociated & AE_OPEN)
            {
              AssociateFileTypesW(hInstance, wszFileTypesOpen, AE_OPEN|AE_DEASSOCIATE);
            }
            else if (dwFileTypesAssociated & AE_EDIT)
            {
              AssociateFileTypesW(hInstance, wszFileTypesEdit, AE_EDIT|AE_DEASSOCIATE);
            }
            else if (dwFileTypesAssociated & AE_PRINT)
            {
              AssociateFileTypesW(hInstance, wszFileTypesPrint, AE_PRINT|AE_DEASSOCIATE);
            }
            if (dwFileTypesAssociated) SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);
          }
          else if (!lstrcmpiW(wbuf, L"/QUIT"))
          {
            goto Quit;
          }
          else if (!lstrcmpiW(wbuf, L"/END"))
          {
            break;
          }
          continue;
        }
        if (!*wbuf) continue;
        lstrcpynW(wszFileBuffer, wbuf, MAX_PATH);

        if (!bMDI)
        {
          if (bSingleOpenFile)
          {
            if (GetFullNameW(wszFileBuffer))
            {
              if ((hWndFriend=FindWindowW(APP_SDI_CLASSW, wszFileBuffer)) &&
                  (hWndFriend=GetParent(hWndFriend)))
              {
                ActivateWindow(hWndFriend);
                SendMessage(hWndFriend, AKDN_ACTIVATE, 0, 0);

                if (hWndFriend)
                {
                  if (bGlobalPrint) SendMessage(hWndFriend, AKD_SETFILEPRINT, TRUE, 0);
                  if (nMsgCreate != AUTOANSWER_ASK) SendMessage(hWndFriend, AKD_SETMSGCREATE, (WPARAM)nMsgCreate, 0);
                  if (nMsgBinary != AUTOANSWER_ASK) SendMessage(hWndFriend, AKD_SETMSGBINARY, (WPARAM)nMsgBinary, 0);

                  OpenDocumentSendW(hWndFriend, NULL, wszFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, 0, TRUE);
                }
                if (!*wpCmdLine) goto Quit;
                continue;
              }
            }
          }
          if (!*wpCmdLine) break;

          hWndFriend=DoFileNewWindowW(STARTF_NOMUTEX);
        }
        else
        {
          if (!hWndFriend)
          {
            if (!*wpCmdLine) break;
            if (hWndFriend=DoFileNewWindowW(STARTF_NOMUTEX))
              bExit=TRUE;
          }
        }

        if (hWndFriend)
        {
          if (bGlobalPrint) SendMessage(hWndFriend, AKD_SETFILEPRINT, TRUE, 0);
          if (nMsgCreate != AUTOANSWER_ASK) SendMessage(hWndFriend, AKD_SETMSGCREATE, (WPARAM)nMsgCreate, 0);
          if (nMsgBinary != AUTOANSWER_ASK) SendMessage(hWndFriend, AKD_SETMSGBINARY, (WPARAM)nMsgBinary, 0);

          OpenDocumentSendW(hWndFriend, NULL, wszFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, 0, TRUE);
        }
      }
    }
    if (bExit) goto Quit;

    //Load DLL's
    hLangLib=hInstance;

    if (*wszLangModule)
    {
      wsprintfW(wbuf, L"%s\\AkelFiles\\Langs\\%s", wszExeDir, wszLangModule);

      if (!(hLangLib=LoadLibraryW(wbuf)))
      {
        hLangLib=hInstance;
        API_LoadStringW(hLangLib, MSG_ERROR_LOAD_DLL, wbuf, BUFFER_SIZE);
        wsprintfW(wbuf2, wbuf, wszLangModule);
        MessageBoxW(NULL, wbuf2, APP_MAIN_TITLEW, MB_OK|MB_ICONEXCLAMATION);
      }
    }

    if (!(hRichLib=LoadLibraryW(L"riched20.dll")))
    {
      API_LoadStringW(hLangLib, MSG_ERROR_LOAD_DLL, wbuf, BUFFER_SIZE);
      wsprintfW(wbuf2, wbuf, L"riched20.dll");
      MessageBoxW(NULL, wbuf2, APP_MAIN_TITLEW, MB_OK|MB_ICONERROR);
      goto Quit;
    }

    //GetOpenFileName dialog file filter
    API_LoadStringW(hLangLib, STR_FILE_FILTER, wszFilter, MAX_PATH);
    for (i=0; wszFilter[i]; ++i)
      if (wszFilter[i] == '|') wszFilter[i]='\0';
    wszFilter[++i]='\0';

    hGlobalAccel=API_LoadAcceleratorsW(hLangLib, MAKEINTRESOURCEW(IDA_ACCEL_GLOBAL));
    hMainAccel=API_LoadAcceleratorsW(hLangLib, MAKEINTRESOURCEW(IDA_ACCEL_MAIN));
    hMainIcon=API_LoadIconW(hLangLib, MAKEINTRESOURCEW(IDI_ICON_MAIN));
    hMainMenu=API_LoadMenuW(hLangLib, MAKEINTRESOURCEW(IDM_MENU_MAIN));
    hPopupMenu=API_LoadMenuW(hLangLib, MAKEINTRESOURCEW(IDM_MENU_POPUP));
    if (bMDI)
    {
      hIconEmpty=(HICON)API_LoadImageW(hLangLib, MAKEINTRESOURCEW(IDI_ICON_EMPTY), IMAGE_ICON, 0, 0, 0);
      hCursorDragMove=(HCURSOR)API_LoadImageW(hLangLib, MAKEINTRESOURCEW(IDC_CURSOR_DRAGMOVE), IMAGE_CURSOR, 0, 0, 0);
    }

    lpfnMainProcW=MainProcW;
    lpfnMainProcRetW=NULL;
    lpfnFrameProcW=FrameProcW;
    lpfnFrameProcRetW=NULL;
    lpfnEditProcW=EditProcW;
    lpfnEditProcRetW=NULL;
    StackProcSet(&hMainProcStack, MainProcW, NULL, NULL);
    StackProcSet(&hEditProcStack, EditProcW, NULL, NULL);
    StackProcSet(&hFrameProcStack, FrameProcW, NULL, NULL);

    InitCommonControls();

    wndclassW.style        =CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
    wndclassW.lpfnWndProc  =CommonMainProcW;
    wndclassW.cbClsExtra   =0;
    wndclassW.cbWndExtra   =DLGWINDOWEXTRA;
    wndclassW.hInstance    =hInstance;
    wndclassW.hIcon        =hMainIcon;
    wndclassW.hCursor      =LoadCursor(NULL, IDC_ARROW);
    wndclassW.hbrBackground=(HBRUSH)GetStockObject(HOLLOW_BRUSH);
    wndclassW.lpszMenuName =MAKEINTRESOURCEW(IDM_MENU_MAIN);
    wndclassW.lpszClassName=APP_MAIN_CLASSW;
    if (!RegisterClassW(&wndclassW)) goto Quit;

    if (!bMDI)
    {
      wndclassW.style        =0;
      wndclassW.lpfnWndProc  =DummyProcW;
      wndclassW.cbClsExtra   =0;
      wndclassW.cbWndExtra   =DLGWINDOWEXTRA;
      wndclassW.hInstance    =hInstance;
      wndclassW.hIcon        =NULL;
      wndclassW.hCursor      =NULL;
      wndclassW.hbrBackground=NULL;
      wndclassW.lpszMenuName =NULL;
      wndclassW.lpszClassName=APP_SDI_CLASSW;
      if (!RegisterClassW(&wndclassW)) goto Quit;
    }
    else
    {
      wndclassW.style        =CS_HREDRAW|CS_VREDRAW;
      wndclassW.lpfnWndProc  =CommonFrameProcW;
      wndclassW.cbClsExtra   =0;
      wndclassW.cbWndExtra   =DLGWINDOWEXTRA;
      wndclassW.hInstance    =hInstance;
      wndclassW.hIcon        =NULL;
      wndclassW.hCursor      =NULL;
      wndclassW.hbrBackground=NULL;
      wndclassW.lpszMenuName =NULL;
      wndclassW.lpszClassName=APP_MDI_CLASSW;
      if (!RegisterClassW(&wndclassW)) goto Quit;
    }

    hMainWnd=CreateWindowW(APP_MAIN_CLASSW,              // window class name
                           APP_MAIN_TITLEW,              // window caption
                           WS_OVERLAPPEDWINDOW,          // window style
                           rcMainWindowRestored.left,    // initial x position
                           rcMainWindowRestored.top,     // initial y position
                           rcMainWindowRestored.right,   // initial x size
                           rcMainWindowRestored.bottom,  // initial y size
                           NULL,                         // parent window handle
                           hMainMenu,                    // window menu handle
                           hInstance,                    // program instance handle
                           NULL);                        // creation parameters
    if (!hMainWnd) goto Quit;

    while ((bMsgStatus=GetMessageW(&msg, NULL, 0, 0)) && bMsgStatus != -1)
    {
      if (!TranslateAccelerator(hMainWnd, hGlobalAccel, &msg))
      {
        if (!TranslateDialogW(&hDocksStack, &msg))
        {
          if (!TranslatePluginW(&msg))
          {
            if (!TranslateHotkeyW(&hPluginsStack, &msg))
            {
              if (!TranslateAcceleratorW(hMainWnd, hMainAccel, &msg))
              {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
              }
            }
          }
        }
      }
      if (bMainOnStartFinish)
      {
        if (GetQueueStatus(QS_ALLINPUT) == 0)
        {
          bMainOnStartFinish=FALSE;
          SendMessage(hMainWnd, AKDN_MAIN_ONSTART_IDLE, 0, 0);
        }
      }
    }
    if (bMsgStatus == -1)
    {
      API_LoadStringW(hLangLib, MSG_ERROR_IN_MESSAGE_QUEUE, wbuf, BUFFER_SIZE);
      MessageBoxW(NULL, wbuf, APP_MAIN_TITLEW, MB_OK|MB_ICONERROR);
    }
  }

  Quit:
  StackProcFree(&hMainProcStack);
  StackProcFree(&hMainProcRetStack);
  StackProcFree(&hEditProcStack);
  StackProcFree(&hEditProcRetStack);
  StackProcFree(&hFrameProcStack);
  StackProcFree(&hFrameProcRetStack);
  StackPluginFree(&hPluginsStack);
  StackDockFree(&hDocksStack);
  if (hLangLib && hLangLib != hInstance) FreeLibrary(hLangLib);
  if (hRichLib) FreeLibrary(hRichLib);
  ExitProcess(0);
}

LRESULT CALLBACK CommonMainProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult;

  if (uMsg >= WM_USER)
  {
    if (uMsg == AKD_GETMAINPROC)
    {
      return StackProcGet(&hMainProcStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETMAINPROC)
    {
      return StackProcSet(&hMainProcStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, &lpfnMainProcA);
    }
    else if (uMsg == AKD_GETMAINPROCRET)
    {
      return StackProcGet(&hMainProcRetStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETMAINPROCRET)
    {
      return StackProcSet(&hMainProcRetStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, (WNDPROC *)&lpfnMainProcRetA);
    }
    else if (uMsg == AKD_GETEDITPROC)
    {
      return StackProcGet(&hEditProcStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETEDITPROC)
    {
      return StackProcSet(&hEditProcStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, &lpfnEditProcA);
    }
    else if (uMsg == AKD_GETEDITPROCRET)
    {
      return StackProcGet(&hEditProcRetStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETEDITPROCRET)
    {
      return StackProcSet(&hEditProcRetStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, (WNDPROC *)&lpfnEditProcRetA);
    }
    else if (uMsg == AKD_GETFRAMEPROC)
    {
      return StackProcGet(&hFrameProcStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETFRAMEPROC)
    {
      return StackProcSet(&hFrameProcStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, &lpfnFrameProcA);
    }
    else if (uMsg == AKD_GETFRAMEPROCRET)
    {
      return StackProcGet(&hFrameProcRetStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETFRAMEPROCRET)
    {
      return StackProcSet(&hFrameProcRetStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, (WNDPROC *)&lpfnFrameProcRetA);
    }
    else if (uMsg == AKD_DLLCALL)
    {
      PLUGINCALLSENDA *pcs=(PLUGINCALLSENDA *)lParam;

      return (LRESULT)CallPluginReceiveSendA(pcs);
    }
    else if (uMsg == AKD_DLLFIND)
    {
      if (wParam)
        return (LRESULT)StackPluginFindA(&hPluginsStack, (char *)wParam);
      if (lParam)
        return (LRESULT)StackHotkeyFindA(&hPluginsStack, (WORD)lParam);
      return 0;
    }
    else if (uMsg == AKD_DLLADD)
    {
      PLUGINFUNCTIONA *pf=(PLUGINFUNCTIONA *)lParam;

      return (LRESULT)StackPluginAddA(&hPluginsStack, pf->szFunction, pf->nFunctionLen, pf->wHotkey, pf->bOnStart, pf->bRunning, pf->PluginProc, pf->lpParameter);
    }
    else if (uMsg == AKD_DLLDELETE)
    {
      StackPluginDelete(&hPluginsStack, (void *)lParam);
      return 0;
    }
    else if (uMsg == AKD_DLLSAVE)
    {
      if (lParam == DLLS_ONEXIT)
      {
        bSavePluginsStackOnExit=TRUE;
        return TRUE;
      }
      return StackPluginSaveA(&hPluginsStack, (lParam == DLLS_CLEAR)?TRUE:FALSE);
    }
  }

  lResult=lpfnMainProcA(hWnd, uMsg, wParam, lParam);

  if (uMsg == AKDN_MAIN_ONFINISH)
  {
    //Unload plugins
    StackHandleFree(&hHandlesStack);

    //Destroy windows
    if (!bMDI)
    {
      SendMessage(hWnd, AKDN_EDIT_ONFINISH, (WPARAM)hWndEdit, 0);
      DestroyWindow(hWndEdit);
      hWndEdit=NULL;
    }
    else
    {
      DestroyWindow(hMdiClient);
      hMdiClient=NULL;
    }
    DestroyWindow(hMainWnd);
    hMainWnd=NULL;
  }

  if (lpfnMainProcRetA)
  {
    CWPRETSTRUCT cwprs;

    cwprs.lResult=lResult;
    cwprs.lParam=lParam;
    cwprs.wParam=wParam;
    cwprs.message=uMsg;
    cwprs.hwnd=hWnd;
    lpfnMainProcRetA(&cwprs);
  }
  return lResult;
}

LRESULT CALLBACK CommonMainProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult;

  if (uMsg >= WM_USER)
  {
    if (uMsg == AKD_GETMAINPROC)
    {
      return StackProcGet(&hMainProcStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETMAINPROC)
    {
      return StackProcSet(&hMainProcStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, &lpfnMainProcW);
    }
    else if (uMsg == AKD_GETMAINPROCRET)
    {
      return StackProcGet(&hMainProcRetStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETMAINPROCRET)
    {
      return StackProcSet(&hMainProcRetStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, (WNDPROC *)&lpfnMainProcRetW);
    }
    else if (uMsg == AKD_GETEDITPROC)
    {
      return StackProcGet(&hEditProcStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETEDITPROC)
    {
      return StackProcSet(&hEditProcStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, &lpfnEditProcW);
    }
    else if (uMsg == AKD_GETEDITPROCRET)
    {
      return StackProcGet(&hEditProcRetStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETEDITPROCRET)
    {
      return StackProcSet(&hEditProcRetStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, (WNDPROC *)&lpfnEditProcRetW);
    }
    else if (uMsg == AKD_GETFRAMEPROC)
    {
      return StackProcGet(&hFrameProcStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETFRAMEPROC)
    {
      return StackProcSet(&hFrameProcStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, &lpfnFrameProcW);
    }
    else if (uMsg == AKD_GETFRAMEPROCRET)
    {
      return StackProcGet(&hFrameProcRetStack, wParam, (WNDPROCDATA **)lParam);
    }
    else if (uMsg == AKD_SETFRAMEPROCRET)
    {
      return StackProcSet(&hFrameProcRetStack, (WNDPROC)wParam, (WNDPROCDATA **)lParam, (WNDPROC *)&lpfnFrameProcRetW);
    }
    else if (uMsg == AKD_DLLCALL)
    {
      PLUGINCALLSENDW *pcs=(PLUGINCALLSENDW *)lParam;

      return (LRESULT)CallPluginReceiveSendW(pcs);
    }
    else if (uMsg == AKD_DLLFIND)
    {
      if (wParam)
        return (LRESULT)StackPluginFindW(&hPluginsStack, (wchar_t *)wParam);
      if (lParam)
        return (LRESULT)StackHotkeyFindW(&hPluginsStack, (WORD)lParam);
      return 0;
    }
    else if (uMsg == AKD_DLLADD)
    {
      PLUGINFUNCTIONW *pf=(PLUGINFUNCTIONW *)lParam;

      return (LRESULT)StackPluginAddW(&hPluginsStack, pf->wszFunction, pf->nFunctionLen, pf->wHotkey, pf->bOnStart, pf->bRunning, pf->PluginProc, pf->lpParameter);
    }
    else if (uMsg == AKD_DLLDELETE)
    {
      StackPluginDelete(&hPluginsStack, (void *)lParam);
      return 0;
    }
    else if (uMsg == AKD_DLLSAVE)
    {
      if (lParam == DLLS_ONEXIT)
      {
        bSavePluginsStackOnExit=TRUE;
        return TRUE;
      }
      return StackPluginSaveW(&hPluginsStack, (lParam == DLLS_CLEAR)?TRUE:FALSE);
    }
  }

  lResult=lpfnMainProcW(hWnd, uMsg, wParam, lParam);

  if (uMsg == AKDN_MAIN_ONFINISH)
  {
    //Unload plugins
    StackHandleFree(&hHandlesStack);

    //Destroy windows
    if (!bMDI)
    {
      SendMessage(hWnd, AKDN_EDIT_ONFINISH, (WPARAM)hWndEdit, 0);
      DestroyWindow(hWndEdit);
      hWndEdit=NULL;
    }
    else
    {
      DestroyWindow(hMdiClient);
      hMdiClient=NULL;
    }
    DestroyWindow(hMainWnd);
    hMainWnd=NULL;
  }

  if (lpfnMainProcRetW)
  {
    CWPRETSTRUCT cwprs;

    cwprs.lResult=lResult;
    cwprs.lParam=lParam;
    cwprs.wParam=wParam;
    cwprs.message=uMsg;
    cwprs.hwnd=hWnd;
    lpfnMainProcRetW(&cwprs);
  }
  return lResult;
}

LRESULT CALLBACK MainProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CREATE)
  {
    int iSBParts[]={110, 220, 250, 280, -1};
    CLIENTCREATESTRUCT ccs;
    DWORD dwClassStyle;
    RECT rcRect;
    HMENU hMenu;

    hPopupEdit=GetSubMenu(hPopupMenu, 0);
    hPopupView=GetSubMenu(hPopupMenu, 1);
    hPopupCodepage=GetSubMenu(hPopupMenu, 2);
    hPopupHeadline=GetSubMenu(hPopupMenu, 3);

    hMenu=GetSubMenu(hMainMenu, MENU_FILE_POSITION);
    hMenuRecentFiles=GetSubMenu(hMenu, MENU_FILE_RECENTFILES_POSITION);
    hMenu=GetSubMenu(hMainMenu, MENU_VIEW_POSITION);
    hMenuLanguage=GetSubMenu(hMenu, MENU_VIEW_LANGUAGE_POSITION);

    if (!bMDI)
    {
      DeleteMenu(hMainMenu, IDM_FILE_SAVEALL, MF_BYCOMMAND);
      DeleteMenu(hMainMenu, IDM_OPTIONS_SINGLEOPEN_PROGRAM, MF_BYCOMMAND);
      DeleteMenu(hMainMenu, MENU_MDI_POSITION, MF_BYPOSITION);
      DrawMenuBar(hWnd);
      DragAcceptFiles(hWnd, TRUE);

      hDummyWindow=CreateWindowA(APP_SDI_CLASSA,        // window class name
                                APP_SDI_TITLEA,        // window caption
                                WS_POPUP,              // window style
                                CW_USEDEFAULT,         // initial x position
                                CW_USEDEFAULT,         // initial y position
                                CW_USEDEFAULT,         // initial x size
                                CW_USEDEFAULT,         // initial y size
                                hWnd,                  // parent window handle
                                NULL,                  // window menu handle
                                hInstance,             // program instance handle
                                NULL);                 // creation parameters
    }
    else
    {
      GetClientRect(hWnd, &rcRect);

      //MDI Client
      ccs.hWindowMenu=GetSubMenu(hMainMenu, MENU_MDI_POSITION);
      ccs.idFirstChild=ID_FIRSTMDI;

      hMdiClient=CreateWindowA("MdiClient",
                               NULL,
                               WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VSCROLL|WS_HSCROLL,
                               rcRect.left, rcRect.top, rcRect.right, rcRect.bottom,
                               hWnd,
                               NULL,
                               hInstance,
                               (LPVOID)&ccs);

      dwClassStyle=GetClassLongA(hMdiClient, GCL_STYLE);
      SetClassLongA(hMdiClient, GCL_STYLE, dwClassStyle|CS_DBLCLKS);

      OldMdiClientProc=(WNDPROC)GetWindowLongA(hMdiClient, GWL_WNDPROC);
      SetWindowLongA(hMdiClient, GWL_WNDPROC, (LONG)NewMdiClientProcA);

      DragAcceptFiles(hMdiClient, TRUE);

      //Tab Control
      hTab=CreateWindowA("SysTabControl32",
                         NULL,
                         WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|TCS_SINGLELINE|TCS_FOCUSNEVER|TCS_BUTTONS|TCS_FLATBUTTONS,
                         rcRect.left, rcRect.top, rcRect.right, TAB_HEIGHT,
                         hWnd,
                         (HMENU)ID_TAB,
                         hInstance,
                         0);

      OldTabProc=(WNDPROC)GetWindowLongA(hTab, GWL_WNDPROC);
      SetWindowLongA(hTab, GWL_WNDPROC, (LONG)NewTabProcA);

      SendMessage(hTab, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
      hImageList=ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32|ILC_MASK, 0, 0);
      ImageList_SetBkColor(hImageList, GetSysColor(COLOR_BTNFACE));
      SendMessage(hTab, TCM_SETIMAGELIST, 0, (LPARAM)hImageList);
      SendMessage(hTab, TCM_SETEXTENDEDSTYLE, 0, TCS_EX_FLATSEPARATORS);
    }

    //Status Bar
    hStatus=CreateWindowA("msctls_statusbar32",
                         NULL,
                         WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|SBARS_SIZEGRIP,
                         0, 0, 0, 0,
                         hWnd,
                         (HMENU)ID_STATUS,
                         hInstance,
                         NULL);

    SendMessage(hStatus, SB_SIMPLE, FALSE, 0);
    SendMessage(hStatus, SB_SETPARTS, STATUS_PARTS, (LPARAM)&iSBParts);
    GetWindowRect(hStatus, &rcRect);
    nStatusHeight=rcRect.bottom - rcRect.top;

    PostMessage(hWnd, AKDN_MAIN_ONSTART, 0, 0);
  }

  if (uMsg >= WM_USER)
  {
    if (uMsg == AKDN_MAIN_ONSTART)
    {
      NMAINSHOW nms;
      RECT rcRect;
      char *pFileName;
      int i;

      cfA.lStructSize =sizeof(CHOOSEFONTA);
      cfA.Flags       =CF_FORCEFONTEXIST|CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS;

      ccA.lStructSize  =sizeof(CHOOSECOLORA);
      ccA.lpCustColors =crCustColors;
      ccA.Flags        =CC_FULLOPEN|CC_RGBINIT;

      psdPageA.lStructSize        =sizeof(PAGESETUPDLGA);
      psdPageA.hwndOwner          =hWnd;
      psdPageA.Flags             |=PSD_MARGINS|PSD_ENABLEPAGESETUPHOOK;
      psdPageA.lpfnPageSetupHook  =PrintPageSetupDlgProcA;

      pdA.lStructSize =sizeof(PRINTDLGA);
      pdA.hwndOwner   =hWnd;
      pdA.Flags       =PD_ALLPAGES|PD_NOPAGENUMS|PD_RETURNDC|PD_USEDEVMODECOPIESANDCOLLATE;
      pdA.nCopies     =1;

      ofnA.hwndOwner      =hWnd;
      ofnA.hInstance      =hLangLib;
      ofnA.lpstrFile      =szFileBuffer;
      ofnA.lpstrFilter    =szFilter;
      ofnA.nFilterIndex   =2;
      ofnA.nMaxFile       =FILELIST_BUFFER_SIZE;
      ofnA.lpstrInitialDir=szHomeDir;
      ofnA.Flags          =OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_ENABLEHOOK|OFN_ENABLETEMPLATE|OFN_ENABLESIZING|OFN_OVERWRITEPROMPT;
      ofnA.lpfnHook       =CodePageDlgProcA;
      ofnA.lpTemplateName =MAKEINTRESOURCEA(IDD_OFN);

      pspA[0].dwSize      =sizeof(PROPSHEETPAGEA);
      pspA[0].dwFlags     =PSP_DEFAULT;
      pspA[0].hInstance   =hLangLib;
      pspA[0].pszTemplate =MAKEINTRESOURCEA(IDD_OPTIONS_GENERAL);
      pspA[0].pfnDlgProc  =(DLGPROC)OptionsGeneralDlgProcA;
      pspA[1].dwSize      =sizeof(PROPSHEETPAGEA);
      pspA[1].dwFlags     =PSP_DEFAULT;
      pspA[1].hInstance   =hLangLib;
      pspA[1].pszTemplate =MAKEINTRESOURCEA(IDD_OPTIONS_ADVANCED1);
      pspA[1].pfnDlgProc  =(DLGPROC)OptionsAdvanced1DlgProcA;
      pspA[2].dwSize      =sizeof(PROPSHEETPAGEA);
      pspA[2].dwFlags     =PSP_DEFAULT;
      pspA[2].hInstance   =hLangLib;
      pspA[2].pszTemplate =MAKEINTRESOURCEA(IDD_OPTIONS_ADVANCED2);
      pspA[2].pfnDlgProc  =(DLGPROC)OptionsAdvanced2DlgProcA;

      pshA.dwSize      =(bOldComctl32)?(PROPSHEETHEADERA_V1_SIZE):(sizeof(PROPSHEETHEADERA));
      pshA.dwFlags     =PSH_PROPSHEETPAGE|PSH_NOAPPLYNOW|PSH_USEICONID|PSH_USECALLBACK;
      pshA.hwndParent  =hWnd;
      pshA.hInstance   =hLangLib;
      pshA.pszIcon     =MAKEINTRESOURCEA(IDI_ICON_MAIN);
      pshA.nPages      =sizeof(pspA) / sizeof(PROPSHEETPAGEA);
      pshA.nStartPage  =0;
      pshA.ppsp        =&pspA[0];
      pshA.pfnCallback =PropSheetProc;

      //Allocate and read search string
      if (nSearchStrings)
      {
        RegReadSearchA();
      }

      //Allocate and read recent files
      if (nRecentFiles)
      {
        if (RecentFilesAllocA())
        {
          RecentFilesZeroA();
          RecentFilesReadA();
        }
      }
      bMenuRecentFiles=TRUE;

      //Call plugins on start
      CallPluginsOnStartA(&hPluginsStack);

      //Create edit window
      if (!bMDI)
      {
        CreateEditWindowA(hWnd);
      }
      else
      {
        if (dwMainStyle == WS_MAXIMIZE)
        {
          rcRect.right=GetSystemMetrics(SM_CXMAXIMIZED);
          rcRect.bottom=GetSystemMetrics(SM_CYMAXIMIZED);
        }
        else GetClientRect(hMdiClient, &rcRect);

        i=MulDiv(rcRect.bottom, 30, 100);
        CreateMDIWindowA(APP_MDI_CLASSA, "", dwMdiStyle, 0, 0, rcRect.right - i, rcRect.bottom - i, hMdiClient, hInstance, 0);
      }

      //Apply settings
      szCurrentFile[0]='\0';
      SetFocus(hWndEdit);
      SetNewLineA(hWndEdit, NEWLINE_WIN, TRUE);
      SetInsertStateA(NULL, FALSE, TRUE);
      SetModifyA(hWndEdit, FALSE, TRUE);
      SetCodePageA(nDefaultCodePage, bDefaultBOM, TRUE);
      DoViewOnTop(bOnTop, TRUE);
      DoViewShowStatusBar(bStatusBar, TRUE);
      DoSettingsSaveTime(bSaveTime);
      DoSettingsKeepSpace(bKeepSpace);
      DoSettingsWatchFile(bWatchFile);
      DoSettingsSingleOpenFile(bSingleOpenFile);
      if (!SearchPathA(NULL, "charmap.exe", NULL, BUFFER_SIZE, buf, &pFileName))
        EnableMenuItem(hMainMenu, IDM_EDIT_INSERTCHAR, MF_GRAYED);
      CheckMenuRadioItem(hMainMenu, IDM_OPTIONS_SDI, IDM_OPTIONS_MDI, bRegMDI?IDM_OPTIONS_MDI:IDM_OPTIONS_SDI, MF_BYCOMMAND);

      if (bMDI)
      {
        DoSettingsSingleOpenProgram(bSingleOpenProgram);
        DoWindowTabView(nTabView, TRUE);
        DoWindowTabType(nTabType, TRUE);
        if (bOldComctl32) EnableMenuItem(hMainMenu, IDM_WINDOW_TABTYPE_FLATBUTTONS, MF_GRAYED);
        CheckMenuRadioItem(hMainMenu, IDM_WINDOW_TABSWITCH_NEXTPREV, IDM_WINDOW_TABSWITCH_RIGHTLEFT, (nTabSwitch == TAB_SWITCH_NEXTPREV)?IDM_WINDOW_TABSWITCH_NEXTPREV:IDM_WINDOW_TABSWITCH_RIGHTLEFT, MF_BYCOMMAND);
      }

      //PreShow
      nms.dwStyle=&dwMainStyle;
      nms.dwShow=&dwCmdShow;
      nms.bProcess=TRUE;
      SendMessage(hWnd, AKDN_MAIN_ONSTART_PRESHOW, 0, (LPARAM)&nms);
      if (nms.bProcess)
      {
        //Show main window
        ShowWindow(hWnd, (dwMainStyle == WS_MAXIMIZE)?SW_SHOWMAXIMIZED:SW_SHOW);

        //Shortcut
        if (dwCmdShow != SW_SHOWNORMAL)
          ShowWindow(hWnd, dwCmdShow);

        //Update main window
        UpdateWindow(hMainWnd);
      }
      SendMessage(hWnd, AKDN_MAIN_ONSTART_SHOW, 0, 0);

      if (*szFileBuffer) OpenDocumentA(hWndEdit, szFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, FALSE);
      SetCurrentDirectoryA(szExeDir);

      if (hMutex)
      {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
      }

      SendMessage(hWnd, AKDN_MAIN_ONSTART_FINISH, 0, 0);
      bMainOnStartFinish=TRUE;
      return 0;
    }
    if (uMsg == AKD_BEGINOPTIONS)
    {
      if (nSaveSettings == SS_REGISTRY)
      {
        REGHANDLEA *rh;

        if (rh=(REGHANDLEA *)API_HeapAlloc(hHeap, 0, sizeof(REGHANDLEA)))
        {
          rh->dwType=wParam;

          if (lParam)
            wsprintfA(rh->szKey, "Software\\Akelsoft\\AkelPad\\Plugs\\%s", (char *)lParam);
          else
            wsprintfA(rh->szKey, "Software\\Akelsoft\\AkelPad\\Options");

          if (rh->dwType & POB_READ)
          {
            if (RegOpenKeyExA(HKEY_CURRENT_USER, rh->szKey, 0, KEY_READ, &rh->hKey) != ERROR_SUCCESS)
            {
              API_HeapFree(hHeap, 0, (LPVOID)rh);
              rh=NULL;
            }
          }
          else if (rh->dwType & POB_SAVE)
          {
            if (rh->dwType & POB_CLEAR)
            {
              RegClearKeyA(HKEY_CURRENT_USER, rh->szKey);
            }
            if (RegCreateKeyExA(HKEY_CURRENT_USER, rh->szKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &rh->hKey, NULL) != ERROR_SUCCESS)
            {
              API_HeapFree(hHeap, 0, (LPVOID)rh);
              rh=NULL;
            }
          }
        }
        return (LRESULT)rh;
      }
      else
      {
        INIHANDLEA *ih;

        if (ih=(INIHANDLEA *)API_HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(INIHANDLEA)))
        {
          ih->dwType=wParam;

          if (lParam)
            wsprintfA(ih->szIniFile, "%s\\AkelFiles\\Plugs\\%s.ini", szExeDir, (char *)lParam);
          else
            wsprintfA(ih->szIniFile, "%s\\AkelPad.ini", szExeDir);

          if (ih->dwType & POB_READ)
          {
            if (!OpenIniA(&ih->hStack, ih->szIniFile))
            {
              API_HeapFree(hHeap, 0, (LPVOID)ih);
              ih=NULL;
            }
          }
          else if (ih->dwType & POB_SAVE)
          {
            if (!CreateIniA(&ih->hStack, ih->szIniFile))
            {
              API_HeapFree(hHeap, 0, (LPVOID)ih);
              ih=NULL;
            }
            else
            {
              if (ih->dwType & POB_CLEAR)
              {
                HINISECTION *lpIniSection;

                if (lpIniSection=StackGetIniSectionA(&ih->hStack, "Options", lstrlenA("Options")))
                  StackFreeIniSection(&lpIniSection->hSectionStack);
              }
            }
          }
        }
        return (LRESULT)ih;
      }
    }
    if (uMsg == AKD_OPTION)
    {
      PLUGINOPTIONA *po=(PLUGINOPTIONA *)lParam;
      DWORD dwType;
      DWORD dwResult=0;

      if (nSaveSettings == SS_REGISTRY)
      {
        REGHANDLEA *rh=(REGHANDLEA *)wParam;
        DWORD dwSize;

        if (po->dwType == PO_DWORD)
          dwType=REG_DWORD;
        else if (po->dwType == PO_BINARY)
          dwType=REG_BINARY;
        else if (po->dwType == PO_STRING)
          dwType=REG_SZ;

        if (rh->dwType & POB_READ)
        {
          dwSize=po->dwData;
          if (RegQueryValueExA(rh->hKey, po->szOptionName, NULL, &dwType, (LPBYTE)po->lpData, &dwSize) == ERROR_SUCCESS)
            dwResult=dwSize;
        }
        else if (rh->dwType & POB_SAVE)
        {
          if (RegSetValueExA(rh->hKey, po->szOptionName, 0, dwType, (LPBYTE)po->lpData, po->dwData) == ERROR_SUCCESS)
            dwResult=TRUE;
        }
      }
      else
      {
        INIHANDLEA *ih=(INIHANDLEA *)wParam;

        if (po->dwType == PO_DWORD)
          dwType=INI_DWORD;
        else if (po->dwType == PO_BINARY)
          dwType=INI_BINARY;
        else if (po->dwType == PO_STRING)
          dwType=INI_STRINGANSI;

        if (ih->dwType & POB_READ)
        {
          dwResult=IniGetValueA(&ih->hStack, "Options", po->szOptionName, dwType, (LPBYTE)po->lpData, po->dwData);
        }
        else if (ih->dwType & POB_SAVE)
        {
          dwResult=IniSetValueA(&ih->hStack, "Options", po->szOptionName, dwType, (LPBYTE)po->lpData, po->dwData);
        }
      }
      return dwResult;
    }
    if (uMsg == AKD_ENDOPTIONS)
    {
      BOOL bResult=FALSE;

      if (wParam)
      {
        if (nSaveSettings == SS_REGISTRY)
        {
          REGHANDLEA *rh=(REGHANDLEA *)wParam;

          if (RegCloseKey(rh->hKey) == ERROR_SUCCESS)
            bResult=TRUE;
          API_HeapFree(hHeap, 0, (LPVOID)rh);
        }
        else
        {
          INIHANDLEA *ih=(INIHANDLEA *)wParam;

          if (ih->dwType & POB_READ)
          {
            bResult=TRUE;
          }
          else if (ih->dwType & POB_SAVE)
          {
            bResult=SaveIniA(&ih->hStack, ih->szIniFile);
          }
          StackFreeIni(&ih->hStack);
          API_HeapFree(hHeap, 0, (LPVOID)ih);
        }
      }
      return bResult;
    }
    if (uMsg == AKD_SAVEDOCUMENT)
    {
      SAVEDOCUMENTA *sd=(SAVEDOCUMENTA *)lParam;

      return SaveDocumentA((HWND)wParam, sd->szFile, sd->nCodePage, sd->bBOM, sd->bUpdate);
    }
    if (uMsg == AKD_GETTEXTLENGTH)
    {
      return GetTextLength((HWND)wParam);
    }
    if (uMsg == AKD_GETSELTEXTW)
    {
      return (LRESULT)GetSelTextW((HWND)wParam);
    }
    if (uMsg == AKD_GETTEXTRANGE)
    {
      GETTEXTRANGE *gtr=(GETTEXTRANGE *)lParam;

      return GetRangeTextA((HWND)wParam, gtr->cpMin, gtr->cpMax, (char **)&gtr->pText);
    }
    if (uMsg == AKD_FREETEXT)
    {
      return FreeText((LPVOID)lParam);
    }
    if (uMsg == AKD_REPLACESELA)
    {
      ReplaceSelA((HWND)wParam, (char *)lParam, -1);
      return 0;
    }
    if (uMsg == AKD_REPLACESELW)
    {
      ReplaceSelW((HWND)wParam, (wchar_t *)lParam, -1);
      return 0;
    }
    if (uMsg == AKD_PASTE)
    {
      DoEditPaste((HWND)wParam, lParam);
      return 0;
    }
    if (uMsg == AKD_COPY)
    {
      DoEditCopy((HWND)wParam);
      return 0;
    }
    if (uMsg == AKD_TEXTFIND)
    {
      TEXTFINDA *ft=(TEXTFINDA *)lParam;

      return FindTextA((HWND)wParam, ft->dwFlags, ft->pFindIt);
    }
    if (uMsg == AKD_TEXTREPLACE)
    {
      TEXTREPLACEA *rt=(TEXTREPLACEA *)lParam;

      return ReplaceTextA((HWND)wParam, rt->dwFlags, rt->pFindIt, rt->pReplaceWith, rt->bAll, &rt->nChanges);
    }
    if (uMsg == AKD_RECODESEL)
    {
      TEXTRECODE *tr=(TEXTRECODE *)lParam;

      RecodeTextW((HWND)wParam, tr->nCodePageFrom, tr->nCodePageTo);
      return 0;
    }
    if (uMsg == AKD_GETMAININFO)
    {
      PLUGINDATA *pd=(PLUGINDATA *)lParam;

      pd->cb=sizeof(PLUGINDATA);
      pd->pFunction=NULL;
      pd->hInstanceDLL=0;
      pd->lpPluginFunction=NULL;
      pd->lpbAutoLoad=NULL;
      pd->nUnload=0;
      pd->bActive=0;
      pd->bOnStart=0;
      pd->lParam=0;
      pd->pAkelDir=(unsigned char *)szExeDir;
      pd->hInstanceEXE=hInstance;
      pd->hPluginsStack=&hPluginsStack;
      pd->hMainWnd=hMainWnd;
      pd->hWndEdit=hWndEdit;
      pd->hStatus=hStatus;
      pd->hMdiClient=hMdiClient;
      pd->hTab=hTab;
      pd->hMainMenu=hMainMenu;
      pd->hMenuRecentFiles=hMenuRecentFiles;
      pd->hMenuLanguage=hMenuLanguage;
      pd->hPopupMenu=hPopupMenu;
      pd->hMainIcon=hMainIcon;
      pd->hGlobalAccel=hGlobalAccel;
      pd->bOldWindows=bOldWindows;
      pd->bOldRichEdit=bOldRichEdit;
      pd->bOldComctl32=bOldComctl32;
      pd->bAkelEdit=FALSE;
      pd->bMDI=bMDI;
      pd->nSaveSettings=nSaveSettings;
      pd->pLangModule=(unsigned char *)szLangModule;
      pd->wLangSystem=(WORD)dwLangSystem;

      return 0;
    }
    if (uMsg == AKD_GETEDITINFO)
    {
      EDITINFO *ei=(EDITINFO *)lParam;

      if (!wParam || (HWND)wParam == hWndEdit)
      {
        if (hWndEdit)
        {
          ei->hWndEdit=hWndEdit;
          ei->pFile=(unsigned char *)szCurrentFile;
          ei->nCodePage=nCurrentCodePage;
          ei->bBOM=bCurrentBOM;
          ei->nNewLine=nNewLine;
          ei->bModified=bModified;
          ei->bReadOnly=bReadOnly;
          ei->bWordWrap=bWordWrap;
          ei->bInsertState=bInsertState;
          ei->nTabStopSize=nTabStopSize;
          ei->bTabStopAsSpaces=bTabStopAsSpaces;
          ei->nUndoLimit=nUndoLimit;
          ei->bDetailedUndo=bDetailedUndo;
          ei->bShowURL=bShowURL;
          ei->dwEditMargins=dwEditMargins;
          ei->ft=ftFileTime;
          ei->aec=aecColors;
          return TRUE;
        }
        return FALSE;
      }
      if (bMDI)
      {
        WNDFRAMEA *wf;
        HWND hWndFrame;

        if (hWndFrame=GetParent((HWND)wParam))
        {
          if (wf=(WNDFRAMEA *)GetWindowLongA(hWndFrame, GWL_USERDATA))
          {
            ei->hWndEdit=(HWND)wParam;
            ei->pFile=(unsigned char *)wf->szFile;
            ei->nCodePage=wf->nCodePage;
            ei->bBOM=wf->bBOM;
            ei->nNewLine=wf->nNewLine;
            ei->bModified=wf->bModified;
            ei->bReadOnly=wf->bReadOnly;
            ei->bWordWrap=wf->bWordWrap;
            ei->bInsertState=wf->bInsertState;
            ei->nTabStopSize=wf->nTabStopSize;
            ei->bTabStopAsSpaces=bTabStopAsSpaces;
            ei->nUndoLimit=wf->nUndoLimit;
            ei->bDetailedUndo=bDetailedUndo;
            ei->bShowURL=wf->bShowURL;
            ei->dwEditMargins=wf->dwEditMargins;
            ei->ft=wf->ft;
            ei->aec=wf->aec;
            return TRUE;
          }
        }
      }
      return FALSE;
    }
    if (uMsg == AKD_GETFONT)
    {
      if (!wParam || (HWND)wParam == hWndEdit)
        return (LRESULT)&lfEditFontA;

      if (bMDI)
      {
        WNDFRAMEA *wf;
        HWND hWndFrame;

        if (hWndFrame=GetParent((HWND)wParam))
        {
          if (wf=(WNDFRAMEA *)GetWindowLongA(hWndFrame, GWL_USERDATA))
            return (LRESULT)&wf->lf;
        }
      }
      return (LRESULT)NULL;
    }
    if (uMsg == AKD_SETFONT)
    {
      SetChosenFontA((HWND)wParam, (LOGFONTA *)lParam);
      bEditFontChanged=TRUE;
      return 0;
    }
    if (uMsg == AKD_SETMODIFY)
    {
      SetModifyA((HWND)wParam, lParam, FALSE);
      return 0;
    }
    if (uMsg == AKD_SETNEWLINE)
    {
      SetNewLineA((HWND)wParam, lParam, FALSE);
      return 0;
    }
    if (uMsg == AKD_SETFILEPRINT)
    {
      bGlobalPrint=(BOOL)wParam;
      return 0;
    }
    if (uMsg == AKD_SETMSGCREATE)
    {
      nMsgCreate=(int)wParam;
      return 0;
    }
    if (uMsg == AKD_SETMSGBINARY)
    {
      nMsgBinary=(int)wParam;
      return 0;
    }
    if (uMsg == AKD_GETCODEPAGELIST)
    {
      int *nCodePage=(int *)wParam;

      if (nCodePage)
      {
        *nCodePage=nDefaultCodePage;
      }
      return (LRESULT)lpCodepageList;
    }
    if (uMsg == AKD_GETRECENTFILES)
    {
      RECENTFILESA *rf=(RECENTFILESA *)lParam;

      rf->lpszRecentNames=lpszRecentNames;
      rf->lpdwRecentPositions=lpdwRecentPositions;
      rf->lpdwRecentCodepages=lpdwRecentCodepages;
      rf->nRecentFiles=nRecentFiles;
      return 0;
    }
    if (uMsg == AKD_GETQUEUE)
    {
      return GetQueueStatus(wParam);
    }
    if (uMsg == AKD_WAITKEYBOARD)
    {
      BYTE lpKeyState[256];
      MSG msg;
      int i;

      Loop:
      Sleep(0);

      if (GetKeyboardState(lpKeyState))
      {
        for (i=0; i < 256; ++i)
        {
          if (lpKeyState[i] & 0x80)
          {
            while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
            {
              TranslateMessage(&msg);
              DispatchMessageA(&msg);
            }
            goto Loop;
          }
        }
      }
      return 0;
    }
    if (uMsg == AKD_GETPRINTDLG)
    {
      return (LRESULT)&pdA;
    }
    if (uMsg == AKD_GETPAGEDLG)
    {
      return (LRESULT)&psdPageA;
    }
    if (uMsg == AKD_GLOBALALLOC)
    {
      return (LRESULT)GlobalAlloc(wParam, lParam);
    }
    if (uMsg == AKD_GLOBALLOCK)
    {
      return (LRESULT)GlobalLock((HGLOBAL)wParam);
    }
    if (uMsg == AKD_GLOBALUNLOCK)
    {
      return (LRESULT)GlobalUnlock((HGLOBAL)wParam);
    }
    if (uMsg == AKD_GLOBALFREE)
    {
      return (LRESULT)GlobalFree((HGLOBAL)wParam);
    }
    if (uMsg == AKD_CREATEWINDOW)
    {
      CREATEWINDOWA *cw=(CREATEWINDOWA *)lParam;

      return (LRESULT)CreateWindowA(cw->szClassName, cw->szWindowName, cw->dwStyle, cw->x, cw->y, cw->nWidth, cw->nHeight, cw->hWndParent, cw->hMenu, cw->hInstance, cw->lpParam);
    }
    if (uMsg == AKD_GETMODELESS)
    {
      return (LRESULT)hDlgModeless;
    }
    if (uMsg == AKD_SETMODELESS)
    {
      hDlgModeless=(HWND)wParam;
      return 0;
    }
    if (uMsg == AKD_RESIZE)
    {
      UpdateSize();
      return 0;
    }
    if (uMsg == AKD_DOCK)
    {
      DOCK *lpDock=(DOCK *)lParam;
      DOCK *lpResult=NULL;

      if (wParam & DK_ADD)
      {
        lpResult=StackDockAdd(&hDocksStack, lpDock);
        lpDock=lpResult;
      }
      if (wParam & DK_SUBCLASS)
      {
        if (lpDock->hWnd)
        {
          OldDockProc=(WNDPROC)GetWindowLongA(lpDock->hWnd, GWL_WNDPROC);
          SetWindowLongA(lpDock->hWnd, GWL_WNDPROC, (LONG)DockProcA);
        }
      }
      if (wParam & DK_UNSUBCLASS)
      {
        if (lpDock->hWnd)
        {
          SetWindowLongA(lpDock->hWnd, GWL_WNDPROC, (LONG)OldDockProc);
        }
      }
      if ((wParam & DK_SETLEFT) ||
          (wParam & DK_SETRIGHT) ||
          (wParam & DK_SETTOP) ||
          (wParam & DK_SETBOTTOM))
      {
        int nSide=0;

        if (wParam & DK_SETLEFT) nSide=DKS_LEFT;
        else if (wParam & DK_SETRIGHT) nSide=DKS_RIGHT;
        else if (wParam & DK_SETTOP) nSide=DKS_TOP;
        else if (wParam & DK_SETBOTTOM) nSide=DKS_BOTTOM;

        if (nSide) DockSetSide(&hDocksStack, lpDock, nSide);
      }
      if (wParam & DK_HIDE)
      {
        if (lpDock->hWnd)
        {
          lpDock->dwFlags|=DKF_HIDDEN;
          ShowWindow(lpDock->hWnd, FALSE);
          UpdateSize();
        }
      }
      if (wParam & DK_SHOW)
      {
        if (lpDock->hWnd)
        {
          lpDock->dwFlags&=~DKF_HIDDEN;
          ShowWindow(lpDock->hWnd, TRUE);
          UpdateSize();
        }
      }
      if (wParam & DK_DELETE)
      {
        StackDockDelete(&hDocksStack, lpDock);
      }
      return (LRESULT)lpResult;
    }
    if (uMsg == AKD_GETCHARCOLOR)
    {
      CHARCOLOR *cc=(CHARCOLOR *)lParam;

      return GetCharColor((HWND)wParam, cc);
    }
  }

  if (uMsg == WM_COPYDATA)
  {
    COPYDATASTRUCT *cds=(COPYDATASTRUCT *)lParam;
    int nResult=0;

    if (cds->dwData == CD_OPENDOCUMENT)
    {
      OPENDOCUMENTA *od=(OPENDOCUMENTA *)cds->lpData;

      if ((od->hWnd && od->hWnd != hWndEdit) || bMDI || SaveChangedA())
      {
        if (*od->szWorkDir) SetCurrentDirectoryA(od->szWorkDir);
        nResult=OpenDocumentA(od->hWnd, od->szFile, od->dwFlags, od->nCodePage, od->bBOM);
        if (*od->szWorkDir) SetCurrentDirectoryA(szExeDir);
      }
    }
    return nResult;
  }
  else if (uMsg == WM_INITMENU)
  {
    LPARAM lMenuState;

    if (!lParam || (lParam & IMENU_EDIT))
    {
      if (hWndEdit)
      {
        EnableMenuItem(hMainMenu, IDM_FILE_CREATENEW, (bMDI && bSingleOpenProgram)?MF_GRAYED:MF_ENABLED);
        EnableMenuItem(hMainMenu, IDM_FILE_REOPEN, (szCurrentFile[0])?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_FILE_SAVE, bModified?MF_ENABLED:MF_GRAYED);

        EnableMenuItem(hMainMenu, IDM_EDIT_UNDO, (!bReadOnly && SendMessage(hWndEdit, EM_CANUNDO, 0, 0))?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_EDIT_REDO, (!bReadOnly && SendMessage(hWndEdit, EM_CANREDO, 0, 0))?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_EDIT_PASTE, (!bReadOnly && SendMessage(hWndEdit, EM_CANPASTE, 0, 0))?MF_ENABLED:MF_GRAYED);

        EnableMenuItem(hMainMenu, IDM_EDIT_CUT, (!bReadOnly && chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_EDIT_CLEAR, (!bReadOnly && chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);

        EnableMenuItem(hMainMenu, IDM_EDIT_COPY, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_EDIT_DELETE_FIRST_CHAR_MENU, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);

        lMenuState=(!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))?MF_GRAYED:MF_ENABLED;
        EnableMenuItem(hMainMenu, IDM_EDIT_INSERT_TAB_MENU, lMenuState);
        EnableMenuItem(hMainMenu, IDM_EDIT_DELETE_TAB_MENU, lMenuState);
        EnableMenuItem(hMainMenu, IDM_EDIT_INSERT_SPACE_MENU, lMenuState);
        EnableMenuItem(hMainMenu, IDM_EDIT_DELETE_SPACE_MENU, lMenuState);
      }
      EnableMenuItem(hMainMenu, IDM_OPTIONS_EXEC, (*szCommand)?MF_ENABLED:MF_GRAYED);
    }
    if (!lParam || (lParam & IMENU_CHECKS))
    {
      CheckMenuRadioItem(hMainMenu, IDM_EDIT_NEWLINE_WIN, IDM_EDIT_NEWLINE_UNIX, (nNewLine == NEWLINE_WIN)?IDM_EDIT_NEWLINE_WIN:IDM_EDIT_NEWLINE_UNIX, MF_BYCOMMAND);
      CheckMenuItem(hMainMenu, IDM_OPTIONS_READONLY, bReadOnly?MF_CHECKED:MF_UNCHECKED);
      CheckMenuItem(hMainMenu, IDM_VIEW_WORDWRAP, bWordWrap?MF_CHECKED:MF_UNCHECKED);
    }
    bMenuLanguage=TRUE;
    if (lParam) return TRUE;
  }
  else if (uMsg == WM_INITMENUPOPUP)
  {
    if ((HMENU)wParam == hMenuLanguage)
    {
      if (bMenuLanguage)
      {
        bMenuLanguage=FALSE;
        LanguageMenuA();
      }
    }
    else if ((HMENU)wParam == hMenuRecentFiles)
    {
      if (bMenuRecentFiles)
      {
        bMenuRecentFiles=FALSE;
        RecentFilesMenuA();
      }
    }
  }
  else if (uMsg == WM_MOVE)
  {
    DWORD dwStyle;

    dwStyle=GetWindowLongA(hWnd, GWL_STYLE);

    if (!(dwStyle & WS_MAXIMIZE) && !(dwStyle & WS_MINIMIZE))
    {
      GetWindowPos(hWnd, NULL, &rcMainWindowRestored);
    }
  }
  else if (uMsg == WM_SIZE)
  {
    if (wParam != SIZE_MINIMIZED)
    {
      if (wParam != SIZE_MAXIMIZED)
      {
        GetWindowPos(hWnd, NULL, &rcMainWindowRestored);
      }
      dwLastMainSize=wParam;
      UpdateSize();
      SendMessage(hStatus, WM_SIZE, wParam, lParam);
      return TRUE;
    }
  }
  else if (uMsg == WM_DROPFILES)
  {
    if (bMDI || SaveChangedA())
    {
      nPosition=nPositionOld;
      DragQueryFileA((HDROP)wParam, 0, szFileBuffer, FILELIST_BUFFER_SIZE);
      OpenDocumentA(hWndEdit, szFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, FALSE);
      DragFinish((HDROP)wParam);
    }
  }
  else if (uMsg == WM_COMMAND)
  {
    //WM_COMMAND (SDI & MDI)
    if (LOWORD(wParam) == IDM_RECENT_FILES)
    {
      int nDead;
      BOOL bMessageShow=TRUE;

      RecentFilesReadA();
      nDead=RecentFilesDeleteDeadA();
      RecentFilesSaveA();
      bMenuRecentFiles=TRUE;
      SendMessage(hMainWnd, AKDN_RECENTFILESDELETE, nDead, (LPARAM)&bMessageShow);

      if (bMessageShow)
      {
        API_LoadStringA(hLangLib, MSG_RECENTFILES_DELETED, buf, BUFFER_SIZE);
        wsprintfA(buf2, buf, nDead);
        MessageBoxA(hWnd, buf2, APP_MAIN_TITLEA, MB_OK|MB_ICONINFORMATION);
      }
    }
    else if (LOWORD(wParam) > IDM_RECENT_FILES && LOWORD(wParam) <= (IDM_RECENT_FILES + nRecentFiles))
    {
      if (bMDI || SaveChangedA())
      {
        lstrcpynA(szFileBuffer, lpszRecentNames[LOWORD(wParam) - IDM_RECENT_FILES - 1], MAX_PATH);
        OpenDocumentA(hWndEdit, szFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, FALSE);
      }
    }
    else if (LOWORD(wParam) >= IDM_LANGUAGE && LOWORD(wParam) <= (IDM_LANGUAGE + nLangModules + 1))
    {
      if (LOWORD(wParam) == IDM_LANGUAGE)
      {
        if (!*szLangModule) return TRUE;
        szLangModule[0]='\0';
      }
      else
      {
        GetMenuStringA(hMainMenu, LOWORD(wParam), buf, BUFFER_SIZE, MF_BYCOMMAND);
        wsprintfA(buf2, "%s.dll", buf);
        if (!lstrcmpiA(szLangModule, buf2)) return TRUE;
        lstrcpynA(szLangModule, buf2, MAX_PATH);
      }
      API_LoadStringA(hLangLib, MSG_RESTART_PROGRAM, buf, BUFFER_SIZE);
      MessageBoxA(hWnd, buf, APP_MAIN_TITLEA, MB_OK|MB_ICONEXCLAMATION);
    }
    else if (LOWORD(wParam) >= IDM_POPUP_OPENAS && LOWORD(wParam) <= (IDM_POPUP_OPENAS + nCodepageListLen + 1))
    {
      int nCodePageSel;

      nCodePageSel=LOWORD(wParam) - IDM_POPUP_OPENAS - 1;
      if (nCodePageSel < 0) nCodePageSel=nCodepageListLen - 2;
      DoFileReopenAsA(OD_ADT_DETECT_BOM, lpCodepageList[nCodePageSel], FALSE);
    }
    else if (LOWORD(wParam) >= IDM_POPUP_SAVEAS && LOWORD(wParam) <= (IDM_POPUP_SAVEAS + nCodepageListLen + 1))
    {
      int nCodePageSel;

      nCodePageSel=LOWORD(wParam) - IDM_POPUP_SAVEAS - 1;
      if (nCodePageSel < 0) nCodePageSel=nCodepageListLen - 2;
      return SaveDocumentA(hWndEdit, szCurrentFile, lpCodepageList[nCodePageSel], TRUE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_FILE_NEW)
    {
      return DoFileNewA();
    }
    else if (LOWORD(wParam) == IDM_FILE_CREATENEW)
    {
      if (!bMDI)
      {
        if (nSaveSettings == SS_REGISTRY)
          RegSaveOptionsA();
        else
          IniSaveOptionsA();
      }
      return (int)DoFileNewWindowA(0);
    }
    else if (LOWORD(wParam) == IDM_FILE_OPEN)
    {
      return DoFileOpenA();
    }
    else if (LOWORD(wParam) == IDM_FILE_REOPEN)
    {
      DoFileReopenAsA(0, nCurrentCodePage, bCurrentBOM);
    }
    else if (LOWORD(wParam) == IDM_FILE_SAVE)
    {
      return DoFileSaveA();
    }
    else if (LOWORD(wParam) == IDM_FILE_SAVEAS)
    {
      return DoFileSaveAsA();
    }
    else if (LOWORD(wParam) == IDM_FILE_SAVEALL)
    {
      if (!bMDI)
      {
        return DoFileSaveA();
      }
      else
      {
        HWND hWndTmp=hWndFrameActive;

        do
        {
          if (!DoFileSaveA()) return FALSE;
          SendMessage(hMdiClient, WM_MDINEXT, (WPARAM)hWndFrameActive, FALSE);
        }
        while (hWndFrameActive != hWndTmp);

        return TRUE;
      }
    }
    else if (LOWORD(wParam) == IDM_FILE_CLOSEALL)
    {
      if (!bMDI)
      {
        return DoFileCloseA();
      }
      else
      {
        while (hWndFrameActive)
        {
          SendMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
          if (!bFileExitError) return FALSE;
        }
        return TRUE;
      }
    }
    else if (LOWORD(wParam) == IDM_FILE_CLOSEALL_BUTACTIVE)
    {
      if (bMDI)
      {
        HWND hWndTmp=hWndFrameActive;

        while (1)
        {
          SendMessage(hMdiClient, WM_MDINEXT, (WPARAM)hWndFrameActive, FALSE);
          if (hWndFrameActive == hWndTmp) break;

          SendMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
          if (!bFileExitError) return FALSE;
        }
        return TRUE;
      }
    }
    else if (LOWORD(wParam) == IDM_FILE_PAGESETUP)
    {
      return DoFilePageSetupA();
    }
    else if (LOWORD(wParam) == IDM_FILE_PRINT)
    {
      return DoFilePrintA(FALSE);
    }
    else if (LOWORD(wParam) == IDM_FILE_EXIT)
    {
      PostMessage(hWnd, WM_CLOSE, 0, 0);
    }
    else if (LOWORD(wParam) == IDM_EDIT_UNDO)
    {
      DoEditUndo(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_REDO)
    {
      DoEditRedo(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_CUT)
    {
      DoEditCutA(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_COPY)
    {
      DoEditCopy(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_PASTE)
    {
      DoEditPaste(hWndEdit, FALSE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_CLEAR)
    {
      DoEditClearA(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_SELECTALL)
    {
      DoEditSelectAll(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_INSERTCHAR)
    {
      DoEditInsertChar();
    }
    else if (LOWORD(wParam) == IDM_EDIT_INSERTDATE)
    {
      DoEditInsertDateA(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_RECODE)
    {
      DoEditRecodeA(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_INSERT_TAB_MENU ||
             LOWORD(wParam) == IDM_EDIT_INSERT_TAB)
    {
      if (!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))
        InsertTabStopW(hWndEdit);
      else
        IndentTabStopW(hWndEdit, STRSEL_INSERT);
    }
    else if (LOWORD(wParam) == IDM_EDIT_DELETE_TAB_MENU ||
             LOWORD(wParam) == IDM_EDIT_DELETE_TAB)
    {
      if (!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))
        InsertTabStopW(hWndEdit);
      else
        IndentTabStopW(hWndEdit, STRSEL_DELETE|STRSEL_TAB);
    }
    else if (LOWORD(wParam) == IDM_EDIT_INSERT_SPACE_MENU ||
             LOWORD(wParam) == IDM_EDIT_INSERT_SPACE)
    {
      if (!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))
      {
        if (!IsReadOnly()) SendMessage(hWndEdit, WM_CHAR, (WPARAM)' ', 0);
      }
      else
        DoEditInsertStringInSelectionW(hWndEdit, STRSEL_INSERT, L" ");
    }
    else if (LOWORD(wParam) == IDM_EDIT_DELETE_SPACE_MENU ||
             LOWORD(wParam) == IDM_EDIT_DELETE_SPACE)
    {
      if (!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))
      {
        if (!IsReadOnly()) SendMessage(hWndEdit, WM_CHAR, (WPARAM)' ', 0);
      }
      else
        DoEditInsertStringInSelectionW(hWndEdit, STRSEL_DELETE|STRSEL_SPACE, L" ");
    }
    else if (LOWORD(wParam) == IDM_EDIT_DELETE_FIRST_CHAR_MENU ||
             LOWORD(wParam) == IDM_EDIT_DELETE_FIRST_CHAR)
    {
      DoEditDeleteFirstCharW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_DELETE_TRAILING_WHITESPACES)
    {
      DoEditDeleteTrailingWhitespacesW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_UPPERCASE)
    {
      DoEditChangeCaseA(hWndEdit, UPPERCASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_LOWERCASE)
    {
      DoEditChangeCaseA(hWndEdit, LOWERCASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_SENTENCECASE)
    {
      DoEditChangeCaseA(hWndEdit, SENTENCECASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_TITLECASE)
    {
      DoEditChangeCaseA(hWndEdit, TITLECASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_INVERTCASE)
    {
      DoEditChangeCaseA(hWndEdit, INVERTCASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_NEWLINE_WIN)
    {
      if (!IsReadOnly())
      {
        if (nNewLine != NEWLINE_WIN)
        {
          SetNewLineA(hWndEdit, NEWLINE_WIN, FALSE);
          SetModifyA(hWndEdit, TRUE, FALSE);
        }
      }
    }
    else if (LOWORD(wParam) == IDM_EDIT_NEWLINE_UNIX)
    {
      if (!IsReadOnly())
      {
        if (nNewLine != NEWLINE_UNIX)
        {
          SetNewLineA(hWndEdit, NEWLINE_UNIX, FALSE);
          SetModifyA(hWndEdit, TRUE, FALSE);
        }
      }
    }
    else if (LOWORD(wParam) == IDM_EDIT_FIND)
    {
      DoEditFindA();
    }
    else if (LOWORD(wParam) == IDM_EDIT_FINDNEXTDOWN)
    {
      DoEditFindNextDownA(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_FINDNEXTUP)
    {
      DoEditFindNextUpA(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_REPLACE)
    {
      DoEditReplaceA();
    }
    else if (LOWORD(wParam) == IDM_EDIT_GOTOLINE)
    {
      DoEditGoToLineA();
    }
    else if (LOWORD(wParam) == IDM_VIEW_FONT)
    {
      if (DoViewFontA(hMainWnd, &lfEditFontA))
      {
        SetChosenFontA(hWndEdit, &lfEditFontA);
        bEditFontChanged=TRUE;
        return TRUE;
      }
      return FALSE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_TEXT_COLOR)
    {
      if (DoViewColorA(hMainWnd, &aecColors.crBasicText))
      {
        aecColors.dwFlags=AECLR_ALL;
        aecColors.crActiveLineText=aecColors.crBasicText;
        SetChosenFontColorA(hWndEdit, aecColors.crBasicText);
        bColorsChanged=TRUE;
        return TRUE;
      }
      return FALSE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_BG_COLOR)
    {
      if (DoViewColorA(hMainWnd, &aecColors.crBasicBk))
      {
        aecColors.dwFlags=AECLR_ALL;
        aecColors.crActiveLineBk=aecColors.crBasicBk;
        SendMessage(hWndEdit, EM_SETBKGNDCOLOR, 0, aecColors.crBasicBk);
        bColorsChanged=TRUE;
        return TRUE;
      }
      return FALSE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_INCREASE_FONT)
    {
      DoViewFontSizeA(hWndEdit, INCREASE_FONT);
      bEditFontChanged=TRUE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_DECREASE_FONT)
    {
      DoViewFontSizeA(hWndEdit, DECREASE_FONT);
      bEditFontChanged=TRUE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_ALIGN_CENTER)
    {
      DoViewAlignmentA(hWndEdit, PFA_CENTER);
    }
    else if (LOWORD(wParam) == IDM_VIEW_ALIGN_RIGHT)
    {
      DoViewAlignmentA(hWndEdit, PFA_RIGHT);
    }
    else if (LOWORD(wParam) == IDM_VIEW_ALIGN_LEFT)
    {
      DoViewAlignmentA(hWndEdit, PFA_LEFT);
    }
    else if (LOWORD(wParam) == IDM_VIEW_WORDWRAP)
    {
      DoViewWordWrap(hWndEdit, !bWordWrap, FALSE);
    }
    else if (LOWORD(wParam) == IDM_VIEW_ONTOP)
    {
      DoViewOnTop(!bOnTop, FALSE);
    }
    else if (LOWORD(wParam) == IDM_VIEW_SHOW_STATUSBAR)
    {
      DoViewShowStatusBar(!bStatusBar, FALSE);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_EXEC)
    {
      return DoSettingsExecA();
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_READONLY)
    {
      DoSettingsReadOnly(hWndEdit, !bReadOnly, FALSE);
      if (hDlgModeless) PostMessage(hDlgModeless, WM_COMMAND, IDC_SETREADONLY, 0);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_SAVETIME)
    {
      DoSettingsSaveTime(!bSaveTime);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_KEEPSPACE)
    {
      DoSettingsKeepSpace(!bKeepSpace);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_WATCHFILE)
    {
      DoSettingsWatchFile(!bWatchFile);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_SINGLEOPEN_FILE)
    {
      DoSettingsSingleOpenFile(!bSingleOpenFile);
      if (nSaveSettings == SS_REGISTRY)
        RegSaveOptionsA();
      else
        IniSaveOptionsA();
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_SINGLEOPEN_PROGRAM)
    {
      DoSettingsSingleOpenProgram(!bSingleOpenProgram);
      if (nSaveSettings == SS_REGISTRY)
        RegSaveOptionsA();
      else
        IniSaveOptionsA();
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_SDI)
    {
      if (bRegMDI)
      {
        CheckMenuRadioItem(hMainMenu, IDM_OPTIONS_SDI, IDM_OPTIONS_MDI, IDM_OPTIONS_SDI, MF_BYCOMMAND);

        API_LoadStringA(hLangLib, MSG_RESTART_PROGRAM, buf, BUFFER_SIZE);
        MessageBoxA(hWnd, buf, APP_MAIN_TITLEA, MB_OK|MB_ICONEXCLAMATION);
        bRegMDI=FALSE;
      }
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_MDI)
    {
      if (!bRegMDI)
      {
        CheckMenuRadioItem(hMainMenu, IDM_OPTIONS_SDI, IDM_OPTIONS_MDI, IDM_OPTIONS_MDI, MF_BYCOMMAND);

        API_LoadStringA(hLangLib, MSG_RESTART_PROGRAM, buf, BUFFER_SIZE);
        MessageBoxA(hWnd, buf, APP_MAIN_TITLEA, MB_OK|MB_ICONEXCLAMATION);
        bRegMDI=TRUE;
      }
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_PLUGINS)
    {
      DoSettingsPluginsA(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS)
    {
      DoSettingsOptionsA();
    }
    else if (LOWORD(wParam) == IDM_ABOUT)
    {
      DoHelpAboutA();
    }
    else if (LOWORD(wParam) == IDM_NONMENU_CHANGESIZE)
    {
      ShowWindow(hWnd, (GetWindowLongA(hWnd, GWL_STYLE) & WS_MAXIMIZE)?SW_RESTORE:SW_MAXIMIZE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_DELLINE)
    {
      DoNonMenuDelLineA(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_PASTEANSI)
    {
      DoEditPaste(hWndEdit, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPEN_MSG)
    {
      if (hWndEdit == (HWND)lParam)
      {
        API_LoadStringA(hLangLib, MSG_FILE_CHANGED, buf, BUFFER_SIZE);
        wsprintfA(buf2, buf, szCurrentFile);
        if (MessageBoxA(hWnd, buf2, APP_MAIN_TITLEA, bModified?MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2:MB_YESNO|MB_ICONQUESTION) == IDYES)
        {
          bMdiReopen=TRUE;
          OpenDocumentA(hWndEdit, szCurrentFile, 0, nCurrentCodePage, bCurrentBOM);
          bMdiReopen=FALSE;
        }
      }
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REDETECT)
    {
      DoFileReopenAsA(OD_ADT_DETECT_CODEPAGE|OD_ADT_DETECT_BOM, 0, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_ANSI)
    {
      DoFileReopenAsA(0, nAnsiCodePage, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_OEM)
    {
      DoFileReopenAsA(0, nOemCodePage, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_UTF16LE)
    {
      DoFileReopenAsA(OD_ADT_DETECT_BOM, CP_UNICODE_UCS2_LE, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_UTF16BE)
    {
      DoFileReopenAsA(OD_ADT_DETECT_BOM, CP_UNICODE_UCS2_BE, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_UTF8)
    {
      DoFileReopenAsA(OD_ADT_DETECT_BOM, CP_UNICODE_UTF8, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_KOIR)
    {
      DoFileReopenAsA(0, CP_KOI8_R, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_ANSI)
    {
      return SaveDocumentA(hWndEdit, szCurrentFile, nAnsiCodePage, FALSE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_OEM)
    {
      return SaveDocumentA(hWndEdit, szCurrentFile, nOemCodePage, FALSE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_UTF16LE)
    {
      return SaveDocumentA(hWndEdit, szCurrentFile, CP_UNICODE_UCS2_LE, TRUE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_UTF16BE)
    {
      return SaveDocumentA(hWndEdit, szCurrentFile, CP_UNICODE_UCS2_BE, TRUE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_UTF8)
    {
      return SaveDocumentA(hWndEdit, szCurrentFile, CP_UNICODE_UTF8, TRUE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_KOIR)
    {
      return SaveDocumentA(hWndEdit, szCurrentFile, CP_KOI8_R, FALSE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_INSERTMODE)
    {
      if (hWndEdit) SetInsertStateA(hWndEdit, !bInsertState, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_FILECLOSE)
    {
      return DoFileCloseA();
    }
    else if (LOWORD(wParam) == IDM_NONMENU_DLGNEXT)
    {
      return (LRESULT)NextDialog(FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_DLGPREV)
    {
      return (LRESULT)NextDialog(TRUE);
    }
    else if (LOWORD(wParam) == IDM_POPUP_CODEPAGEMENU)
    {
      RECT rc;

      SendMessage(hStatus, SB_GETRECT, STATUS_CODEPAGE, (LPARAM)&rc);
      ClientToScreen(hStatus, (POINT *)&rc);
      ShowMenuPopupCodepageA((POINT *)&rc);
    }

    //WM_COMMAND (MDI)
    if (bMDI)
    {
      if (LOWORD(wParam) == IDM_WINDOW_TABVIEW_TOP)
      {
        DoWindowTabView(TAB_VIEW_TOP, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABVIEW_BOTTOM)
      {
        DoWindowTabView(TAB_VIEW_BOTTOM, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABVIEW_NONE)
      {
        DoWindowTabView(TAB_VIEW_NONE, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABTYPE_STANDARD)
      {
        DoWindowTabType(TAB_TYPE_STANDARD, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABTYPE_BUTTONS)
      {
        DoWindowTabType(TAB_TYPE_BUTTONS, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABTYPE_FLATBUTTONS)
      {
        DoWindowTabType(TAB_TYPE_FLATBUTTONS, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABSWITCH_NEXTPREV)
      {
        if (nTabSwitch == TAB_SWITCH_RIGHTLEFT)
        {
          CheckMenuRadioItem(hMainMenu, IDM_WINDOW_TABSWITCH_NEXTPREV, IDM_WINDOW_TABSWITCH_RIGHTLEFT, IDM_WINDOW_TABSWITCH_NEXTPREV, MF_BYCOMMAND);
          nTabSwitch=TAB_SWITCH_NEXTPREV;
        }
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABSWITCH_RIGHTLEFT)
      {
        if (nTabSwitch == TAB_SWITCH_NEXTPREV)
        {
          CheckMenuRadioItem(hMainMenu, IDM_WINDOW_TABSWITCH_NEXTPREV, IDM_WINDOW_TABSWITCH_RIGHTLEFT, IDM_WINDOW_TABSWITCH_RIGHTLEFT, MF_BYCOMMAND);
          nTabSwitch=TAB_SWITCH_RIGHTLEFT;
        }
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TILEHORIZONTAL)
      {
        SendMessage(hMdiClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TILEVERTICAL)
      {
        SendMessage(hMdiClient, WM_MDITILE, MDITILE_VERTICAL, 0);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_CASCADE)
      {
        SendMessage(hMdiClient, WM_MDICASCADE, 0, 0);
      }
      else if (LOWORD(wParam) == IDM_NONMENU_MDINEXT)
      {
        SendMessage(hMdiClient, WM_MDINEXT, (WPARAM)hWndFrameActive, FALSE);
      }
      else if (LOWORD(wParam) == IDM_NONMENU_MDIPREV)
      {
        SendMessage(hMdiClient, WM_MDINEXT, (WPARAM)hWndFrameActive, TRUE);
      }
      else if (LOWORD(wParam) == IDM_NONMENU_MDICLOSE)
      {
        SendMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
        return bFileExitError;
      }
    }
  }
  else if (uMsg == WM_NOTIFY)
  {
    //WM_NOTIFY (SDI & MDI)
    if (wParam == ID_STATUS && (!bMDI || hWndFrameActive))
    {
      if ((int)((NMHDR *)lParam)->code == NM_DBLCLK)
      {
        if (((NMMOUSE *)lParam)->dwItemSpec == 0)
        {
          DoEditGoToLineA();
        }
        else if (((NMMOUSE *)lParam)->dwItemSpec == 2)
        {
          SetInsertStateA(hWndEdit, !bInsertState, FALSE);
        }
        else if (((NMMOUSE *)lParam)->dwItemSpec == 3)
        {
          if (!IsReadOnly())
          {
            if (nNewLine == NEWLINE_WIN) SetNewLineA(hWndEdit, NEWLINE_UNIX, FALSE);
            else if (nNewLine == NEWLINE_UNIX) SetNewLineA(hWndEdit, NEWLINE_WIN, FALSE);
            SetModifyA(hWndEdit, TRUE, FALSE);
          }
        }
      }
      else if ((int)((NMHDR *)lParam)->code == NM_RCLICK)
      {
        if (((NMMOUSE *)lParam)->dwItemSpec == 4)
        {
          POINT pt;

          GetCursorPos(&pt);
          ShowMenuPopupCodepageA(&pt);
        }
      }
    }

    //WM_NOTIFY (MDI)
    if (bMDI)
    {
      if (wParam == ID_TAB)
      {
        if (((NMHDR *)lParam)->code == TCN_SELCHANGE)
        {
          TCITEMA tcItemA;
          int nItem;

          bTabPressed=TRUE;
          nItem=SendMessage(hTab, TCM_GETCURSEL, 0, 0);
          tcItemA.mask=TCIF_PARAM;
          SendMessage(hTab, TCM_GETITEMA, nItem, (LPARAM)&tcItemA);
          SendMessage(hMdiClient, WM_MDIACTIVATE, (WPARAM)tcItemA.lParam, 0);
          bTabPressed=FALSE;
        }
      }
    }
  }
  else if (uMsg == WM_CLOSE)
  {
    if (!bMDI)
    {
      if (!DoFileExitA()) return TRUE;
    }
    else
    {
      bMdiClientRedraw=FALSE;

      while (hWndFrameActive)
      {
        SendMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
        if (!bFileExitError)
        {
          bMdiClientRedraw=TRUE;
          return TRUE;
        }
      }
    }

    //Close modeless dialog
    if (hDlgModeless)
      SendMessage(hDlgModeless, WM_COMMAND, IDCANCEL, 0);

    //Main window will be destroyed
    PostMessage(hWnd, AKDN_MAIN_ONFINISH, 0, 0);

    return 0;
  }
  else if (uMsg == WM_DESTROY)
  {
    //Save options
    if (nSaveSettings == SS_REGISTRY)
      RegSaveOptionsA();
    else
      IniSaveOptionsA();

    //Save plugin stack
    if (bSavePluginsStackOnExit)
      StackPluginSaveA(&hPluginsStack, DLLS_CLEAR);

    //Clean up
    if (bMDI)
    {
      ImageList_Destroy(hImageList);
      DestroyCursor(hCursorDragMove);
      DestroyIcon(hIconEmpty);
    }
    DestroyAcceleratorTable(hMainAccel);
    DestroyAcceleratorTable(hGlobalAccel);
    DestroyIcon(hMainIcon);
    DestroyMenu(hMainMenu);
    DestroyMenu(hPopupMenu);
    CodepageListFree(&lpCodepageList);
    FreeMemoryRecentFilesA();
    FreeMemorySearchA();

    PostQuitMessage(0);
    return 0;
  }

  //Special messages
  if (!bMDI)
  {
    LRESULT lResult;

    if (lResult=EditParentMessagesA(hWnd, uMsg, wParam, lParam)) return lResult;
  }
  else
  {
    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_MBUTTONDOWN)
    {
      if (hWndFrameActive) SendMessage(hMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMdiMaximize);
      CreateMDIWindowA(APP_MDI_CLASSA, "", (bMdiMaximize == TRUE)?WS_MAXIMIZE:0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hMdiClient, hInstance, 0);
    }
  }

  if (!bMDI)
    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
  else
    return DefFrameProcA(hWnd, hMdiClient, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CREATE)
  {
    int iSBParts[]={110, 220, 250, 280, -1};
    CLIENTCREATESTRUCT ccs;
    DWORD dwClassStyle;
    RECT rcRect;
    HMENU hMenu;

    hPopupEdit=GetSubMenu(hPopupMenu, 0);
    hPopupView=GetSubMenu(hPopupMenu, 1);
    hPopupCodepage=GetSubMenu(hPopupMenu, 2);
    hPopupHeadline=GetSubMenu(hPopupMenu, 3);

    hMenu=GetSubMenu(hMainMenu, MENU_FILE_POSITION);
    hMenuRecentFiles=GetSubMenu(hMenu, MENU_FILE_RECENTFILES_POSITION);
    hMenu=GetSubMenu(hMainMenu, MENU_VIEW_POSITION);
    hMenuLanguage=GetSubMenu(hMenu, MENU_VIEW_LANGUAGE_POSITION);

    if (!bMDI)
    {
      DeleteMenu(hMainMenu, IDM_FILE_SAVEALL, MF_BYCOMMAND);
      DeleteMenu(hMainMenu, IDM_OPTIONS_SINGLEOPEN_PROGRAM, MF_BYCOMMAND);
      DeleteMenu(hMainMenu, MENU_MDI_POSITION, MF_BYPOSITION);
      DrawMenuBar(hWnd);
      DragAcceptFiles(hWnd, TRUE);

      hDummyWindow=CreateWindowW(APP_SDI_CLASSW,        // window class name
                                 APP_SDI_TITLEW,        // window caption
                                 WS_POPUP,              // window style
                                 CW_USEDEFAULT,         // initial x position
                                 CW_USEDEFAULT,         // initial y position
                                 CW_USEDEFAULT,         // initial x size
                                 CW_USEDEFAULT,         // initial y size
                                 hWnd,                  // parent window handle
                                 NULL,                  // window menu handle
                                 hInstance,             // program instance handle
                                 NULL);                 // creation parameters
    }
    else
    {
      GetClientRect(hWnd, &rcRect);

      //MDI Client
      ccs.hWindowMenu=GetSubMenu(hMainMenu, MENU_MDI_POSITION);
      ccs.idFirstChild=ID_FIRSTMDI;

      hMdiClient=CreateWindowW(L"MdiClient",
                               NULL,
                               WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VSCROLL|WS_HSCROLL,
                               rcRect.left, rcRect.top, rcRect.right, rcRect.bottom,
                               hWnd,
                               NULL,
                               hInstance,
                               (LPVOID)&ccs);

      dwClassStyle=GetClassLongW(hMdiClient, GCL_STYLE);
      SetClassLongW(hMdiClient, GCL_STYLE, dwClassStyle|CS_DBLCLKS);

      OldMdiClientProc=(WNDPROC)GetWindowLongW(hMdiClient, GWL_WNDPROC);
      SetWindowLongW(hMdiClient, GWL_WNDPROC, (LONG)NewMdiClientProcW);

      DragAcceptFiles(hMdiClient, TRUE);

      //Tab Control
      hTab=CreateWindowW(L"SysTabControl32",
                         NULL,
                         WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|TCS_SINGLELINE|TCS_FOCUSNEVER|TCS_BUTTONS|TCS_FLATBUTTONS,
                         rcRect.left, rcRect.top, rcRect.right, TAB_HEIGHT,
                         hWnd,
                         (HMENU)ID_TAB,
                         hInstance,
                         0);

      OldTabProc=(WNDPROC)GetWindowLongW(hTab, GWL_WNDPROC);
      SetWindowLongW(hTab, GWL_WNDPROC, (LONG)NewTabProcW);

      SendMessage(hTab, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
      hImageList=ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32|ILC_MASK, 0, 0);
      ImageList_SetBkColor(hImageList, GetSysColor(COLOR_BTNFACE));
      SendMessage(hTab, TCM_SETIMAGELIST, 0, (LPARAM)hImageList);
      SendMessage(hTab, TCM_SETEXTENDEDSTYLE, 0, TCS_EX_FLATSEPARATORS);
    }

    //Status Bar
    hStatus=CreateWindowW(L"msctls_statusbar32",
                          NULL,
                          WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|SBARS_SIZEGRIP,
                          0, 0, 0, 0,
                          hWnd,
                          (HMENU)ID_STATUS,
                          hInstance,
                          NULL);

    SendMessage(hStatus, SB_SIMPLE, FALSE, 0);
    SendMessage(hStatus, SB_SETPARTS, STATUS_PARTS, (LPARAM)&iSBParts);
    GetWindowRect(hStatus, &rcRect);
    nStatusHeight=rcRect.bottom - rcRect.top;

    PostMessage(hWnd, AKDN_MAIN_ONSTART, 0, 0);
  }

  if (uMsg >= WM_USER)
  {
    if (uMsg == AKDN_MAIN_ONSTART)
    {
      NMAINSHOW nms;
      RECT rcRect;
      wchar_t *wpFileName;
      int i;

      cfW.lStructSize =sizeof(CHOOSEFONTW);
      cfW.Flags       =CF_FORCEFONTEXIST|CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS;

      ccW.lStructSize  =sizeof(CHOOSECOLORW);
      ccW.lpCustColors =crCustColors;
      ccW.Flags        =CC_FULLOPEN|CC_RGBINIT;

      psdPageW.lStructSize        =sizeof(PAGESETUPDLGW);
      psdPageW.hwndOwner          =hWnd;
      psdPageW.Flags             |=PSD_MARGINS|PSD_ENABLEPAGESETUPHOOK;
      psdPageW.lpfnPageSetupHook  =PrintPageSetupDlgProcW;

      pdW.lStructSize =sizeof(PRINTDLGW);
      pdW.hwndOwner   =hWnd;
      pdW.Flags       =PD_ALLPAGES|PD_NOPAGENUMS|PD_RETURNDC|PD_USEDEVMODECOPIESANDCOLLATE;
      pdW.nCopies     =1;

      ofnW.hwndOwner      =hWnd;
      ofnW.hInstance      =hLangLib;
      ofnW.lpstrFile      =wszFileBuffer;
      ofnW.lpstrFilter    =wszFilter;
      ofnW.nFilterIndex   =2;
      ofnW.nMaxFile       =FILELIST_BUFFER_SIZE;
      ofnW.lpstrInitialDir=wszHomeDir;
      ofnW.Flags          =OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_ENABLEHOOK|OFN_ENABLETEMPLATE|OFN_ENABLESIZING|OFN_OVERWRITEPROMPT;
      ofnW.lpfnHook       =CodePageDlgProcW;
      ofnW.lpTemplateName =MAKEINTRESOURCEW(IDD_OFN);

      pspW[0].dwSize      =sizeof(PROPSHEETPAGEW);
      pspW[0].dwFlags     =PSP_DEFAULT;
      pspW[0].hInstance   =hLangLib;
      pspW[0].pszTemplate =MAKEINTRESOURCEW(IDD_OPTIONS_GENERAL);
      pspW[0].pfnDlgProc  =(DLGPROC)OptionsGeneralDlgProcW;
      pspW[1].dwSize      =sizeof(PROPSHEETPAGEW);
      pspW[1].dwFlags     =PSP_DEFAULT;
      pspW[1].hInstance   =hLangLib;
      pspW[1].pszTemplate =MAKEINTRESOURCEW(IDD_OPTIONS_ADVANCED1);
      pspW[1].pfnDlgProc  =(DLGPROC)OptionsAdvanced1DlgProcW;
      pspW[2].dwSize      =sizeof(PROPSHEETPAGEW);
      pspW[2].dwFlags     =PSP_DEFAULT;
      pspW[2].hInstance   =hLangLib;
      pspW[2].pszTemplate =MAKEINTRESOURCEW(IDD_OPTIONS_ADVANCED2);
      pspW[2].pfnDlgProc  =(DLGPROC)OptionsAdvanced2DlgProcW;

      pshW.dwSize      =sizeof(PROPSHEETHEADERW);
      pshW.dwFlags     =PSH_PROPSHEETPAGE|PSH_NOAPPLYNOW|PSH_USEICONID|PSH_USECALLBACK;
      pshW.hwndParent  =hWnd;
      pshW.hInstance   =hLangLib;
      pshW.pszIcon     =MAKEINTRESOURCEW(IDI_ICON_MAIN);
      pshW.nPages      =sizeof(pspW) / sizeof(PROPSHEETPAGEW);
      pshW.nStartPage  =0;
      pshW.ppsp        =&pspW[0];
      pshW.pfnCallback =PropSheetProc;

      //Allocate and read search string
      if (nSearchStrings)
      {
        RegReadSearchW();
      }

      //Allocate and read recent files
      if (nRecentFiles)
      {
        if (RecentFilesAllocW())
        {
          RecentFilesZeroW();
          RecentFilesReadW();
        }
      }
      bMenuRecentFiles=TRUE;

      //Call plugins on start
      CallPluginsOnStartW(&hPluginsStack);

      //Create edit window
      if (!bMDI)
      {
        CreateEditWindowW(hWnd);
      }
      else
      {
        if (dwMainStyle == WS_MAXIMIZE)
        {
          rcRect.right=GetSystemMetrics(SM_CXMAXIMIZED);
          rcRect.bottom=GetSystemMetrics(SM_CYMAXIMIZED);
        }
        else GetClientRect(hMdiClient, &rcRect);

        i=MulDiv(rcRect.bottom, 30, 100);
        CreateMDIWindowW(APP_MDI_CLASSW, L"", dwMdiStyle, 0, 0, rcRect.right - i, rcRect.bottom - i, hMdiClient, hInstance, 0);
      }

      //Apply settings
      wszCurrentFile[0]='\0';
      SetFocus(hWndEdit);
      SetNewLineW(hWndEdit, NEWLINE_WIN, TRUE);
      SetInsertStateW(NULL, FALSE, TRUE);
      SetModifyW(hWndEdit, FALSE, TRUE);
      SetCodePageW(nDefaultCodePage, bDefaultBOM, TRUE);
      DoViewOnTop(bOnTop, TRUE);
      DoViewShowStatusBar(bStatusBar, TRUE);
      DoSettingsSaveTime(bSaveTime);
      DoSettingsKeepSpace(bKeepSpace);
      DoSettingsWatchFile(bWatchFile);
      DoSettingsSingleOpenFile(bSingleOpenFile);
      if (!SearchPathW(NULL, L"charmap.exe", NULL, BUFFER_SIZE, wbuf, &wpFileName))
        EnableMenuItem(hMainMenu, IDM_EDIT_INSERTCHAR, MF_GRAYED);
      CheckMenuRadioItem(hMainMenu, IDM_OPTIONS_SDI, IDM_OPTIONS_MDI, bRegMDI?IDM_OPTIONS_MDI:IDM_OPTIONS_SDI, MF_BYCOMMAND);

      if (bMDI)
      {
        DoSettingsSingleOpenProgram(bSingleOpenProgram);
        DoWindowTabView(nTabView, TRUE);
        DoWindowTabType(nTabType, TRUE);
        if (bOldComctl32) EnableMenuItem(hMainMenu, IDM_WINDOW_TABTYPE_FLATBUTTONS, MF_GRAYED);
        CheckMenuRadioItem(hMainMenu, IDM_WINDOW_TABSWITCH_NEXTPREV, IDM_WINDOW_TABSWITCH_RIGHTLEFT, (nTabSwitch == TAB_SWITCH_NEXTPREV)?IDM_WINDOW_TABSWITCH_NEXTPREV:IDM_WINDOW_TABSWITCH_RIGHTLEFT, MF_BYCOMMAND);
      }

      //PreShow
      nms.dwStyle=&dwMainStyle;
      nms.dwShow=&dwCmdShow;
      nms.bProcess=TRUE;
      SendMessage(hWnd, AKDN_MAIN_ONSTART_PRESHOW, 0, (LPARAM)&nms);
      if (nms.bProcess)
      {
        //Show main window
        ShowWindow(hWnd, (dwMainStyle == WS_MAXIMIZE)?SW_SHOWMAXIMIZED:SW_SHOW);

        //Shortcut
        if (dwCmdShow != SW_SHOWNORMAL)
          ShowWindow(hWnd, dwCmdShow);

        //Update main window
        UpdateWindow(hMainWnd);
      }
      SendMessage(hWnd, AKDN_MAIN_ONSTART_SHOW, 0, 0);

      if (*wszFileBuffer) OpenDocumentW(hWndEdit, wszFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, FALSE);
      SetCurrentDirectoryW(wszExeDir);

      if (hMutex)
      {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
      }

      SendMessage(hWnd, AKDN_MAIN_ONSTART_FINISH, 0, 0);
      bMainOnStartFinish=TRUE;
      return 0;
    }
    if (uMsg == AKD_BEGINOPTIONS)
    {
      if (nSaveSettings == SS_REGISTRY)
      {
        REGHANDLEW *rh;

        if (rh=(REGHANDLEW *)API_HeapAlloc(hHeap, 0, sizeof(REGHANDLEW)))
        {
          rh->dwType=wParam;

          if (lParam)
            wsprintfW(rh->wszKey, L"Software\\Akelsoft\\AkelPad\\Plugs\\%s", (wchar_t *)lParam);
          else
            wsprintfW(rh->wszKey, L"Software\\Akelsoft\\AkelPad\\Options");

          if (rh->dwType & POB_READ)
          {
            if (RegOpenKeyExW(HKEY_CURRENT_USER, rh->wszKey, 0, KEY_READ, &rh->hKey) != ERROR_SUCCESS)
            {
              API_HeapFree(hHeap, 0, (LPVOID)rh);
              rh=NULL;
            }
          }
          else if (rh->dwType & POB_SAVE)
          {
            if (rh->dwType & POB_CLEAR)
            {
              RegClearKeyW(HKEY_CURRENT_USER, rh->wszKey);
            }
            if (RegCreateKeyExW(HKEY_CURRENT_USER, rh->wszKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &rh->hKey, NULL) != ERROR_SUCCESS)
            {
              API_HeapFree(hHeap, 0, (LPVOID)rh);
              rh=NULL;
            }
          }
        }
        return (LRESULT)rh;
      }
      else
      {
        INIHANDLEW *ih;

        if (ih=(INIHANDLEW *)API_HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(INIHANDLEW)))
        {
          ih->dwType=wParam;

          if (lParam)
            wsprintfW(ih->wszIniFile, L"%s\\AkelFiles\\Plugs\\%s.ini", wszExeDir, (wchar_t *)lParam);
          else
            wsprintfW(ih->wszIniFile, L"%s\\AkelPad.ini", wszExeDir);

          if (ih->dwType & POB_READ)
          {
            if (!OpenIniW(&ih->hStack, ih->wszIniFile))
            {
              API_HeapFree(hHeap, 0, (LPVOID)ih);
              ih=NULL;
            }
          }
          else if (ih->dwType & POB_SAVE)
          {
            if (!CreateIniW(&ih->hStack, ih->wszIniFile))
            {
              API_HeapFree(hHeap, 0, (LPVOID)ih);
              ih=NULL;
            }
            else
            {
              if (ih->dwType & POB_CLEAR)
              {
                HINISECTION *lpIniSection;

                if (lpIniSection=StackGetIniSectionW(&ih->hStack, L"Options", lstrlenW(L"Options")))
                  StackFreeIniSection(&lpIniSection->hSectionStack);
              }
            }
          }
        }
        return (LRESULT)ih;
      }
    }
    if (uMsg == AKD_OPTION)
    {
      PLUGINOPTIONW *po=(PLUGINOPTIONW *)lParam;
      DWORD dwType;
      DWORD dwResult=0;

      if (nSaveSettings == SS_REGISTRY)
      {
        REGHANDLEW *rh=(REGHANDLEW *)wParam;
        DWORD dwSize;

        if (po->dwType == PO_DWORD)
          dwType=REG_DWORD;
        else if (po->dwType == PO_BINARY)
          dwType=REG_BINARY;
        else if (po->dwType == PO_STRING)
          dwType=REG_SZ;

        if (rh->dwType & POB_READ)
        {
          dwSize=po->dwData;
          if (RegQueryValueExW(rh->hKey, po->wszOptionName, NULL, &dwType, (LPBYTE)po->lpData, &dwSize) == ERROR_SUCCESS)
            dwResult=dwSize;
        }
        else if (rh->dwType & POB_SAVE)
        {
          if (RegSetValueExW(rh->hKey, po->wszOptionName, 0, dwType, (LPBYTE)po->lpData, po->dwData) == ERROR_SUCCESS)
            dwResult=TRUE;
        }
      }
      else
      {
        INIHANDLEW *ih=(INIHANDLEW *)wParam;

        if (po->dwType == PO_DWORD)
          dwType=INI_DWORD;
        else if (po->dwType == PO_BINARY)
          dwType=INI_BINARY;
        else if (po->dwType == PO_STRING)
          dwType=INI_STRINGUNICODE;

        if (ih->dwType & POB_READ)
        {
          dwResult=IniGetValueW(&ih->hStack, L"Options", po->wszOptionName, dwType, (LPBYTE)po->lpData, po->dwData);
        }
        else if (ih->dwType & POB_SAVE)
        {
          dwResult=IniSetValueW(&ih->hStack, L"Options", po->wszOptionName, dwType, (LPBYTE)po->lpData, po->dwData);
        }
      }
      return dwResult;
    }
    if (uMsg == AKD_ENDOPTIONS)
    {
      BOOL bResult=FALSE;

      if (wParam)
      {
        if (nSaveSettings == SS_REGISTRY)
        {
          REGHANDLEW *rh=(REGHANDLEW *)wParam;

          if (RegCloseKey(rh->hKey) == ERROR_SUCCESS)
            bResult=TRUE;
          API_HeapFree(hHeap, 0, (LPVOID)rh);
        }
        else
        {
          INIHANDLEW *ih=(INIHANDLEW *)wParam;

          if (ih->dwType & POB_READ)
          {
            bResult=TRUE;
          }
          else if (ih->dwType & POB_SAVE)
          {
            bResult=SaveIniW(&ih->hStack, ih->wszIniFile);
          }
          StackFreeIni(&ih->hStack);
          API_HeapFree(hHeap, 0, (LPVOID)ih);
        }
      }
      return bResult;
    }
    if (uMsg == AKD_SAVEDOCUMENT)
    {
      SAVEDOCUMENTW *sd=(SAVEDOCUMENTW *)lParam;

      return SaveDocumentW((HWND)wParam, sd->wszFile, sd->nCodePage, sd->bBOM, sd->bUpdate);
    }
    if (uMsg == AKD_GETTEXTLENGTH)
    {
      return GetTextLength((HWND)wParam);
    }
    if (uMsg == AKD_GETSELTEXTW)
    {
      return (LRESULT)GetSelTextW((HWND)wParam);
    }
    if (uMsg == AKD_GETTEXTRANGE)
    {
      GETTEXTRANGE *gtr=(GETTEXTRANGE *)lParam;

      return GetRangeTextW((HWND)wParam, gtr->cpMin, gtr->cpMax, (wchar_t **)&gtr->pText);
    }
    if (uMsg == AKD_FREETEXT)
    {
      return FreeText((LPVOID)lParam);
    }
    if (uMsg == AKD_REPLACESELA)
    {
      ReplaceSelA((HWND)wParam, (char *)lParam, -1);
      return 0;
    }
    if (uMsg == AKD_REPLACESELW)
    {
      ReplaceSelW((HWND)wParam, (wchar_t *)lParam, -1);
      return 0;
    }
    if (uMsg == AKD_PASTE)
    {
      DoEditPaste((HWND)wParam, lParam);
      return 0;
    }
    if (uMsg == AKD_COPY)
    {
      DoEditCopy((HWND)wParam);
      return 0;
    }
    if (uMsg == AKD_TEXTFIND)
    {
      TEXTFINDW *ft=(TEXTFINDW *)lParam;

      return FindTextW((HWND)wParam, ft->dwFlags, ft->wpFindIt);
    }
    if (uMsg == AKD_TEXTREPLACE)
    {
      TEXTREPLACEW *rt=(TEXTREPLACEW *)lParam;

      return ReplaceTextW((HWND)wParam, rt->dwFlags, rt->wpFindIt, rt->wpReplaceWith, rt->bAll, &rt->nChanges);
    }
    if (uMsg == AKD_RECODESEL)
    {
      TEXTRECODE *tr=(TEXTRECODE *)lParam;

      RecodeTextW((HWND)wParam, tr->nCodePageFrom, tr->nCodePageTo);
      return 0;
    }
    if (uMsg == AKD_GETMAININFO)
    {
      PLUGINDATA *pd=(PLUGINDATA *)lParam;

      pd->cb=sizeof(PLUGINDATA);
      pd->pFunction=NULL;
      pd->hInstanceDLL=0;
      pd->lpPluginFunction=NULL;
      pd->lpbAutoLoad=NULL;
      pd->nUnload=0;
      pd->bActive=0;
      pd->bOnStart=0;
      pd->lParam=0;
      pd->pAkelDir=(unsigned char *)wszExeDir;
      pd->hInstanceEXE=hInstance;
      pd->hPluginsStack=&hPluginsStack;
      pd->hMainWnd=hMainWnd;
      pd->hWndEdit=hWndEdit;
      pd->hStatus=hStatus;
      pd->hMdiClient=hMdiClient;
      pd->hTab=hTab;
      pd->hMainMenu=hMainMenu;
      pd->hMenuRecentFiles=hMenuRecentFiles;
      pd->hMenuLanguage=hMenuLanguage;
      pd->hPopupMenu=hPopupMenu;
      pd->hMainIcon=hMainIcon;
      pd->hGlobalAccel=hGlobalAccel;
      pd->bOldWindows=bOldWindows;
      pd->bOldRichEdit=bOldRichEdit;
      pd->bOldComctl32=bOldComctl32;
      pd->bAkelEdit=FALSE;
      pd->bMDI=bMDI;
      pd->nSaveSettings=nSaveSettings;
      pd->pLangModule=(unsigned char *)wszLangModule;
      pd->wLangSystem=(WORD)dwLangSystem;

      return 0;
    }
    if (uMsg == AKD_GETEDITINFO)
    {
      EDITINFO *ei=(EDITINFO *)lParam;

      if (!wParam || (HWND)wParam == hWndEdit)
      {
        if (hWndEdit)
        {
          ei->hWndEdit=hWndEdit;
          ei->pFile=(unsigned char *)wszCurrentFile;
          ei->nCodePage=nCurrentCodePage;
          ei->bBOM=bCurrentBOM;
          ei->nNewLine=nNewLine;
          ei->bModified=bModified;
          ei->bReadOnly=bReadOnly;
          ei->bWordWrap=bWordWrap;
          ei->bInsertState=bInsertState;
          ei->nTabStopSize=nTabStopSize;
          ei->bTabStopAsSpaces=bTabStopAsSpaces;
          ei->nUndoLimit=nUndoLimit;
          ei->bDetailedUndo=bDetailedUndo;
          ei->bShowURL=bShowURL;
          ei->dwEditMargins=dwEditMargins;
          ei->ft=ftFileTime;
          ei->aec=aecColors;
          return TRUE;
        }
        return FALSE;
      }
      if (bMDI)
      {
        WNDFRAMEW *wf;
        HWND hWndFrame;

        if (hWndFrame=GetParent((HWND)wParam))
        {
          if (wf=(WNDFRAMEW *)GetWindowLongW(hWndFrame, GWL_USERDATA))
          {
            ei->hWndEdit=(HWND)wParam;
            ei->pFile=(unsigned char *)wf->wszFile;
            ei->nCodePage=wf->nCodePage;
            ei->bBOM=wf->bBOM;
            ei->nNewLine=wf->nNewLine;
            ei->bModified=wf->bModified;
            ei->bReadOnly=wf->bReadOnly;
            ei->bWordWrap=wf->bWordWrap;
            ei->bInsertState=wf->bInsertState;
            ei->nTabStopSize=wf->nTabStopSize;
            ei->bTabStopAsSpaces=bTabStopAsSpaces;
            ei->nUndoLimit=wf->nUndoLimit;
            ei->bDetailedUndo=bDetailedUndo;
            ei->bShowURL=wf->bShowURL;
            ei->dwEditMargins=wf->dwEditMargins;
            ei->ft=wf->ft;
            ei->aec=wf->aec;
            return TRUE;
          }
        }
      }
      return FALSE;
    }
    if (uMsg == AKD_GETFONT)
    {
      if (!wParam || (HWND)wParam == hWndEdit)
        return (LRESULT)&lfEditFontW;

      if (bMDI)
      {
        WNDFRAMEW *wf;
        HWND hWndFrame;

        if (hWndFrame=GetParent((HWND)wParam))
        {
          if (wf=(WNDFRAMEW *)GetWindowLongW(hWndFrame, GWL_USERDATA))
            return (LRESULT)&wf->lf;
        }
      }
      return (LRESULT)NULL;
    }
    if (uMsg == AKD_SETFONT)
    {
      SetChosenFontW((HWND)wParam, (LOGFONTW *)lParam);
      bEditFontChanged=TRUE;
      return 0;
    }
    if (uMsg == AKD_SETMODIFY)
    {
      SetModifyW((HWND)wParam, lParam, FALSE);
      return 0;
    }
    if (uMsg == AKD_SETNEWLINE)
    {
      SetNewLineW((HWND)wParam, lParam, FALSE);
      return 0;
    }
    if (uMsg == AKD_SETFILEPRINT)
    {
      bGlobalPrint=(BOOL)wParam;
      return 0;
    }
    if (uMsg == AKD_SETMSGCREATE)
    {
      nMsgCreate=(int)wParam;
      return 0;
    }
    if (uMsg == AKD_SETMSGBINARY)
    {
      nMsgBinary=(int)wParam;
      return 0;
    }
    if (uMsg == AKD_GETCODEPAGELIST)
    {
      int *nCodePage=(int *)wParam;

      if (nCodePage)
      {
        *nCodePage=nDefaultCodePage;
      }
      return (LRESULT)lpCodepageList;
    }
    if (uMsg == AKD_GETRECENTFILES)
    {
      RECENTFILESW *rf=(RECENTFILESW *)lParam;

      rf->lpwszRecentNames=lpwszRecentNames;
      rf->lpdwRecentPositions=lpdwRecentPositions;
      rf->lpdwRecentCodepages=lpdwRecentCodepages;
      rf->nRecentFiles=nRecentFiles;
      return 0;
    }
    if (uMsg == AKD_GETQUEUE)
    {
      return GetQueueStatus(wParam);
    }
    if (uMsg == AKD_WAITKEYBOARD)
    {
      BYTE lpKeyState[256];
      MSG msg;
      int i;

      Loop:
      Sleep(0);

      if (GetKeyboardState(lpKeyState))
      {
        for (i=0; i < 256; ++i)
        {
          if (lpKeyState[i] & 0x80)
          {
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
            {
              TranslateMessage(&msg);
              DispatchMessageW(&msg);
            }
            goto Loop;
          }
        }
      }
      return 0;
    }
    if (uMsg == AKD_GETPRINTDLG)
    {
      return (LRESULT)&pdW;
    }
    if (uMsg == AKD_GETPAGEDLG)
    {
      return (LRESULT)&psdPageW;
    }
    if (uMsg == AKD_GLOBALALLOC)
    {
      return (LRESULT)GlobalAlloc(wParam, lParam);
    }
    if (uMsg == AKD_GLOBALLOCK)
    {
      return (LRESULT)GlobalLock((HGLOBAL)wParam);
    }
    if (uMsg == AKD_GLOBALUNLOCK)
    {
      return (LRESULT)GlobalUnlock((HGLOBAL)wParam);
    }
    if (uMsg == AKD_GLOBALFREE)
    {
      return (LRESULT)GlobalFree((HGLOBAL)wParam);
    }
    if (uMsg == AKD_CREATEWINDOW)
    {
      CREATEWINDOWW *cw=(CREATEWINDOWW *)lParam;

      return (LRESULT)CreateWindowW(cw->wszClassName, cw->wszWindowName, cw->dwStyle, cw->x, cw->y, cw->nWidth, cw->nHeight, cw->hWndParent, cw->hMenu, cw->hInstance, cw->lpParam);
    }
    if (uMsg == AKD_GETMODELESS)
    {
      return (LRESULT)hDlgModeless;
    }
    if (uMsg == AKD_SETMODELESS)
    {
      hDlgModeless=(HWND)wParam;
      return 0;
    }
    if (uMsg == AKD_RESIZE)
    {
      UpdateSize();
      return 0;
    }
    if (uMsg == AKD_DOCK)
    {
      DOCK *lpDock=(DOCK *)lParam;
      DOCK *lpResult=NULL;

      if (wParam & DK_ADD)
      {
        lpResult=StackDockAdd(&hDocksStack, lpDock);
        lpDock=lpResult;
      }
      if (wParam & DK_SUBCLASS)
      {
        if (lpDock->hWnd)
        {
          OldDockProc=(WNDPROC)GetWindowLongW(lpDock->hWnd, GWL_WNDPROC);
          SetWindowLongW(lpDock->hWnd, GWL_WNDPROC, (LONG)DockProcW);
        }
      }
      if (wParam & DK_UNSUBCLASS)
      {
        if (lpDock->hWnd)
        {
          SetWindowLongW(lpDock->hWnd, GWL_WNDPROC, (LONG)OldDockProc);
        }
      }
      if ((wParam & DK_SETLEFT) ||
          (wParam & DK_SETRIGHT) ||
          (wParam & DK_SETTOP) ||
          (wParam & DK_SETBOTTOM))
      {
        int nSide=0;

        if (wParam & DK_SETLEFT) nSide=DKS_LEFT;
        else if (wParam & DK_SETRIGHT) nSide=DKS_RIGHT;
        else if (wParam & DK_SETTOP) nSide=DKS_TOP;
        else if (wParam & DK_SETBOTTOM) nSide=DKS_BOTTOM;

        if (nSide) DockSetSide(&hDocksStack, lpDock, nSide);
      }
      if (wParam & DK_HIDE)
      {
        if (lpDock->hWnd)
        {
          lpDock->dwFlags|=DKF_HIDDEN;
          ShowWindow(lpDock->hWnd, FALSE);
          UpdateSize();
        }
      }
      if (wParam & DK_SHOW)
      {
        if (lpDock->hWnd)
        {
          lpDock->dwFlags&=~DKF_HIDDEN;
          ShowWindow(lpDock->hWnd, TRUE);
          UpdateSize();
        }
      }
      if (wParam & DK_DELETE)
      {
        StackDockDelete(&hDocksStack, lpDock);
      }
      return (LRESULT)lpResult;
    }
    if (uMsg == AKD_GETCHARCOLOR)
    {
      CHARCOLOR *cc=(CHARCOLOR *)lParam;

      return GetCharColor((HWND)wParam, cc);
    }
  }

  if (uMsg == WM_COPYDATA)
  {
    COPYDATASTRUCT *cds=(COPYDATASTRUCT *)lParam;
    int nResult=0;

    if (cds->dwData == CD_OPENDOCUMENT)
    {
      OPENDOCUMENTW *od=(OPENDOCUMENTW *)cds->lpData;

      if ((od->hWnd && od->hWnd != hWndEdit) || bMDI || SaveChangedW())
      {
        if (*od->wszWorkDir) SetCurrentDirectoryW(od->wszWorkDir);
        nResult=OpenDocumentW(od->hWnd, od->wszFile, od->dwFlags, od->nCodePage, od->bBOM);
        if (*od->wszWorkDir) SetCurrentDirectoryW(wszExeDir);
      }
    }
    return nResult;
  }
  else if (uMsg == WM_INITMENU)
  {
    LPARAM lMenuState;

    if (!lParam || (lParam & IMENU_EDIT))
    {
      if (hWndEdit)
      {
        EnableMenuItem(hMainMenu, IDM_FILE_CREATENEW, (bMDI && bSingleOpenProgram)?MF_GRAYED:MF_ENABLED);
        EnableMenuItem(hMainMenu, IDM_FILE_REOPEN, (wszCurrentFile[0])?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_FILE_SAVE, bModified?MF_ENABLED:MF_GRAYED);

        EnableMenuItem(hMainMenu, IDM_EDIT_UNDO, (!bReadOnly && SendMessage(hWndEdit, EM_CANUNDO, 0, 0))?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_EDIT_REDO, (!bReadOnly && SendMessage(hWndEdit, EM_CANREDO, 0, 0))?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_EDIT_PASTE, (!bReadOnly && SendMessage(hWndEdit, EM_CANPASTE, 0, 0))?MF_ENABLED:MF_GRAYED);

        EnableMenuItem(hMainMenu, IDM_EDIT_CUT, (!bReadOnly && chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_EDIT_CLEAR, (!bReadOnly && chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);

        EnableMenuItem(hMainMenu, IDM_EDIT_COPY, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
        EnableMenuItem(hMainMenu, IDM_EDIT_DELETE_FIRST_CHAR_MENU, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);

        lMenuState=(!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))?MF_GRAYED:MF_ENABLED;
        EnableMenuItem(hMainMenu, IDM_EDIT_INSERT_TAB_MENU, lMenuState);
        EnableMenuItem(hMainMenu, IDM_EDIT_DELETE_TAB_MENU, lMenuState);
        EnableMenuItem(hMainMenu, IDM_EDIT_INSERT_SPACE_MENU, lMenuState);
        EnableMenuItem(hMainMenu, IDM_EDIT_DELETE_SPACE_MENU, lMenuState);
      }
      EnableMenuItem(hMainMenu, IDM_OPTIONS_EXEC, (*wszCommand)?MF_ENABLED:MF_GRAYED);
    }
    if (!lParam || (lParam & IMENU_CHECKS))
    {
      CheckMenuRadioItem(hMainMenu, IDM_EDIT_NEWLINE_WIN, IDM_EDIT_NEWLINE_UNIX, (nNewLine == NEWLINE_WIN)?IDM_EDIT_NEWLINE_WIN:IDM_EDIT_NEWLINE_UNIX, MF_BYCOMMAND);
      CheckMenuItem(hMainMenu, IDM_OPTIONS_READONLY, bReadOnly?MF_CHECKED:MF_UNCHECKED);
      CheckMenuItem(hMainMenu, IDM_VIEW_WORDWRAP, bWordWrap?MF_CHECKED:MF_UNCHECKED);
    }
    bMenuLanguage=TRUE;
    if (lParam) return TRUE;
  }
  else if (uMsg == WM_INITMENUPOPUP)
  {
    if ((HMENU)wParam == hMenuLanguage)
    {
      if (bMenuLanguage)
      {
        bMenuLanguage=FALSE;
        LanguageMenuW();
      }
    }
    else if ((HMENU)wParam == hMenuRecentFiles)
    {
      if (bMenuRecentFiles)
      {
        bMenuRecentFiles=FALSE;
        RecentFilesMenuW();
      }
    }
  }
  else if (uMsg == WM_MOVE)
  {
    DWORD dwStyle;

    dwStyle=GetWindowLongW(hWnd, GWL_STYLE);

    if (!(dwStyle & WS_MAXIMIZE) && !(dwStyle & WS_MINIMIZE))
    {
      GetWindowPos(hWnd, NULL, &rcMainWindowRestored);
    }
  }
  else if (uMsg == WM_SIZE)
  {
    if (wParam != SIZE_MINIMIZED)
    {
      if (wParam != SIZE_MAXIMIZED)
      {
        GetWindowPos(hWnd, NULL, &rcMainWindowRestored);
      }
      dwLastMainSize=wParam;
      UpdateSize();
      SendMessage(hStatus, WM_SIZE, wParam, lParam);
      return TRUE;
    }
  }
  else if (uMsg == WM_DROPFILES)
  {
    if (bMDI || SaveChangedW())
    {
      nPosition=nPositionOld;
      DragQueryFileW((HDROP)wParam, 0, wszFileBuffer, FILELIST_BUFFER_SIZE);
      OpenDocumentW(hWndEdit, wszFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, FALSE);
      DragFinish((HDROP)wParam);
    }
  }
  else if (uMsg == WM_COMMAND)
  {
    //WM_COMMAND (SDI & MDI)
    if (LOWORD(wParam) == IDM_RECENT_FILES)
    {
      int nDead;
      BOOL bMessageShow=TRUE;

      RecentFilesReadW();
      nDead=RecentFilesDeleteDeadW();
      RecentFilesSaveW();
      bMenuRecentFiles=TRUE;
      SendMessage(hMainWnd, AKDN_RECENTFILESDELETE, nDead, (LPARAM)&bMessageShow);

      if (bMessageShow)
      {
        API_LoadStringW(hLangLib, MSG_RECENTFILES_DELETED, wbuf, BUFFER_SIZE);
        wsprintfW(wbuf2, wbuf, nDead);
        MessageBoxW(hWnd, wbuf2, APP_MAIN_TITLEW, MB_OK|MB_ICONINFORMATION);
      }
    }
    else if (LOWORD(wParam) > IDM_RECENT_FILES && LOWORD(wParam) <= (IDM_RECENT_FILES + nRecentFiles))
    {
      if (bMDI || SaveChangedW())
      {
        lstrcpynW(wszFileBuffer, lpwszRecentNames[LOWORD(wParam) - IDM_RECENT_FILES - 1], MAX_PATH);
        OpenDocumentW(hWndEdit, wszFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, FALSE);
      }
    }
    else if (LOWORD(wParam) >= IDM_LANGUAGE && LOWORD(wParam) <= (IDM_LANGUAGE + nLangModules + 1))
    {
      if (LOWORD(wParam) == IDM_LANGUAGE)
      {
        if (!*wszLangModule) return TRUE;
        wszLangModule[0]='\0';
      }
      else
      {
        GetMenuStringW(hMainMenu, LOWORD(wParam), wbuf, BUFFER_SIZE, MF_BYCOMMAND);
        wsprintfW(wbuf2, L"%s.dll", wbuf);
        if (!lstrcmpiW(wszLangModule, wbuf2)) return TRUE;
        lstrcpynW(wszLangModule, wbuf2, MAX_PATH);
      }
      API_LoadStringW(hLangLib, MSG_RESTART_PROGRAM, wbuf, BUFFER_SIZE);
      MessageBoxW(hWnd, wbuf, APP_MAIN_TITLEW, MB_OK|MB_ICONEXCLAMATION);
    }
    else if (LOWORD(wParam) >= IDM_POPUP_OPENAS && LOWORD(wParam) <= (IDM_POPUP_OPENAS + nCodepageListLen + 1))
    {
      int nCodePageSel;

      nCodePageSel=LOWORD(wParam) - IDM_POPUP_OPENAS - 1;
      if (nCodePageSel < 0) nCodePageSel=nCodepageListLen - 2;
      DoFileReopenAsW(OD_ADT_DETECT_BOM, lpCodepageList[nCodePageSel], FALSE);
    }
    else if (LOWORD(wParam) >= IDM_POPUP_SAVEAS && LOWORD(wParam) <= (IDM_POPUP_SAVEAS + nCodepageListLen + 1))
    {
      int nCodePageSel;

      nCodePageSel=LOWORD(wParam) - IDM_POPUP_SAVEAS - 1;
      if (nCodePageSel < 0) nCodePageSel=nCodepageListLen - 2;
      return SaveDocumentW(hWndEdit, wszCurrentFile, lpCodepageList[nCodePageSel], TRUE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_FILE_NEW)
    {
      return DoFileNewW();
    }
    else if (LOWORD(wParam) == IDM_FILE_CREATENEW)
    {
      if (!bMDI)
      {
        if (nSaveSettings == SS_REGISTRY)
          RegSaveOptionsW();
        else
          IniSaveOptionsW();
      }
      return (int)DoFileNewWindowW(0);
    }
    else if (LOWORD(wParam) == IDM_FILE_OPEN)
    {
      return DoFileOpenW();
    }
    else if (LOWORD(wParam) == IDM_FILE_REOPEN)
    {
      DoFileReopenAsW(0, nCurrentCodePage, bCurrentBOM);
    }
    else if (LOWORD(wParam) == IDM_FILE_SAVE)
    {
      return DoFileSaveW();
    }
    else if (LOWORD(wParam) == IDM_FILE_SAVEAS)
    {
      return DoFileSaveAsW();
    }
    else if (LOWORD(wParam) == IDM_FILE_SAVEALL)
    {
      if (!bMDI)
      {
        return DoFileSaveW();
      }
      else
      {
        HWND hWndTmp=hWndFrameActive;

        do
        {
          if (!DoFileSaveW()) return FALSE;
          SendMessage(hMdiClient, WM_MDINEXT, (WPARAM)hWndFrameActive, FALSE);
        }
        while (hWndFrameActive != hWndTmp);

        return TRUE;
      }
    }
    else if (LOWORD(wParam) == IDM_FILE_CLOSEALL)
    {
      if (!bMDI)
      {
        return DoFileCloseW();
      }
      else
      {
        while (hWndFrameActive)
        {
          SendMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
          if (!bFileExitError) return FALSE;
        }
        return TRUE;
      }
    }
    else if (LOWORD(wParam) == IDM_FILE_CLOSEALL_BUTACTIVE)
    {
      if (bMDI)
      {
        HWND hWndTmp=hWndFrameActive;

        while (1)
        {
          SendMessage(hMdiClient, WM_MDINEXT, (WPARAM)hWndFrameActive, TRUE);
          if (hWndFrameActive == hWndTmp) break;

          SendMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
          if (!bFileExitError) return FALSE;
        }
        return TRUE;
      }
    }
    else if (LOWORD(wParam) == IDM_FILE_PAGESETUP)
    {
      return DoFilePageSetupW();
    }
    else if (LOWORD(wParam) == IDM_FILE_PRINT)
    {
      return DoFilePrintW(FALSE);
    }
    else if (LOWORD(wParam) == IDM_FILE_EXIT)
    {
      PostMessage(hWnd, WM_CLOSE, 0, 0);
    }
    else if (LOWORD(wParam) == IDM_EDIT_UNDO)
    {
      DoEditUndo(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_REDO)
    {
      DoEditRedo(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_CUT)
    {
      DoEditCutW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_COPY)
    {
      DoEditCopy(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_PASTE)
    {
      DoEditPaste(hWndEdit, FALSE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_CLEAR)
    {
      DoEditClearW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_SELECTALL)
    {
      DoEditSelectAll(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_INSERTCHAR)
    {
      DoEditInsertChar();
    }
    else if (LOWORD(wParam) == IDM_EDIT_INSERTDATE)
    {
      DoEditInsertDateW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_RECODE)
    {
      DoEditRecodeW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_INSERT_TAB_MENU ||
             LOWORD(wParam) == IDM_EDIT_INSERT_TAB)
    {
      if (!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))
        InsertTabStopW(hWndEdit);
      else
        IndentTabStopW(hWndEdit, STRSEL_INSERT|STRSEL_TAB);
    }
    else if (LOWORD(wParam) == IDM_EDIT_DELETE_TAB_MENU ||
             LOWORD(wParam) == IDM_EDIT_DELETE_TAB)
    {
      if (!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))
        InsertTabStopW(hWndEdit);
      else
        IndentTabStopW(hWndEdit, STRSEL_DELETE|STRSEL_TAB);
    }
    else if (LOWORD(wParam) == IDM_EDIT_INSERT_SPACE_MENU ||
             LOWORD(wParam) == IDM_EDIT_INSERT_SPACE)
    {
      if (!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))
      {
        if (!IsReadOnly()) SendMessage(hWndEdit, WM_CHAR, (WPARAM)' ', 0);
      }
      else
        DoEditInsertStringInSelectionW(hWndEdit, STRSEL_INSERT, L" ");
    }
    else if (LOWORD(wParam) == IDM_EDIT_DELETE_SPACE_MENU ||
             LOWORD(wParam) == IDM_EDIT_DELETE_SPACE)
    {
      if (!DoEditInsertStringInSelectionW(hWndEdit, STRSEL_CHECK, NULL))
      {
        if (!IsReadOnly()) SendMessage(hWndEdit, WM_CHAR, (WPARAM)' ', 0);
      }
      else
        DoEditInsertStringInSelectionW(hWndEdit, STRSEL_DELETE|STRSEL_SPACE, L" ");
    }
    else if (LOWORD(wParam) == IDM_EDIT_DELETE_FIRST_CHAR_MENU ||
             LOWORD(wParam) == IDM_EDIT_DELETE_FIRST_CHAR)
    {
      DoEditDeleteFirstCharW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_DELETE_TRAILING_WHITESPACES)
    {
      DoEditDeleteTrailingWhitespacesW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_UPPERCASE)
    {
      DoEditChangeCaseW(hWndEdit, UPPERCASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_LOWERCASE)
    {
      DoEditChangeCaseW(hWndEdit, LOWERCASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_SENTENCECASE)
    {
      DoEditChangeCaseW(hWndEdit, SENTENCECASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_TITLECASE)
    {
      DoEditChangeCaseW(hWndEdit, TITLECASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_INVERTCASE)
    {
      DoEditChangeCaseW(hWndEdit, INVERTCASE);
    }
    else if (LOWORD(wParam) == IDM_EDIT_NEWLINE_WIN)
    {
      if (!IsReadOnly())
      {
        if (nNewLine != NEWLINE_WIN)
        {
          SetNewLineW(hWndEdit, NEWLINE_WIN, FALSE);
          SetModifyW(hWndEdit, TRUE, FALSE);
        }
      }
    }
    else if (LOWORD(wParam) == IDM_EDIT_NEWLINE_UNIX)
    {
      if (!IsReadOnly())
      {
        if (nNewLine != NEWLINE_UNIX)
        {
          SetNewLineW(hWndEdit, NEWLINE_UNIX, FALSE);
          SetModifyW(hWndEdit, TRUE, FALSE);
        }
      }
    }
    else if (LOWORD(wParam) == IDM_EDIT_FIND)
    {
      DoEditFindW();
    }
    else if (LOWORD(wParam) == IDM_EDIT_FINDNEXTDOWN)
    {
      DoEditFindNextDownW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_FINDNEXTUP)
    {
      DoEditFindNextUpW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_EDIT_REPLACE)
    {
      DoEditReplaceW();
    }
    else if (LOWORD(wParam) == IDM_EDIT_GOTOLINE)
    {
      DoEditGoToLineW();
    }
    else if (LOWORD(wParam) == IDM_VIEW_FONT)
    {
      if (DoViewFontW(hMainWnd, &lfEditFontW))
      {
        SetChosenFontW(hWndEdit, &lfEditFontW);
        bEditFontChanged=TRUE;
        return TRUE;
      }
      return FALSE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_TEXT_COLOR)
    {
      if (DoViewColorW(hMainWnd, &aecColors.crBasicText))
      {
        aecColors.dwFlags=AECLR_ALL;
        aecColors.crActiveLineText=aecColors.crBasicText;
        SetChosenFontColorW(hWndEdit, aecColors.crBasicText);
        bColorsChanged=TRUE;
        return TRUE;
      }
      return FALSE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_BG_COLOR)
    {
      if (DoViewColorW(hMainWnd, &aecColors.crBasicBk))
      {
        aecColors.dwFlags=AECLR_ALL;
        aecColors.crActiveLineBk=aecColors.crBasicBk;
        SendMessage(hWndEdit, EM_SETBKGNDCOLOR, 0, aecColors.crBasicBk);
        bColorsChanged=TRUE;
        return TRUE;
      }
      return FALSE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_INCREASE_FONT)
    {
      DoViewFontSizeW(hWndEdit, INCREASE_FONT);
      bEditFontChanged=TRUE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_DECREASE_FONT)
    {
      DoViewFontSizeW(hWndEdit, DECREASE_FONT);
      bEditFontChanged=TRUE;
    }
    else if (LOWORD(wParam) == IDM_VIEW_ALIGN_CENTER)
    {
      DoViewAlignmentW(hWndEdit, PFA_CENTER);
    }
    else if (LOWORD(wParam) == IDM_VIEW_ALIGN_RIGHT)
    {
      DoViewAlignmentW(hWndEdit, PFA_RIGHT);
    }
    else if (LOWORD(wParam) == IDM_VIEW_ALIGN_LEFT)
    {
      DoViewAlignmentW(hWndEdit, PFA_LEFT);
    }
    else if (LOWORD(wParam) == IDM_VIEW_WORDWRAP)
    {
      DoViewWordWrap(hWndEdit, !bWordWrap, FALSE);
    }
    else if (LOWORD(wParam) == IDM_VIEW_ONTOP)
    {
      DoViewOnTop(!bOnTop, FALSE);
    }
    else if (LOWORD(wParam) == IDM_VIEW_SHOW_STATUSBAR)
    {
      DoViewShowStatusBar(!bStatusBar, FALSE);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_EXEC)
    {
      return DoSettingsExecW();
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_READONLY)
    {
      DoSettingsReadOnly(hWndEdit, !bReadOnly, FALSE);
      if (hDlgModeless) PostMessage(hDlgModeless, WM_COMMAND, IDC_SETREADONLY, 0);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_SAVETIME)
    {
      DoSettingsSaveTime(!bSaveTime);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_KEEPSPACE)
    {
      DoSettingsKeepSpace(!bKeepSpace);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_WATCHFILE)
    {
      DoSettingsWatchFile(!bWatchFile);
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_SINGLEOPEN_FILE)
    {
      DoSettingsSingleOpenFile(!bSingleOpenFile);
      if (nSaveSettings == SS_REGISTRY)
        RegSaveOptionsW();
      else
        IniSaveOptionsW();
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_SINGLEOPEN_PROGRAM)
    {
      DoSettingsSingleOpenProgram(!bSingleOpenProgram);
      if (nSaveSettings == SS_REGISTRY)
        RegSaveOptionsW();
      else
        IniSaveOptionsW();
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_SDI)
    {
      if (bRegMDI)
      {
        CheckMenuRadioItem(hMainMenu, IDM_OPTIONS_SDI, IDM_OPTIONS_MDI, IDM_OPTIONS_SDI, MF_BYCOMMAND);

        API_LoadStringW(hLangLib, MSG_RESTART_PROGRAM, wbuf, BUFFER_SIZE);
        MessageBoxW(hWnd, wbuf, APP_MAIN_TITLEW, MB_OK|MB_ICONEXCLAMATION);
        bRegMDI=FALSE;
      }
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_MDI)
    {
      if (!bRegMDI)
      {
        CheckMenuRadioItem(hMainMenu, IDM_OPTIONS_SDI, IDM_OPTIONS_MDI, IDM_OPTIONS_MDI, MF_BYCOMMAND);

        API_LoadStringW(hLangLib, MSG_RESTART_PROGRAM, wbuf, BUFFER_SIZE);
        MessageBoxW(hWnd, wbuf, APP_MAIN_TITLEW, MB_OK|MB_ICONEXCLAMATION);
        bRegMDI=TRUE;
      }
    }
    else if (LOWORD(wParam) == IDM_OPTIONS)
    {
      DoSettingsOptionsW();
    }
    else if (LOWORD(wParam) == IDM_OPTIONS_PLUGINS)
    {
      DoSettingsPluginsW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_ABOUT)
    {
      DoHelpAboutW();
    }
    else if (LOWORD(wParam) == IDM_NONMENU_CHANGESIZE)
    {
      ShowWindow(hWnd, (GetWindowLongW(hWnd, GWL_STYLE) & WS_MAXIMIZE)?SW_RESTORE:SW_MAXIMIZE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_DELLINE)
    {
      DoNonMenuDelLineW(hWndEdit);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_PASTEANSI)
    {
      DoEditPaste(hWndEdit, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPEN_MSG)
    {
      if (hWndEdit == (HWND)lParam)
      {
        API_LoadStringW(hLangLib, MSG_FILE_CHANGED, wbuf, BUFFER_SIZE);
        wsprintfW(wbuf2, wbuf, wszCurrentFile);
        if (MessageBoxW(hWnd, wbuf2, APP_MAIN_TITLEW, bModified?MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2:MB_YESNO|MB_ICONQUESTION) == IDYES)
        {
          bMdiReopen=TRUE;
          OpenDocumentW(hWndEdit, wszCurrentFile, 0, nCurrentCodePage, bCurrentBOM);
          bMdiReopen=FALSE;
        }
      }
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REDETECT)
    {
      DoFileReopenAsW(OD_ADT_DETECT_CODEPAGE|OD_ADT_DETECT_BOM, 0, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_ANSI)
    {
      DoFileReopenAsW(0, nAnsiCodePage, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_OEM)
    {
      DoFileReopenAsW(0, nOemCodePage, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_UTF16LE)
    {
      DoFileReopenAsW(OD_ADT_DETECT_BOM, CP_UNICODE_UCS2_LE, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_UTF16BE)
    {
      DoFileReopenAsW(OD_ADT_DETECT_BOM, CP_UNICODE_UCS2_BE, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_UTF8)
    {
      DoFileReopenAsW(OD_ADT_DETECT_BOM, CP_UNICODE_UTF8, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_REOPENAS_KOIR)
    {
      DoFileReopenAsW(0, CP_KOI8_R, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_ANSI)
    {
      return SaveDocumentW(hWndEdit, wszCurrentFile, nAnsiCodePage, FALSE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_OEM)
    {
      return SaveDocumentW(hWndEdit, wszCurrentFile, nOemCodePage, FALSE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_UTF16LE)
    {
      return SaveDocumentW(hWndEdit, wszCurrentFile, CP_UNICODE_UCS2_LE, TRUE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_UTF16BE)
    {
      return SaveDocumentW(hWndEdit, wszCurrentFile, CP_UNICODE_UCS2_BE, TRUE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_UTF8)
    {
      return SaveDocumentW(hWndEdit, wszCurrentFile, CP_UNICODE_UTF8, TRUE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_SAVEAS_KOIR)
    {
      return SaveDocumentW(hWndEdit, wszCurrentFile, CP_KOI8_R, FALSE, TRUE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_INSERTMODE)
    {
      if (hWndEdit) SetInsertStateW(hWndEdit, !bInsertState, FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_FILECLOSE)
    {
      return DoFileCloseW();
    }
    else if (LOWORD(wParam) == IDM_NONMENU_DLGNEXT)
    {
      return (LRESULT)NextDialog(FALSE);
    }
    else if (LOWORD(wParam) == IDM_NONMENU_DLGPREV)
    {
      return (LRESULT)NextDialog(TRUE);
    }
    else if (LOWORD(wParam) == IDM_POPUP_CODEPAGEMENU)
    {
      RECT rc;

      SendMessage(hStatus, SB_GETRECT, STATUS_CODEPAGE, (LPARAM)&rc);
      ClientToScreen(hStatus, (POINT *)&rc);
      ShowMenuPopupCodepageW((POINT *)&rc);
    }

    //WM_COMMAND (MDI)
    if (bMDI)
    {
      if (LOWORD(wParam) == IDM_WINDOW_TABVIEW_TOP)
      {
        DoWindowTabView(TAB_VIEW_TOP, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABVIEW_BOTTOM)
      {
        DoWindowTabView(TAB_VIEW_BOTTOM, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABVIEW_NONE)
      {
        DoWindowTabView(TAB_VIEW_NONE, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABTYPE_STANDARD)
      {
        DoWindowTabType(TAB_TYPE_STANDARD, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABTYPE_BUTTONS)
      {
        DoWindowTabType(TAB_TYPE_BUTTONS, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABTYPE_FLATBUTTONS)
      {
        DoWindowTabType(TAB_TYPE_FLATBUTTONS, FALSE);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABSWITCH_NEXTPREV)
      {
        if (nTabSwitch == TAB_SWITCH_RIGHTLEFT)
        {
          CheckMenuRadioItem(hMainMenu, IDM_WINDOW_TABSWITCH_NEXTPREV, IDM_WINDOW_TABSWITCH_RIGHTLEFT, IDM_WINDOW_TABSWITCH_NEXTPREV, MF_BYCOMMAND);
          nTabSwitch=TAB_SWITCH_NEXTPREV;
        }
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TABSWITCH_RIGHTLEFT)
      {
        if (nTabSwitch == TAB_SWITCH_NEXTPREV)
        {
          CheckMenuRadioItem(hMainMenu, IDM_WINDOW_TABSWITCH_NEXTPREV, IDM_WINDOW_TABSWITCH_RIGHTLEFT, IDM_WINDOW_TABSWITCH_RIGHTLEFT, MF_BYCOMMAND);
          nTabSwitch=TAB_SWITCH_RIGHTLEFT;
        }
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TILEHORIZONTAL)
      {
        SendMessage(hMdiClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_TILEVERTICAL)
      {
        SendMessage(hMdiClient, WM_MDITILE, MDITILE_VERTICAL, 0);
      }
      else if (LOWORD(wParam) == IDM_WINDOW_CASCADE)
      {
        SendMessage(hMdiClient, WM_MDICASCADE, 0, 0);
      }
      else if (LOWORD(wParam) == IDM_NONMENU_MDINEXT)
      {
        SendMessage(hMdiClient, WM_MDINEXT, (WPARAM)hWndFrameActive, FALSE);
      }
      else if (LOWORD(wParam) == IDM_NONMENU_MDIPREV)
      {
        SendMessage(hMdiClient, WM_MDINEXT, (WPARAM)hWndFrameActive, TRUE);
      }
      else if (LOWORD(wParam) == IDM_NONMENU_MDICLOSE)
      {
        SendMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
        return bFileExitError;
      }
    }
  }
  else if (uMsg == WM_NOTIFY)
  {
    //WM_NOTIFY (SDI & MDI)
    if (wParam == ID_STATUS && (!bMDI || hWndFrameActive))
    {
      if ((int)((NMHDR *)lParam)->code == NM_DBLCLK)
      {
        if (((NMMOUSE *)lParam)->dwItemSpec == 0)
        {
          DoEditGoToLineW();
        }
        else if (((NMMOUSE *)lParam)->dwItemSpec == 2)
        {
          SetInsertStateW(hWndEdit, !bInsertState, FALSE);
        }
        else if (((NMMOUSE *)lParam)->dwItemSpec == 3)
        {
          if (!IsReadOnly())
          {
            if (nNewLine == NEWLINE_WIN) SetNewLineW(hWndEdit, NEWLINE_UNIX, FALSE);
            else if (nNewLine == NEWLINE_UNIX) SetNewLineW(hWndEdit, NEWLINE_WIN, FALSE);
            SetModifyW(hWndEdit, TRUE, FALSE);
          }
        }
      }
      else if ((int)((NMHDR *)lParam)->code == NM_RCLICK)
      {
        if (((NMMOUSE *)lParam)->dwItemSpec == 4)
        {
          POINT pt;

          GetCursorPos(&pt);
          ShowMenuPopupCodepageW(&pt);
        }
      }
    }

    //WM_NOTIFY (MDI)
    if (bMDI)
    {
      if (wParam == ID_TAB)
      {
        if (((NMHDR *)lParam)->code == TCN_SELCHANGE)
        {
          TCITEMW tcItemW;
          int nItem;

          bTabPressed=TRUE;
          nItem=SendMessage(hTab, TCM_GETCURSEL, 0, 0);
          tcItemW.mask=TCIF_PARAM;
          SendMessage(hTab, TCM_GETITEMW, nItem, (LPARAM)&tcItemW);
          SendMessage(hMdiClient, WM_MDIACTIVATE, (WPARAM)tcItemW.lParam, 0);
          bTabPressed=FALSE;
        }
      }
    }
  }
  else if (uMsg == WM_CLOSE)
  {
    if (!bMDI)
    {
      if (!DoFileExitW()) return TRUE;
    }
    else
    {
      bMdiClientRedraw=FALSE;

      while (hWndFrameActive)
      {
        SendMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
        if (!bFileExitError)
        {
          bMdiClientRedraw=TRUE;
          return TRUE;
        }
      }
    }

    //Close modeless dialog
    if (hDlgModeless)
      SendMessage(hDlgModeless, WM_COMMAND, IDCANCEL, 0);

    //Main window will be destroyed
    PostMessage(hWnd, AKDN_MAIN_ONFINISH, 0, 0);

    return 0;
  }
  else if (uMsg == WM_DESTROY)
  {
    //Save options
    if (nSaveSettings == SS_REGISTRY)
      RegSaveOptionsW();
    else
      IniSaveOptionsW();

    //Save plugin stack
    if (bSavePluginsStackOnExit)
      StackPluginSaveW(&hPluginsStack, DLLS_CLEAR);

    //Clean up
    if (bMDI)
    {
      ImageList_Destroy(hImageList);
      DestroyCursor(hCursorDragMove);
      DestroyIcon(hIconEmpty);
    }
    DestroyAcceleratorTable(hMainAccel);
    DestroyAcceleratorTable(hGlobalAccel);
    DestroyIcon(hMainIcon);
    DestroyMenu(hMainMenu);
    DestroyMenu(hPopupMenu);
    CodepageListFree(&lpCodepageList);
    FreeMemoryRecentFilesW();
    FreeMemorySearchW();

    PostQuitMessage(0);
    return 0;
  }

  //Special messages
  if (!bMDI)
  {
    LRESULT lResult;

    if (lResult=EditParentMessagesW(hWnd, uMsg, wParam, lParam)) return lResult;
  }
  else
  {
    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_MBUTTONDOWN)
    {
      if (hWndFrameActive) SendMessage(hMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMdiMaximize);
      CreateMDIWindowW(APP_MDI_CLASSW, L"", (bMdiMaximize == TRUE)?WS_MAXIMIZE:0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hMdiClient, hInstance, 0);
    }
  }

  if (!bMDI)
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
  else
    return DefFrameProcW(hWnd, hMdiClient, uMsg, wParam, lParam);
}

LRESULT CALLBACK EditParentMessagesA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_SETFOCUS)
  {
    SetFocus(hWndEdit);

    if (bWatchFile && szCurrentFile[0] && (ftFileTime.dwLowDateTime || ftFileTime.dwHighDateTime))
    {
      FILETIME ftTmp;

      if (!FileExistsA(szCurrentFile))
      {
        memset(&ftFileTime, 0, sizeof(FILETIME));

        API_LoadStringA(hLangLib, MSG_ERROR_CANNOT_OPEN_FILE, buf, BUFFER_SIZE);
        wsprintfA(buf2, buf, szCurrentFile);
        MessageBoxA(NULL, buf2, APP_MAIN_TITLEA, MB_OK|MB_ICONEXCLAMATION);
      }
      else if (GetFileWriteTimeA(szCurrentFile, &ftTmp))
      {
        if (CompareFileTime(&ftFileTime, &ftTmp))
        {
          ftFileTime=ftTmp;
          PostMessage(hMainWnd, WM_COMMAND, IDM_NONMENU_REOPEN_MSG, (LPARAM)hWndEdit);
        }
      }
      else
      {
        memset(&ftFileTime, 0, sizeof(FILETIME));
      }
    }
  }
  else if (uMsg == WM_CONTEXTMENU)
  {
    if ((HWND)wParam == hWndEdit)
    {
      NCONTEXTMENU ncm;
      RECT rc;
      POINT pt;
      DWORD dwPoint;
      HMENU hTrackMenu;

      if (lParam == -1)
      {
        dwPoint=SendMessage(hWndEdit, EM_POSFROMCHAR, nPosition, 0);
        pt.x=LOWORD(dwPoint);
        pt.y=HIWORD(dwPoint) - lfEditFontA.lfHeight;
        ClientToScreen(hWndEdit, &pt);
        GetWindowRect(hWndEdit, &rc);
        if (pt.x > rc.right || pt.y > rc.bottom)
        {
          pt.x=rc.left;
          pt.y=rc.top;
        }
      }
      else
      {
        pt.x=LOWORD(lParam);
        pt.y=HIWORD(lParam);
      }

      ncm.hWnd=hWndEdit;
      ncm.uType=NCM_EDIT;
      ncm.pt=pt;
      ncm.bProcess=TRUE;
      SendMessage(hMainWnd, AKDN_CONTEXTMENU, 0, (LPARAM)&ncm);
      if (ncm.bProcess)
      {
        if (bReadOnly)
        {
          hTrackMenu=hPopupView;
          EnableMenuItem(hTrackMenu, IDM_EDIT_COPY, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
        }
        else
        {
          hTrackMenu=hPopupEdit;
          EnableMenuItem(hTrackMenu, IDM_EDIT_UNDO, SendMessage(hWndEdit, EM_CANUNDO, 0, 0)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_REDO, SendMessage(hWndEdit, EM_CANREDO, 0, 0)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_PASTE, SendMessage(hWndEdit, EM_CANPASTE, 0, 0)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_CUT, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_CLEAR, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_COPY, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
        }
        TrackPopupMenu(hTrackMenu, TPM_LEFTBUTTON|TPM_RIGHTBUTTON, pt.x, pt.y, 0, hMainWnd, NULL);
      }
    }
  }
  else if (uMsg == WM_COMMAND)
  {
    if (LOWORD(wParam) == ID_EDIT)
    {
      if (HIWORD(wParam) == EN_CHANGE)
      {
        if ((HWND)lParam != hWndEdit)
        {
          //Drag'n'Drop between tabs
          WNDFRAMEA *wf;

          if (wf=(WNDFRAMEA *)GetWindowLongA(GetParent((HWND)lParam), GWL_USERDATA))
            wf->bModified=TRUE;
        }
        else
        {
          if (bDetailedUndo && chrg.cpMin == chrg.cpMax)
            SendMessage(hWndEdit, EM_STOPGROUPTYPING, 0, 0);
          SetModifyA(hWndEdit, TRUE, FALSE);
        }
      }
      else if (HIWORD(wParam) == EN_MAXTEXT)
      {
        //trying to reallocate
        API_LoadStringA(hLangLib, MSG_ERROR_LIMIT_REACHED, buf, BUFFER_SIZE);
        MessageBoxA(hMainWnd, buf, APP_MAIN_TITLEA, MB_OK|MB_ICONERROR);
        nMaxChars+=nMcstep;
        SendMessage(hWndEdit, EM_EXLIMITTEXT, 0, nMaxChars);
      }
      else if (HIWORD(wParam) == EN_ERRSPACE)
      {
        API_LoadStringA(hLangLib, MSG_ERROR_NOT_ENOUGH_MEMORY_FOR_RICHEDIT, buf, BUFFER_SIZE);
        MessageBoxA(hMainWnd, buf, APP_MAIN_TITLEA, MB_OK|MB_ICONERROR);
        ExitProcess(1);
      }
    }
  }
  else if (uMsg == WM_NOTIFY)
  {
    if (wParam == ID_EDIT)
    {
      if (((NMHDR *)lParam)->code == EN_SELCHANGE)
      {
        char szStatus[MAX_PATH];
        CHARRANGE cr;
        int nLine;
        int nColumn;
        int nLinebreaks=0;

        SendMessage(hWndEdit, EM_EXGETSEL, 0, (LPARAM)&cr);
        nPositionOld=nPosition;
        nPosition=(cr.cpMin == chrg.cpMin || cr.cpMin == chrg.cpMax)?cr.cpMax:cr.cpMin;
        chrg.cpMin=cr.cpMin;
        chrg.cpMax=cr.cpMax;

        nLine=SendMessage(hWndEdit, EM_EXLINEFROMCHAR, 0, nPosition) + 1;
        nColumn=SendMessage(hWndEdit, EM_LINEINDEX, nLine - 1, 0);
        nColumn=nPosition - nColumn + 1;

        if (chrg.cpMin == chrg.cpMax)
        {
          wsprintfA(szStatus, "%u:%u", nLine, nColumn);
        }
        else
        {
          if (nNewLine == NEWLINE_WIN) nLinebreaks=SendMessage(hWndEdit, EM_EXLINEFROMCHAR, 0, chrg.cpMax) - SendMessage(hWndEdit, EM_EXLINEFROMCHAR, 0, chrg.cpMin);
          wsprintfA(szStatus, "%u:%u, %u", nLine, nColumn, chrg.cpMax - chrg.cpMin + nLinebreaks);
        }
        SendMessage(hStatus, SB_SETTEXTA, STATUS_POSITION, (LPARAM)szStatus);
      }
      else if (((NMHDR *)lParam)->code == EN_LINK)
      {
        if ((nClickURL == 1 && ((ENLINK *)lParam)->msg == WM_LBUTTONUP) ||
            (nClickURL == 2 && ((ENLINK *)lParam)->msg == WM_LBUTTONDBLCLK))
        {
          char *szURL;

          if (chrg.cpMin == chrg.cpMax)
          {
            if (GetRangeTextA(hWndEdit, ((ENLINK *)lParam)->chrg.cpMin, ((ENLINK *)lParam)->chrg.cpMax, &szURL))
            {
              ShellExecuteA(hWndEdit, "open", szURL, NULL, NULL, SW_SHOWNORMAL);
              FreeText(szURL);
            }
          }
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

LRESULT CALLBACK EditParentMessagesW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_SETFOCUS)
  {
    SetFocus(hWndEdit);

    if (bWatchFile && wszCurrentFile[0] && (ftFileTime.dwLowDateTime || ftFileTime.dwHighDateTime))
    {
      FILETIME ftTmp;

      if (!FileExistsW(wszCurrentFile))
      {
        memset(&ftFileTime, 0, sizeof(FILETIME));

        API_LoadStringW(hLangLib, MSG_ERROR_CANNOT_OPEN_FILE, wbuf, BUFFER_SIZE);
        wsprintfW(wbuf2, wbuf, wszCurrentFile);
        MessageBoxW(NULL, wbuf2, APP_MAIN_TITLEW, MB_OK|MB_ICONEXCLAMATION);
      }
      else if (GetFileWriteTimeW(wszCurrentFile, &ftTmp))
      {
        if (CompareFileTime(&ftFileTime, &ftTmp))
        {
          ftFileTime=ftTmp;
          PostMessage(hMainWnd, WM_COMMAND, IDM_NONMENU_REOPEN_MSG, (LPARAM)hWndEdit);
        }
      }
      else
      {
        memset(&ftFileTime, 0, sizeof(FILETIME));
      }
    }
  }
  else if (uMsg == WM_CONTEXTMENU)
  {
    if ((HWND)wParam == hWndEdit)
    {
      NCONTEXTMENU ncm;
      RECT rc;
      POINT pt;
      DWORD dwPoint;
      HMENU hTrackMenu;

      if (lParam == -1)
      {
        dwPoint=SendMessage(hWndEdit, EM_POSFROMCHAR, nPosition, 0);
        pt.x=LOWORD(dwPoint);
        pt.y=HIWORD(dwPoint) - lfEditFontW.lfHeight;
        ClientToScreen(hWndEdit, &pt);
        GetWindowRect(hWndEdit, &rc);
        if (pt.x > rc.right || pt.y > rc.bottom)
        {
          pt.x=rc.left;
          pt.y=rc.top;
        }
      }
      else
      {
        pt.x=LOWORD(lParam);
        pt.y=HIWORD(lParam);
      }

      ncm.hWnd=hWndEdit;
      ncm.uType=NCM_EDIT;
      ncm.pt=pt;
      ncm.bProcess=TRUE;
      SendMessage(hMainWnd, AKDN_CONTEXTMENU, 0, (LPARAM)&ncm);
      if (ncm.bProcess)
      {
        if (bReadOnly)
        {
          hTrackMenu=hPopupView;
          EnableMenuItem(hTrackMenu, IDM_EDIT_COPY, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
        }
        else
        {
          hTrackMenu=hPopupEdit;
          EnableMenuItem(hTrackMenu, IDM_EDIT_UNDO, SendMessage(hWndEdit, EM_CANUNDO, 0, 0)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_REDO, SendMessage(hWndEdit, EM_CANREDO, 0, 0)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_PASTE, SendMessage(hWndEdit, EM_CANPASTE, 0, 0)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_CUT, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_CLEAR, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
          EnableMenuItem(hTrackMenu, IDM_EDIT_COPY, (chrg.cpMin < chrg.cpMax)?MF_ENABLED:MF_GRAYED);
        }
        TrackPopupMenu(hTrackMenu, TPM_LEFTBUTTON|TPM_RIGHTBUTTON, pt.x, pt.y, 0, hMainWnd, NULL);
      }
    }
  }
  else if (uMsg == WM_COMMAND)
  {
    if (LOWORD(wParam) == ID_EDIT)
    {
      if (HIWORD(wParam) == EN_CHANGE)
      {
        if ((HWND)lParam != hWndEdit)
        {
          //Drag'n'Drop between tabs
          WNDFRAMEW *wf;

          if (wf=(WNDFRAMEW *)GetWindowLongW(GetParent((HWND)lParam), GWL_USERDATA))
            wf->bModified=TRUE;
        }
        else
        {
          if (bDetailedUndo && chrg.cpMin == chrg.cpMax)
            SendMessage(hWndEdit, EM_STOPGROUPTYPING, 0, 0);
          SetModifyW(hWndEdit, TRUE, FALSE);
        }
      }
      else if (HIWORD(wParam) == EN_MAXTEXT)
      {
        //trying to reallocate
        API_LoadStringW(hLangLib, MSG_ERROR_LIMIT_REACHED, wbuf, BUFFER_SIZE);
        MessageBoxW(hMainWnd, wbuf, APP_MAIN_TITLEW, MB_OK|MB_ICONERROR);
        nMaxChars+=nMcstep;
        SendMessage(hWndEdit, EM_EXLIMITTEXT, 0, nMaxChars);
      }
      else if (HIWORD(wParam) == EN_ERRSPACE)
      {
        API_LoadStringW(hLangLib, MSG_ERROR_NOT_ENOUGH_MEMORY_FOR_RICHEDIT, wbuf, BUFFER_SIZE);
        MessageBoxW(hMainWnd, wbuf, APP_MAIN_TITLEW, MB_OK|MB_ICONERROR);
        ExitProcess(1);
      }
    }
  }
  else if (uMsg == WM_NOTIFY)
  {
    if (wParam == ID_EDIT)
    {
      if (((NMHDR *)lParam)->code == EN_SELCHANGE)
      {
        wchar_t wszStatus[MAX_PATH];
        CHARRANGE cr;
        int nLine;
        int nColumn;
        int nLinebreaks=0;

        SendMessage(hWndEdit, EM_EXGETSEL, 0, (LPARAM)&cr);
        nPositionOld=nPosition;
        nPosition=(cr.cpMin == chrg.cpMin || cr.cpMin == chrg.cpMax)?cr.cpMax:cr.cpMin;
        chrg.cpMin=cr.cpMin;
        chrg.cpMax=cr.cpMax;

        nLine=SendMessage(hWndEdit, EM_EXLINEFROMCHAR, 0, nPosition) + 1;
        nColumn=SendMessage(hWndEdit, EM_LINEINDEX, nLine - 1, 0);
        nColumn=nPosition - nColumn + 1;

        if (chrg.cpMin == chrg.cpMax)
        {
          wsprintfW(wszStatus, L"%u:%u", nLine, nColumn);
        }
        else
        {
          if (nNewLine == NEWLINE_WIN) nLinebreaks=SendMessage(hWndEdit, EM_EXLINEFROMCHAR, 0, chrg.cpMax) - SendMessage(hWndEdit, EM_EXLINEFROMCHAR, 0, chrg.cpMin);
          wsprintfW(wszStatus, L"%u:%u, %u", nLine, nColumn, chrg.cpMax - chrg.cpMin + nLinebreaks);
        }
        SendMessage(hStatus, SB_SETTEXTW, STATUS_POSITION, (LPARAM)wszStatus);
      }
      else if (((NMHDR *)lParam)->code == EN_LINK)
      {
        if ((nClickURL == 1 && ((ENLINK *)lParam)->msg == WM_LBUTTONUP) ||
            (nClickURL == 2 && ((ENLINK *)lParam)->msg == WM_LBUTTONDBLCLK))
        {
          wchar_t *wszURL;

          if (chrg.cpMin == chrg.cpMax)
          {
            if (GetRangeTextW(hWndEdit, ((ENLINK *)lParam)->chrg.cpMin, ((ENLINK *)lParam)->chrg.cpMax, &wszURL))
            {
              ShellExecuteW(hWndEdit, L"open", wszURL, NULL, NULL, SW_SHOWNORMAL);
              FreeText(wszURL);
            }
          }
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

LRESULT CALLBACK CommonFrameProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult;

  lResult=lpfnFrameProcA(hWnd, uMsg, wParam, lParam);

  if (lpfnFrameProcRetA)
  {
    CWPRETSTRUCT cwprs;

    cwprs.lResult=lResult;
    cwprs.lParam=lParam;
    cwprs.wParam=wParam;
    cwprs.message=uMsg;
    cwprs.hwnd=hWnd;
    lpfnFrameProcRetA(&cwprs);
  }
  return lResult;
}

LRESULT CALLBACK CommonFrameProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult;

  lResult=lpfnFrameProcW(hWnd, uMsg, wParam, lParam);

  if (lpfnFrameProcRetW)
  {
    CWPRETSTRUCT cwprs;

    cwprs.lResult=lResult;
    cwprs.lParam=lParam;
    cwprs.wParam=wParam;
    cwprs.message=uMsg;
    cwprs.hwnd=hWnd;
    lpfnFrameProcRetW(&cwprs);
  }
  return lResult;
}

LRESULT CALLBACK FrameProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CREATE)
  {
    if (lpWndFrameA=(WNDFRAMEA *)API_HeapAlloc(hHeap, 0, sizeof(WNDFRAMEA)))
    {
      TCITEMA tcItemA;
      int nIndex;
      int nItemSum;

      CreateEditWindowA(hWnd);

      lpWndFrameA->hIcon=hIconEmpty;
      lpWndFrameA->szFile[0]='\0';
      lpWndFrameA->nCodePage=nDefaultCodePage;
      lpWndFrameA->bBOM=bDefaultBOM;
      lpWndFrameA->nNewLine=NEWLINE_WIN;
      lpWndFrameA->bModified=FALSE;
      lpWndFrameA->bReadOnly=bReadOnly;
      lpWndFrameA->bWordWrap=bWordWrap;
      lpWndFrameA->bInsertState=FALSE;
      lpWndFrameA->nTabStopSize=nTabStopSize;
      lpWndFrameA->nUndoLimit=nUndoLimit;
      lpWndFrameA->bShowURL=bShowURL;
      lpWndFrameA->dwEditMargins=dwEditMargins;
      lpWndFrameA->bDelimitersEnable=bDelimitersEnable;
      memset(&lpWndFrameA->ft, 0, sizeof(FILETIME));
      memcpy(&lpWndFrameA->lf, &lfEditFontA, sizeof(LOGFONTA));
      lpWndFrameA->aec=aecColors;
      SetWindowLongA(hWnd, GWL_USERDATA, (LONG)lpWndFrameA);

      nIndex=ImageList_AddIcon(hImageList, hIconEmpty);
      tcItemA.mask=TCIF_TEXT|TCIF_IMAGE|TCIF_PARAM;
      tcItemA.pszText="";
      tcItemA.iImage=nIndex;
      tcItemA.lParam=(LPARAM)hWnd;
      nItemSum=SendMessage(hTab, TCM_GETITEMCOUNT, 0, 0);
      SendMessage(hTab, TCM_INSERTITEMA, nItemSum, (LPARAM)&tcItemA);
      SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconEmpty);

      if (!hWndFrameActive) InvalidateRect(hTab, NULL, TRUE);
    }
  }
  else if (uMsg == WM_SIZE)
  {
    if (wParam != SIZE_MINIMIZED)
    {
      HWND hWndTmp;

      if (hWndTmp=GetDlgItem(hWnd, ID_EDIT))
        ResizeEdit(hWndTmp, 0, 0, LOWORD(lParam), HIWORD(lParam));
    }
  }
  else if (uMsg == WM_MDIACTIVATE)
  {
    NMHDR nmhdr;
    int i;
    int nMessages[]={IDM_FILE_REOPEN,
                     IDM_FILE_SAVE,
                     IDM_FILE_SAVEAS,
                     IDM_FILE_SAVEALL,
                     IDM_FILE_PAGESETUP,
                     IDM_FILE_PRINT,
                     IDM_EDIT_UNDO,
                     IDM_EDIT_REDO,
                     IDM_EDIT_CUT,
                     IDM_EDIT_COPY,
                     IDM_EDIT_PASTE,
                     IDM_EDIT_CLEAR,
                     IDM_EDIT_SELECTALL,
                     IDM_EDIT_FIND,
                     IDM_EDIT_FINDNEXTDOWN,
                     IDM_EDIT_FINDNEXTUP,
                     IDM_EDIT_REPLACE,
                     IDM_EDIT_GOTOLINE,
                     IDM_EDIT_INSERT_TAB_MENU,
                     IDM_EDIT_DELETE_TAB_MENU,
                     IDM_EDIT_INSERT_SPACE_MENU,
                     IDM_EDIT_DELETE_SPACE_MENU,
                     IDM_EDIT_DELETE_FIRST_CHAR_MENU,
                     IDM_EDIT_DELETE_TRAILING_WHITESPACES,
                     IDM_EDIT_UPPERCASE,
                     IDM_EDIT_LOWERCASE,
                     IDM_EDIT_SENTENCECASE,
                     IDM_EDIT_TITLECASE,
                     IDM_EDIT_INVERTCASE,
                     IDM_EDIT_NEWLINE_WIN,
                     IDM_EDIT_NEWLINE_UNIX,
                     IDM_EDIT_RECODE,
                     IDM_VIEW_FONT,
                     IDM_VIEW_TEXT_COLOR,
                     IDM_VIEW_BG_COLOR,
                     IDM_VIEW_INCREASE_FONT,
                     IDM_VIEW_DECREASE_FONT,
                     IDM_VIEW_ALIGN_LEFT,
                     IDM_VIEW_ALIGN_CENTER,
                     IDM_VIEW_ALIGN_RIGHT,
                     IDM_VIEW_WORDWRAP,
                     IDM_WINDOW_TILEHORIZONTAL,
                     IDM_WINDOW_TILEVERTICAL,
                     IDM_WINDOW_CASCADE,
                     0};

    if (!lParam)
    {
      hWndEdit=NULL;
      hWndFrameActive=NULL;

      for (i=0; nMessages[i]; ++i)
        EnableMenuItem(hMainMenu, nMessages[i], MF_GRAYED);
      bMdiNoWindows=TRUE;

      for (i=0; i < STATUS_PARTS; ++i)
        SendMessage(hStatus, SB_SETTEXTA, i, (LPARAM)"");
      szCurrentFile[0]='\0';
      bModified=FALSE;
      SendMessage(hMainWnd, AKDN_FRAME_NOWINDOWS, 0, 0);
    }
    else
    {
      if (bMdiNoWindows)
      {
        for (i=0; nMessages[i]; ++i)
          EnableMenuItem(hMainMenu, nMessages[i], MF_ENABLED);
        bMdiNoWindows=FALSE;

        SetNewLineA(hWndEdit, NEWLINE_WIN, TRUE);
        SetInsertStateA(NULL, FALSE, TRUE);
        SetModifyA(hWndEdit, FALSE, TRUE);
        SetCodePageA(nDefaultCodePage, bDefaultBOM, TRUE);
      }

      if ((HWND)wParam == hWndFrameActive)
      {
        //Save deactivated MDI window settings
        if (wParam && (HWND)wParam != hWndFrameDestroyed)
        {
          if (lpWndFrameA=(WNDFRAMEA *)GetWindowLongA((HWND)wParam, GWL_USERDATA))
          {
            lstrcpynA(lpWndFrameA->szFile, szCurrentFile, MAX_PATH);
            lpWndFrameA->nCodePage=nCurrentCodePage;
            lpWndFrameA->bBOM=bCurrentBOM;
            lpWndFrameA->nNewLine=nNewLine;
            lpWndFrameA->bModified=bModified;
            lpWndFrameA->bReadOnly=bReadOnly;
            lpWndFrameA->bWordWrap=bWordWrap;
            lpWndFrameA->bInsertState=bInsertState;
            lpWndFrameA->nTabStopSize=nTabStopSize;
            lpWndFrameA->nUndoLimit=nUndoLimit;
            lpWndFrameA->bShowURL=bShowURL;
            lpWndFrameA->dwEditMargins=dwEditMargins;
            lpWndFrameA->bDelimitersEnable=bDelimitersEnable;
            lpWndFrameA->ft=ftFileTime;
            memcpy(&lpWndFrameA->lf, &lfEditFontA, sizeof(LOGFONTA));
            lpWndFrameA->aec=aecColors;
          }
        }
        //Handles
        hWndEdit=GetDlgItem((HWND)lParam, ID_EDIT);
        hWndFrameActive=(HWND)lParam;
        hWndFrameDestroyed=NULL;

        //Set activated MDI window settings
        if (lpWndFrameA=(WNDFRAMEA *)GetWindowLongA((HWND)lParam, GWL_USERDATA))
        {
          lstrcpynA(szCurrentFile, lpWndFrameA->szFile, MAX_PATH);
          SetCodePageA(lpWndFrameA->nCodePage, lpWndFrameA->bBOM, FALSE);
          SetNewLineA(hWndEdit, lpWndFrameA->nNewLine, FALSE);
          SetModifyA(hWndEdit, lpWndFrameA->bModified, FALSE);
          bReadOnly=lpWndFrameA->bReadOnly;
          bWordWrap=lpWndFrameA->bWordWrap;
          SetInsertStateA(NULL, lpWndFrameA->bInsertState, FALSE);
          nTabStopSize=lpWndFrameA->nTabStopSize;
          nUndoLimit=lpWndFrameA->nUndoLimit;
          dwEditMargins=lpWndFrameA->dwEditMargins;
          bDelimitersEnable=lpWndFrameA->bDelimitersEnable;
          ftFileTime=lpWndFrameA->ft;
          memcpy(&lfEditFontA, &lpWndFrameA->lf, sizeof(LOGFONTA));
          aecColors=lpWndFrameA->aec;
        }

        //Update selection
        nmhdr.hwndFrom=hWndEdit;
        nmhdr.idFrom=ID_EDIT;
        nmhdr.code=EN_SELCHANGE;
        SendMessage((HWND)lParam, WM_NOTIFY, ID_EDIT, (LPARAM)&nmhdr);

        //Update tabs
        if (!bTabPressed)
        {
          if ((i=GetTabItemFromParam(hTab, lParam)) != -1)
          {
            SendMessage(hTab, TCM_SETCURSEL, i, 0);
            UpdateTabs(hTab);
          }
        }
      }
      else
      {
        SetFocus(hWndEdit);
      }
    }
  }

  //Special messages
  {
   LRESULT lResult;

   if (lResult=EditParentMessagesA(hWnd, uMsg, wParam, lParam)) return lResult;
  }

  return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK FrameProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CREATE)
  {
    if (lpWndFrameW=(WNDFRAMEW *)API_HeapAlloc(hHeap, 0, sizeof(WNDFRAMEW)))
    {
      TCITEMW tcItemW;
      int nIndex;
      int nItemSum;

      CreateEditWindowW(hWnd);

      lpWndFrameW->hIcon=hIconEmpty;
      lpWndFrameW->wszFile[0]='\0';
      lpWndFrameW->nCodePage=nDefaultCodePage;
      lpWndFrameW->bBOM=bDefaultBOM;
      lpWndFrameW->nNewLine=NEWLINE_WIN;
      lpWndFrameW->bModified=FALSE;
      lpWndFrameW->bReadOnly=bReadOnly;
      lpWndFrameW->bWordWrap=bWordWrap;
      lpWndFrameW->bInsertState=FALSE;
      lpWndFrameW->nTabStopSize=nTabStopSize;
      lpWndFrameW->nUndoLimit=nUndoLimit;
      lpWndFrameW->bShowURL=bShowURL;
      lpWndFrameW->dwEditMargins=dwEditMargins;
      lpWndFrameW->bDelimitersEnable=bDelimitersEnable;
      memset(&lpWndFrameW->ft, 0, sizeof(FILETIME));
      memcpy(&lpWndFrameW->lf, &lfEditFontW, sizeof(LOGFONTW));
      lpWndFrameW->aec=aecColors;
      SetWindowLongW(hWnd, GWL_USERDATA, (LONG)lpWndFrameW);

      nIndex=ImageList_AddIcon(hImageList, hIconEmpty);
      tcItemW.mask=TCIF_TEXT|TCIF_IMAGE|TCIF_PARAM;
      tcItemW.pszText=L"";
      tcItemW.iImage=nIndex;
      tcItemW.lParam=(LPARAM)hWnd;
      nItemSum=SendMessage(hTab, TCM_GETITEMCOUNT, 0, 0);
      SendMessage(hTab, TCM_INSERTITEMW, nItemSum, (LPARAM)&tcItemW);
      SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconEmpty);

      if (!hWndFrameActive) InvalidateRect(hTab, NULL, TRUE);
    }
  }
  else if (uMsg == WM_SIZE)
  {
    if (wParam != SIZE_MINIMIZED)
    {
      HWND hWndTmp;

      if (hWndTmp=GetDlgItem(hWnd, ID_EDIT))
        ResizeEdit(hWndTmp, 0, 0, LOWORD(lParam), HIWORD(lParam));
    }
  }
  else if (uMsg == WM_MDIACTIVATE)
  {
    NMHDR nmhdr;
    int i;
    int nMessages[]={IDM_FILE_REOPEN,
                     IDM_FILE_SAVE,
                     IDM_FILE_SAVEAS,
                     IDM_FILE_SAVEALL,
                     IDM_FILE_PAGESETUP,
                     IDM_FILE_PRINT,
                     IDM_EDIT_UNDO,
                     IDM_EDIT_REDO,
                     IDM_EDIT_CUT,
                     IDM_EDIT_COPY,
                     IDM_EDIT_PASTE,
                     IDM_EDIT_CLEAR,
                     IDM_EDIT_SELECTALL,
                     IDM_EDIT_FIND,
                     IDM_EDIT_FINDNEXTDOWN,
                     IDM_EDIT_FINDNEXTUP,
                     IDM_EDIT_REPLACE,
                     IDM_EDIT_GOTOLINE,
                     IDM_EDIT_INSERT_TAB_MENU,
                     IDM_EDIT_DELETE_TAB_MENU,
                     IDM_EDIT_INSERT_SPACE_MENU,
                     IDM_EDIT_DELETE_SPACE_MENU,
                     IDM_EDIT_DELETE_FIRST_CHAR_MENU,
                     IDM_EDIT_DELETE_TRAILING_WHITESPACES,
                     IDM_EDIT_UPPERCASE,
                     IDM_EDIT_LOWERCASE,
                     IDM_EDIT_SENTENCECASE,
                     IDM_EDIT_TITLECASE,
                     IDM_EDIT_INVERTCASE,
                     IDM_EDIT_NEWLINE_WIN,
                     IDM_EDIT_NEWLINE_UNIX,
                     IDM_EDIT_RECODE,
                     IDM_VIEW_FONT,
                     IDM_VIEW_TEXT_COLOR,
                     IDM_VIEW_BG_COLOR,
                     IDM_VIEW_INCREASE_FONT,
                     IDM_VIEW_DECREASE_FONT,
                     IDM_VIEW_ALIGN_LEFT,
                     IDM_VIEW_ALIGN_CENTER,
                     IDM_VIEW_ALIGN_RIGHT,
                     IDM_VIEW_WORDWRAP,
                     IDM_WINDOW_TILEHORIZONTAL,
                     IDM_WINDOW_TILEVERTICAL,
                     IDM_WINDOW_CASCADE,
                     0};

    if (!lParam)
    {
      hWndEdit=NULL;
      hWndFrameActive=NULL;

      for (i=0; nMessages[i]; ++i)
        EnableMenuItem(hMainMenu, nMessages[i], MF_GRAYED);
      bMdiNoWindows=TRUE;

      for (i=0; i < STATUS_PARTS; ++i)
        SendMessage(hStatus, SB_SETTEXTW, i, (LPARAM)L"");
      wszCurrentFile[0]='\0';
      bModified=FALSE;
      SendMessage(hMainWnd, AKDN_FRAME_NOWINDOWS, 0, 0);
    }
    else
    {
      if (bMdiNoWindows)
      {
        for (i=0; nMessages[i]; ++i)
          EnableMenuItem(hMainMenu, nMessages[i], MF_ENABLED);
        bMdiNoWindows=FALSE;

        SetNewLineW(hWndEdit, NEWLINE_WIN, TRUE);
        SetInsertStateW(NULL, FALSE, TRUE);
        SetModifyW(hWndEdit, FALSE, TRUE);
        SetCodePageW(nDefaultCodePage, bDefaultBOM, TRUE);
      }

      if ((HWND)wParam == hWndFrameActive)
      {
        //Save deactivated MDI window settings
        if (wParam && (HWND)wParam != hWndFrameDestroyed)
        {
          if (lpWndFrameW=(WNDFRAMEW *)GetWindowLongW((HWND)wParam, GWL_USERDATA))
          {
            lstrcpynW(lpWndFrameW->wszFile, wszCurrentFile, MAX_PATH);
            lpWndFrameW->nCodePage=nCurrentCodePage;
            lpWndFrameW->bBOM=bCurrentBOM;
            lpWndFrameW->nNewLine=nNewLine;
            lpWndFrameW->bModified=bModified;
            lpWndFrameW->bReadOnly=bReadOnly;
            lpWndFrameW->bWordWrap=bWordWrap;
            lpWndFrameW->bInsertState=bInsertState;
            lpWndFrameW->nTabStopSize=nTabStopSize;
            lpWndFrameW->nUndoLimit=nUndoLimit;
            lpWndFrameW->bShowURL=bShowURL;
            lpWndFrameW->dwEditMargins=dwEditMargins;
            lpWndFrameW->bDelimitersEnable=bDelimitersEnable;
            lpWndFrameW->ft=ftFileTime;
            memcpy(&lpWndFrameW->lf, &lfEditFontW, sizeof(LOGFONTW));
            lpWndFrameW->aec=aecColors;
          }
        }
        //Handles
        hWndEdit=GetDlgItem((HWND)lParam, ID_EDIT);
        hWndFrameActive=(HWND)lParam;
        hWndFrameDestroyed=NULL;

        //Set activated MDI window settings
        if (lpWndFrameW=(WNDFRAMEW *)GetWindowLongW((HWND)lParam, GWL_USERDATA))
        {
          lstrcpynW(wszCurrentFile, lpWndFrameW->wszFile, MAX_PATH);
          SetCodePageW(lpWndFrameW->nCodePage, lpWndFrameW->bBOM, FALSE);
          SetNewLineW(hWndEdit, lpWndFrameW->nNewLine, FALSE);
          SetModifyW(hWndEdit, lpWndFrameW->bModified, FALSE);
          bReadOnly=lpWndFrameW->bReadOnly;
          bWordWrap=lpWndFrameW->bWordWrap;
          SetInsertStateW(NULL, lpWndFrameW->bInsertState, FALSE);
          nTabStopSize=lpWndFrameW->nTabStopSize;
          nUndoLimit=lpWndFrameW->nUndoLimit;
          dwEditMargins=lpWndFrameW->dwEditMargins;
          bDelimitersEnable=lpWndFrameW->bDelimitersEnable;
          ftFileTime=lpWndFrameW->ft;
          memcpy(&lfEditFontW, &lpWndFrameW->lf, sizeof(LOGFONTW));
          aecColors=lpWndFrameW->aec;
        }

        //Update selection
        nmhdr.hwndFrom=hWndEdit;
        nmhdr.idFrom=ID_EDIT;
        nmhdr.code=EN_SELCHANGE;
        SendMessage((HWND)lParam, WM_NOTIFY, ID_EDIT, (LPARAM)&nmhdr);

        //Update tabs
        if (!bTabPressed)
        {
          if ((i=GetTabItemFromParam(hTab, lParam)) != -1)
          {
            SendMessage(hTab, TCM_SETCURSEL, i, 0);
            UpdateTabs(hTab);
          }
        }
      }
      else
      {
        SetFocus(hWndEdit);
      }
    }
  }

  //Special messages
  {
   LRESULT lResult;

   if (lResult=EditParentMessagesW(hWnd, uMsg, wParam, lParam)) return lResult;
  }

  return DefMDIChildProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CommonEditProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult;

  lResult=lpfnEditProcA(hWnd, uMsg, wParam, lParam);

  if (lpfnEditProcRetA)
  {
    CWPRETSTRUCT cwprs;

    cwprs.lResult=lResult;
    cwprs.lParam=lParam;
    cwprs.wParam=wParam;
    cwprs.message=uMsg;
    cwprs.hwnd=hWnd;
    lpfnEditProcRetA(&cwprs);
  }
  return lResult;
}

LRESULT CALLBACK CommonEditProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult;

  lResult=lpfnEditProcW(hWnd, uMsg, wParam, lParam);

  if (lpfnEditProcRetW)
  {
    CWPRETSTRUCT cwprs;

    cwprs.lResult=lResult;
    cwprs.lParam=lParam;
    cwprs.wParam=wParam;
    cwprs.message=uMsg;
    cwprs.hwnd=hWnd;
    lpfnEditProcRetW(&cwprs);
  }
  return lResult;
}

LRESULT CALLBACK EditProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_SETFOCUS)
  {
    if (bMDI && hWnd != hWndEdit)
      SendMessage(hMdiClient, WM_MDIACTIVATE, (WPARAM)GetParent(hWnd), 0);
  }
  else if (uMsg == WM_KEYDOWN)
  {
    if (wParam == VK_RETURN)
    {
      if (!bReadOnly)
      {
        if (bKeepSpace)
        {
          AutoIndentA(hWnd, &chrg);
          return TRUE;
        }
      }
    }
    else if (wParam == VK_INSERT)
    {
      if (!bInsertStateChange) return TRUE;
      bInsertStateChange=FALSE;
    }
    else if (wParam == VK_DELETE)
    {
      if (chrg.cpMin == chrg.cpMax)
      {
        TEXTRANGEA txtrngA;
        char szChar[4];

        txtrngA.chrg.cpMin=chrg.cpMin;
        txtrngA.chrg.cpMax=chrg.cpMin + 1;
        txtrngA.lpstrText=szChar;
        SendMessageA(hWnd, EM_GETTEXTRANGE, 0, (LPARAM)&txtrngA);
        if (*szChar == '\0') MessageBeep(MB_OK);
      }
    }
  }
  else if (uMsg == WM_MOUSEWHEEL)
  {
    if (LOWORD(wParam) == MK_CONTROL)
    {
      if ((short)HIWORD(wParam) < 0)
        DoViewFontSizeA(hWndEdit, DECREASE_FONT);
      else
        DoViewFontSizeA(hWndEdit, INCREASE_FONT);
    }
    else
    {
      DWORD dwLines;
      DWORD dwCount;

      SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &dwLines, 0);

      if ((short)HIWORD(wParam) < 0)
      {
        if ((int)dwLines == -1)
        {
          SendMessage(hWndEdit, EM_SCROLL, SB_PAGEDOWN, 0);
        }
        else
        {
          SCROLLINFO si;

          si.cbSize=sizeof(SCROLLINFO);
          si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;

          for (dwCount=0; dwCount < dwLines; ++dwCount)
          {
            GetScrollInfo(hWndEdit, SB_VERT, &si);
            if (si.nPage > 0 && si.nPos + (int)si.nPage < si.nMax)
              SendMessage(hWndEdit, EM_SCROLL, SB_LINEDOWN, 0);
            else
              break;
          }
        }
      }
      else
      {
        if ((int)dwLines == -1)
        {
          SendMessage(hWndEdit, EM_SCROLL, SB_PAGEUP, 0);
        }
        else
        {
          for (dwCount=0; dwCount < dwLines; ++dwCount)
          {
            if (LOWORD(SendMessage(hWndEdit, EM_SCROLL, SB_LINEUP, 0)) == 0)
              break;
          }
        }
      }
    }
    return TRUE;
  }
  else if (uMsg == WM_CONTEXTMENU)
  {
    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
  }
  else if (uMsg == WM_RBUTTONUP)
  {
    NCONTEXTMENU ncm;

    CallWindowProcA(OldEditProc, hWnd, uMsg, wParam, lParam);

    GetCursorPos(&ncm.pt);
    ncm.hWnd=hWnd;
    ncm.uType=NCM_EDIT;
    ncm.bProcess=TRUE;
    SendMessage(hMainWnd, AKDN_CONTEXTMENU, 0, (LPARAM)&ncm);
    if (ncm.bProcess) DefWindowProcA(hWnd, uMsg, wParam, lParam);
    return 0;
  }

  return CallWindowProcA(OldEditProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK EditProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_SETFOCUS)
  {
    if (bMDI && hWnd != hWndEdit)
      SendMessage(hMdiClient, WM_MDIACTIVATE, (WPARAM)GetParent(hWnd), 0);
  }
  else if (uMsg == WM_KEYDOWN)
  {
    if (wParam == VK_RETURN)
    {
      if (!bReadOnly)
      {
        if (bKeepSpace)
        {
          AutoIndentW(hWnd, &chrg);
          return TRUE;
        }
      }
    }
    else if (wParam == VK_INSERT)
    {
      if (!bInsertStateChange) return TRUE;
      bInsertStateChange=FALSE;
    }
    else if (wParam == VK_DELETE)
    {
      if (chrg.cpMin == chrg.cpMax)
      {
        TEXTRANGEW txtrngW;
        wchar_t wszChar[4];

        txtrngW.chrg.cpMin=chrg.cpMin;
        txtrngW.chrg.cpMax=chrg.cpMin + 1;
        txtrngW.lpstrText=wszChar;
        SendMessageW(hWnd, EM_GETTEXTRANGE, 0, (LPARAM)&txtrngW);
        if (*wszChar == '\0') MessageBeep(MB_OK);
      }
    }
  }
  else if (uMsg == WM_MOUSEWHEEL)
  {
    if (LOWORD(wParam) == MK_CONTROL)
    {
      if ((short)HIWORD(wParam) < 0)
        DoViewFontSizeW(hWndEdit, DECREASE_FONT);
      else
        DoViewFontSizeW(hWndEdit, INCREASE_FONT);
    }
    else
    {
      DWORD dwLines;
      DWORD dwCount;

      SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &dwLines, 0);

      if ((short)HIWORD(wParam) < 0)
      {
        if ((int)dwLines == -1)
        {
          SendMessage(hWndEdit, EM_SCROLL, SB_PAGEDOWN, 0);
        }
        else
        {
          SCROLLINFO si;

          si.cbSize=sizeof(SCROLLINFO);
          si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;

          for (dwCount=0; dwCount < dwLines; ++dwCount)
          {
            GetScrollInfo(hWndEdit, SB_VERT, &si);
            if (si.nPage > 0 && si.nPos + (int)si.nPage < si.nMax)
              SendMessage(hWndEdit, EM_SCROLL, SB_LINEDOWN, 0);
            else
              break;
          }
        }
      }
      else
      {
        if ((int)dwLines == -1)
        {
          SendMessage(hWndEdit, EM_SCROLL, SB_PAGEUP, 0);
        }
        else
        {
          for (dwCount=0; dwCount < dwLines; ++dwCount)
          {
            if (LOWORD(SendMessage(hWndEdit, EM_SCROLL, SB_LINEUP, 0)) == 0)
              break;
          }
        }
      }
    }
    return TRUE;
  }
  else if (uMsg == WM_CONTEXTMENU)
  {
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
  }
  else if (uMsg == WM_RBUTTONUP)
  {
    NCONTEXTMENU ncm;

    CallWindowProcW(OldEditProc, hWnd, uMsg, wParam, lParam);

    GetCursorPos(&ncm.pt);
    ncm.hWnd=hWnd;
    ncm.uType=NCM_EDIT;
    ncm.bProcess=TRUE;
    SendMessage(hMainWnd, AKDN_CONTEXTMENU, 0, (LPARAM)&ncm);
    if (ncm.bProcess) DefWindowProcW(hWnd, uMsg, wParam, lParam);
    return 0;
  }

  return CallWindowProcW(OldEditProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK NewMdiClientProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_DROPFILES)
  {
    int nDropped;
    int i;

    nDropped=DragQueryFileA((HDROP)wParam, 0xFFFFFFFF, szFileBuffer, FILELIST_BUFFER_SIZE);

    for (i=0; i < nDropped; ++i)
    {
      DragQueryFileA((HDROP)wParam, i, szFileBuffer, FILELIST_BUFFER_SIZE);
      if (IsFileA(szFileBuffer) == ERROR_FILE_NOT_FOUND)
        OpenDirectoryA(szFileBuffer, TRUE);
      else
        OpenDocumentA(hWndEdit, szFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, FALSE);
    }
    DragFinish((HDROP)wParam);
    return TRUE;
  }
  else if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_MBUTTONDOWN)
  {
    if (hWndFrameActive) SendMessage(hMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMdiMaximize);
    CreateMDIWindowA(APP_MDI_CLASSA, "", (bMdiMaximize == TRUE)?WS_MAXIMIZE:0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hMdiClient, hInstance, 0);
  }
  else if (uMsg == WM_MDINEXT)
  {
    int nItem;
    int nItemCount;

    if (nTabSwitch == TAB_SWITCH_RIGHTLEFT)
    {
      nItem=SendMessage(hTab, TCM_GETCURSEL, 0, 0);
      nItemCount=SendMessage(hTab, TCM_GETITEMCOUNT, 0, 0) - 1;

      if (!lParam)
      {
        if (nItem + 1 > nItemCount)
          SelectTabItem(hTab, 0);
        else
          SelectTabItem(hTab, nItem + 1);
      }
      else
      {
        if (nItem - 1 < 0)
          SelectTabItem(hTab, nItemCount);
        else
          SelectTabItem(hTab, nItem - 1);
      }
      return TRUE;
    }
  }
  else if (uMsg == WM_MDIACTIVATE)
  {
    //Remove blinking frame windows effect (from Mark editor)
    if (hWndFrameActive)
    {
      SendMessage(hWnd, WM_MDIGETACTIVE, 0, (LPARAM)&bMdiMaximize);

      if (bMdiMaximize)
      {
        LRESULT lResult;

        SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
        lResult=CallWindowProcA(OldMdiClientProc, hWnd, uMsg, wParam, lParam);
        SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
        InvalidateRect((HWND)wParam, NULL, TRUE);
        return lResult;
      }
    }
  }
  else if (uMsg == WM_MDIDESTROY)
  {
    WNDFRAMEA *wf;
    HWND hWndEditDestroyed;
    int nItem;

    if ((HWND)wParam)
    {
      if (!(bFileExitError=DoFileExitA())) return TRUE;

      if (!bMdiClientRedraw)
      {
        if (SendMessage(hTab, TCM_GETITEMCOUNT, 0, 0) == 1)
          SendMessage(hMdiClient, WM_SETREDRAW, FALSE, 0);
      }
      hWndFrameDestroyed=(HWND)wParam;
      hWndEditDestroyed=GetDlgItem((HWND)wParam, ID_EDIT);
      SendMessage(hMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMdiMaximize);
      SendMessage(hMainWnd, AKDN_EDIT_ONFINISH, (WPARAM)hWndEditDestroyed, 0);
      DestroyWindow(hWndEditDestroyed);
      if (hWndEditDestroyed == hWndEdit) hWndEdit=NULL;

      nItem=SendMessage(hTab, TCM_GETCURSEL, 0, 0);
      DeleteTabItem(hTab, nItem);

      if (wf=(WNDFRAMEA *)GetWindowLongA((HWND)wParam, GWL_USERDATA))
        if (wf->hIcon != hIconEmpty) DestroyIcon(wf->hIcon);
      API_HeapFree(hHeap, 0, (LPVOID)wf);
    }
  }

  return CallWindowProcA(OldMdiClientProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK NewMdiClientProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_DROPFILES)
  {
    int nDropped;
    int i;

    nDropped=DragQueryFileW((HDROP)wParam, 0xFFFFFFFF, wszFileBuffer, FILELIST_BUFFER_SIZE);

    for (i=0; i < nDropped; ++i)
    {
      DragQueryFileW((HDROP)wParam, i, wszFileBuffer, FILELIST_BUFFER_SIZE);
      if (IsFileW(wszFileBuffer) == ERROR_FILE_NOT_FOUND)
        OpenDirectoryW(wszFileBuffer, TRUE);
      else
        OpenDocumentW(hWndEdit, wszFileBuffer, OD_ADT_BINARY_ERROR|OD_ADT_REG_CODEPAGE, 0, FALSE);
    }
    DragFinish((HDROP)wParam);
    return TRUE;
  }
  else if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_MBUTTONDOWN)
  {
    if (hWndFrameActive) SendMessage(hMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMdiMaximize);
    CreateMDIWindowW(APP_MDI_CLASSW, L"", (bMdiMaximize == TRUE)?WS_MAXIMIZE:0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hMdiClient, hInstance, 0);
  }
  else if (uMsg == WM_MDINEXT)
  {
    int nItem;
    int nItemCount;

    if (nTabSwitch == TAB_SWITCH_RIGHTLEFT)
    {
      nItem=SendMessage(hTab, TCM_GETCURSEL, 0, 0);
      nItemCount=SendMessage(hTab, TCM_GETITEMCOUNT, 0, 0) - 1;

      if (!lParam)
      {
        if (nItem + 1 > nItemCount)
          SelectTabItem(hTab, 0);
        else
          SelectTabItem(hTab, nItem + 1);
      }
      else
      {
        if (nItem - 1 < 0)
          SelectTabItem(hTab, nItemCount);
        else
          SelectTabItem(hTab, nItem - 1);
      }
      return TRUE;
    }
  }
  else if (uMsg == WM_MDIACTIVATE)
  {
    //Remove blinking frame windows effect (from Mark editor)
    if (hWndFrameActive)
    {
      SendMessage(hWnd, WM_MDIGETACTIVE, 0, (LPARAM)&bMdiMaximize);

      if (bMdiMaximize)
      {
        LRESULT lResult;

        SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
        lResult=CallWindowProcW(OldMdiClientProc, hWnd, uMsg, wParam, lParam);
        SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
        InvalidateRect((HWND)wParam, NULL, TRUE);
        return lResult;
      }
    }
  }
  else if (uMsg == WM_MDIDESTROY)
  {
    WNDFRAMEW *wf;
    HWND hWndEditDestroyed;
    int nItem;

    if ((HWND)wParam)
    {
      if (!(bFileExitError=DoFileExitW())) return TRUE;

      if (!bMdiClientRedraw)
      {
        if (SendMessage(hTab, TCM_GETITEMCOUNT, 0, 0) == 1)
          SendMessage(hMdiClient, WM_SETREDRAW, FALSE, 0);
      }
      hWndFrameDestroyed=(HWND)wParam;
      hWndEditDestroyed=GetDlgItem((HWND)wParam, ID_EDIT);
      SendMessage(hMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMdiMaximize);
      SendMessage(hMainWnd, AKDN_EDIT_ONFINISH, (WPARAM)hWndEditDestroyed, 0);
      DestroyWindow(hWndEditDestroyed);
      if (hWndEditDestroyed == hWndEdit) hWndEdit=NULL;

      nItem=SendMessage(hTab, TCM_GETCURSEL, 0, 0);
      DeleteTabItem(hTab, nItem);

      if (wf=(WNDFRAMEW *)GetWindowLongW((HWND)wParam, GWL_USERDATA))
        if (wf->hIcon != hIconEmpty) DestroyIcon(wf->hIcon);
      API_HeapFree(hHeap, 0, (LPVOID)wf);
    }
  }

  return CallWindowProcW(OldMdiClientProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK NewTabProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static int nDragItem;
  static int nDropItem;
  static int nMouseMove;
  static BOOL bMouseDown=FALSE;

  if (uMsg == WM_CONTEXTMENU)
  {
    if (lParam != -1)
    {
      NCONTEXTMENU ncm;
      TCHITTESTINFO thti;
      POINT pt;
      HMENU hMdiChildMenu;
      int nItem;
      int i;

      pt.x=thti.pt.x=LOWORD(lParam);
      pt.y=thti.pt.y=HIWORD(lParam);
      ScreenToClient(hWnd, &thti.pt);
      nItem=SendMessage(hWnd, TCM_HITTEST, 0, (LPARAM)&thti);

      if (nItem != -1)
      {
        SelectTabItem(hWnd, nItem);

        ncm.hWnd=hWnd;
        ncm.uType=NCM_TAB;
        ncm.pt=pt;
        ncm.bProcess=TRUE;
        SendMessage(hMainWnd, AKDN_CONTEXTMENU, 0, (LPARAM)&ncm);
        if (ncm.bProcess)
        {
          hMdiChildMenu=GetSystemMenu(hWndFrameActive, FALSE);
          if (i=TrackPopupMenu(hMdiChildMenu, TPM_NONOTIFY|TPM_RETURNCMD|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWndFrameActive, NULL))
            PostMessage(hWndFrameActive, WM_SYSCOMMAND, i, 0);
        }
        return TRUE;
      }
    }
    return TRUE;
  }
  else if (uMsg == WM_MBUTTONDOWN)
  {
    TCHITTESTINFO thti;
    int nItem;

    thti.pt.x=LOWORD(lParam);
    thti.pt.y=HIWORD(lParam);
    nItem=SendMessage(hWnd, TCM_HITTEST, 0, (LPARAM)&thti);

    if (nItem != -1)
    {
      SelectTabItem(hWnd, nItem);
      PostMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
      return TRUE;
    }
  }
  else if (uMsg == WM_LBUTTONDBLCLK)
  {
    PostMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
    return TRUE;
  }

  //Drag'n'Drop
  if (uMsg == WM_LBUTTONDOWN)
  {
    if (!bMouseDown)
    {
      TCHITTESTINFO thti;

      thti.pt.x=LOWORD(lParam);
      thti.pt.y=HIWORD(lParam);
      nDragItem=SendMessage(hWnd, TCM_HITTEST, 0, (LPARAM)&thti);

      if (nDragItem != -1)
      {
        SelectTabItem(hWnd, nDragItem);
        bMouseDown=TRUE;
        nMouseMove=4;
        SetCapture(hWnd);
        return TRUE;
      }
    }
  }
  else if (uMsg == WM_MOUSEMOVE)
  {
    if (bMouseDown)
    {
      if (nMouseMove > 0)
      {
        if (--nMouseMove == 0)
        {
          SetCursor(hCursorDragMove);
        }
      }
      return TRUE;
    }
  }
  else if (uMsg == WM_LBUTTONUP)
  {
    if (bMouseDown)
    {
      TCITEMA tcItemA;
      POINT pt;

      bMouseDown=FALSE;
      ReleaseCapture();

      GetCursorPos(&pt);
      nDropItem=GetTabItemFromPoint(hWnd, &pt);

      if (nDropItem != -1 && nDropItem != nDragItem)
      {
        tcItemA.mask=TCIF_TEXT|TCIF_IMAGE|TCIF_PARAM;
        tcItemA.pszText=buf;
        tcItemA.cchTextMax=BUFFER_SIZE;
        SendMessage(hWnd, TCM_GETITEMA, nDragItem, (LPARAM)&tcItemA);
        SendMessage(hWnd, TCM_DELETEITEM, nDragItem, 0);
        SendMessage(hWnd, TCM_INSERTITEMA, nDropItem, (LPARAM)&tcItemA);
      }
      return TRUE;
    }
  }
  else if (uMsg == WM_CAPTURECHANGED)
  {
    if (bMouseDown)
    {
      bMouseDown=FALSE;
      ReleaseCapture();
    }
  }

  return CallWindowProcA(OldTabProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK NewTabProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static int nDragItem;
  static int nDropItem;
  static int nMouseMove;
  static BOOL bMouseDown=FALSE;

  if (uMsg == WM_CONTEXTMENU)
  {
    if (lParam != -1)
    {
      NCONTEXTMENU ncm;
      TCHITTESTINFO thti;
      POINT pt;
      HMENU hMdiChildMenu;
      int nItem;
      int i;

      pt.x=thti.pt.x=LOWORD(lParam);
      pt.y=thti.pt.y=HIWORD(lParam);
      ScreenToClient(hWnd, &thti.pt);
      nItem=SendMessage(hWnd, TCM_HITTEST, 0, (LPARAM)&thti);

      if (nItem != -1)
      {
        SelectTabItem(hWnd, nItem);

        ncm.hWnd=hWnd;
        ncm.uType=NCM_TAB;
        ncm.pt=pt;
        ncm.bProcess=TRUE;
        SendMessage(hMainWnd, AKDN_CONTEXTMENU, 0, (LPARAM)&ncm);
        if (ncm.bProcess)
        {
          hMdiChildMenu=GetSystemMenu(hWndFrameActive, FALSE);
          if (i=TrackPopupMenu(hMdiChildMenu, TPM_NONOTIFY|TPM_RETURNCMD|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWndFrameActive, NULL))
            PostMessage(hWndFrameActive, WM_SYSCOMMAND, i, 0);
        }
        return TRUE;
      }
    }
    return TRUE;
  }
  else if (uMsg == WM_MBUTTONDOWN)
  {
    TCHITTESTINFO thti;
    int nItem;

    thti.pt.x=LOWORD(lParam);
    thti.pt.y=HIWORD(lParam);
    nItem=SendMessage(hWnd, TCM_HITTEST, 0, (LPARAM)&thti);

    if (nItem != -1)
    {
      SelectTabItem(hWnd, nItem);
      PostMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
      return TRUE;
    }
  }
  else if (uMsg == WM_LBUTTONDBLCLK)
  {
    PostMessage(hMdiClient, WM_MDIDESTROY, (WPARAM)hWndFrameActive, 0);
    return TRUE;
  }

  //Drag'n'Drop
  if (uMsg == WM_LBUTTONDOWN)
  {
    if (!bMouseDown)
    {
      TCHITTESTINFO thti;

      thti.pt.x=LOWORD(lParam);
      thti.pt.y=HIWORD(lParam);
      nDragItem=SendMessage(hWnd, TCM_HITTEST, 0, (LPARAM)&thti);

      if (nDragItem != -1)
      {
        SelectTabItem(hWnd, nDragItem);
        bMouseDown=TRUE;
        nMouseMove=4;
        SetCapture(hWnd);
        return TRUE;
      }
    }
  }
  else if (uMsg == WM_MOUSEMOVE)
  {
    if (bMouseDown)
    {
      if (nMouseMove > 0)
      {
        if (--nMouseMove == 0)
        {
          SetCursor(hCursorDragMove);
        }
      }
      return TRUE;
    }
  }
  else if (uMsg == WM_LBUTTONUP)
  {
    if (bMouseDown)
    {
      TCITEMW tcItemW;
      POINT pt;

      bMouseDown=FALSE;
      ReleaseCapture();

      GetCursorPos(&pt);
      nDropItem=GetTabItemFromPoint(hWnd, &pt);

      if (nDropItem != -1 && nDropItem != nDragItem)
      {
        tcItemW.mask=TCIF_TEXT|TCIF_IMAGE|TCIF_PARAM;
        tcItemW.pszText=wbuf;
        tcItemW.cchTextMax=BUFFER_SIZE;
        SendMessage(hWnd, TCM_GETITEMW, nDragItem, (LPARAM)&tcItemW);
        SendMessage(hWnd, TCM_DELETEITEM, nDragItem, 0);
        SendMessage(hWnd, TCM_INSERTITEMW, nDropItem, (LPARAM)&tcItemW);
      }
      return TRUE;
    }
  }
  else if (uMsg == WM_CAPTURECHANGED)
  {
    if (bMouseDown)
    {
      bMouseDown=FALSE;
      ReleaseCapture();
    }
  }

  return CallWindowProcW(OldTabProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DockProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult;

  if (lResult=DockMessages(hWnd, uMsg, wParam, lParam)) return lResult;

  return CallWindowProcA(OldDockProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DockProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult;

  if (lResult=DockMessages(hWnd, uMsg, wParam, lParam)) return lResult;

  return CallWindowProcW(OldDockProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DockMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static MINMAXINFO mmi;
  static RECT rcEdge;
  static RECT rcDrop;
  static DOCK *dkDragSource;
  static DOCK *dkDropTarget;
  static int nMouseEdge;
  static int nMouseDown=0;
  static int nMouseMove;
  static int nDropSide;
  DOCK *dkData;
  POINT pt;
  RECT rc;

  if (uMsg == WM_LBUTTONDOWN ||
      uMsg == WM_MOUSEMOVE ||
      uMsg == WM_LBUTTONUP ||
      uMsg == WM_CAPTURECHANGED)
  {
    if (dkData=StackDockFindWindow(&hDocksStack, hWnd, FALSE))
    {
      if (uMsg == WM_LBUTTONDOWN)
      {
        if (!nMouseDown)
        {
          GetCursorPos(&pt);

          if ((nMouseEdge=GetMouseEdge(hWnd, &pt)) == GetOppEdge(dkData->nSide))
          {
            if (!(dkData->dwFlags & DKF_FIXEDSIZE))
            {
              memset(&mmi, 0, sizeof(MINMAXINFO));
              mmi.ptMinTrackSize.x=0;
              mmi.ptMinTrackSize.y=0;
              mmi.ptMaxTrackSize.x=0x0FFFFFFF;
              mmi.ptMaxTrackSize.y=0x0FFFFFFF;
              SendMessage(hMainWnd, AKDN_DOCK_GETMINMAXINFO, (WPARAM)dkData, (LPARAM)&mmi);

              SetEdgeCursor(nMouseEdge);
              GetMovingRect(dkData, &pt, &mmi, &rcEdge);
              DrawMovingRect(&rcEdge);

              nMouseDown=DOCK_SIZING;
              SetCapture(hWnd);
            }
          }
          else
          {
            if (dkData->dwFlags & DKF_DRAGDROP)
            {
              rc=dkData->rcDragDrop;

              if (ClientToScreenRect(dkData->hWnd, &rc))
              {
                if (PtInRect(&rc, pt))
                {
                  nDropSide=0;
                  dkDragSource=dkData;
                  dkDropTarget=dkData;

                  nMouseDown=DOCK_DRAGDROP;
                  nMouseMove=4;
                  SetCapture(hWnd);
                }
              }
            }
          }
        }
      }
      else if (uMsg == WM_MOUSEMOVE)
      {
        GetCursorPos(&pt);

        if (nMouseDown)
        {
          if (nMouseDown == DOCK_SIZING)
          {
            DrawMovingRect(&rcEdge);
            GetMovingRect(dkData, &pt, &mmi, &rcEdge);
            DrawMovingRect(&rcEdge);
          }
          else if (nMouseDown == DOCK_DRAGDROP)
          {
            if (nMouseMove > 0)
            {
              if (--nMouseMove == 0)
              {
                GetWindowRect(dkDropTarget->hWnd, &rcDrop);
                DrawMovingRect(&rcDrop);
                SetEdgeCursor(0);
              }
            }
            if (nMouseMove == 0)
            {
              DOCK *dkPoint;
              int nSide=0;

              if (dkPoint=StackDockFromPoint(&hDocksStack, &pt))
              {
                if (dkPoint != dkDropTarget)
                {
                  if ((dkPoint->nSide == DKS_LEFT && !(dkDragSource->dwFlags & DKF_NODROPLEFT)) ||
                      (dkPoint->nSide == DKS_TOP && !(dkDragSource->dwFlags & DKF_NODROPTOP)) ||
                      (dkPoint->nSide == DKS_RIGHT && !(dkDragSource->dwFlags & DKF_NODROPRIGHT)) ||
                      (dkPoint->nSide == DKS_BOTTOM && !(dkDragSource->dwFlags & DKF_NODROPBOTTOM)))
                  {
                    nDropSide=0;
                    dkDropTarget=dkPoint;
                    DrawMovingRect(&rcDrop);
                    GetWindowRect(dkDropTarget->hWnd, &rcDrop);
                    DrawMovingRect(&rcDrop);
                  }
                }
              }
              else
              {
                rc=nsSize.rcCurrent;

                if (ClientToScreenRect(hMainWnd, &rc))
                {
                  if (PtInRect(&rc, pt))
                  {
                    if (pt.x >= rc.left && pt.x <= rc.left + DOCK_MAINMIN_X)
                    {
                      rc.right=rc.left + dkDragSource->rcSize.right;
                      nSide=DKS_LEFT;
                    }
                    else if (pt.y >= rc.top && pt.y <= rc.top + DOCK_MAINMIN_Y)
                    {
                      rc.bottom=rc.top + dkDragSource->rcSize.bottom;
                      nSide=DKS_TOP;
                    }
                    else if (pt.x <= rc.right && pt.x >= rc.right - DOCK_MAINMIN_X)
                    {
                      rc.left=rc.right - dkDragSource->rcSize.right;
                      nSide=DKS_RIGHT;
                    }
                    else if (pt.y <= rc.bottom && pt.y >= rc.bottom - DOCK_MAINMIN_Y)
                    {
                      rc.top=rc.bottom - dkDragSource->rcSize.bottom;
                      nSide=DKS_BOTTOM;
                    }

                    if (nSide && nSide != nDropSide && nSide != dkDragSource->nSide)
                    {
                      if ((nSide == DKS_LEFT && !(dkDragSource->dwFlags & DKF_NODROPLEFT)) ||
                          (nSide == DKS_TOP && !(dkDragSource->dwFlags & DKF_NODROPTOP)) ||
                          (nSide == DKS_RIGHT && !(dkDragSource->dwFlags & DKF_NODROPRIGHT)) ||
                          (nSide == DKS_BOTTOM && !(dkDragSource->dwFlags & DKF_NODROPBOTTOM)))
                      {
                        nDropSide=nSide;
                        dkDropTarget=NULL;
                        DrawMovingRect(&rcDrop);
                        rcDrop=rc;
                        DrawMovingRect(&rcDrop);
                      }
                    }
                  }
                }
              }
            }
          }
        }
        else
        {
          if (!(dkData->dwFlags & DKF_FIXEDSIZE))
          {
            if ((nMouseEdge=GetMouseEdge(hWnd, &pt)) == GetOppEdge(dkData->nSide))
            {
              SetEdgeCursor(nMouseEdge);
            }
          }
        }
      }
      else if (uMsg == WM_LBUTTONUP)
      {
        if (nMouseDown == DOCK_SIZING)
        {
          nMouseDown=0;
          DrawMovingRect(&rcEdge);
          ReleaseCapture();

          if (ScreenToClientRect(hMainWnd, &rcEdge))
          {
            if (dkData->nSide == DKS_LEFT ||
                dkData->nSide == DKS_RIGHT)
            {
              dkData->rcSize.right=rcEdge.right;
            }
            else if (dkData->nSide == DKS_TOP ||
                     dkData->nSide == DKS_BOTTOM)
            {
              dkData->rcSize.bottom=rcEdge.bottom;
            }
            hDocksStack.nSizingSide=dkData->nSide;
            UpdateSize();
          }
        }
        else if (nMouseDown == DOCK_DRAGDROP)
        {
          if (nMouseMove == 0)
            DrawMovingRect(&rcDrop);
          nMouseDown=0;
          ReleaseCapture();

          if (dkDropTarget != dkDragSource)
          {
            if (dkDropTarget)
              nDropSide=dkDropTarget->nSide;
            rc=rcDrop;

            if (ScreenToClientRect(hMainWnd, &rc))
            {
              if (nDropSide == DKS_LEFT ||
                  nDropSide == DKS_RIGHT)
              {
                dkDragSource->rcSize.left=rc.left;
              }
              else if (nDropSide == DKS_TOP ||
                       nDropSide == DKS_BOTTOM)
              {
                dkDragSource->rcSize.top=rc.top;
              }
              DockSetSide(&hDocksStack, dkDragSource, nDropSide);
              hDocksStack.nSizingSide=dkDragSource->nSide;
              UpdateSize();
            }
          }
        }
      }
      else if (uMsg == WM_CAPTURECHANGED)
      {
        if (nMouseDown)
        {
          if (nMouseDown == DOCK_SIZING)
          {
            nMouseDown=0;
            DrawMovingRect(&rcEdge);
            ReleaseCapture();
          }
          else if (nMouseDown == DOCK_DRAGDROP)
          {
            if (nMouseMove == 0)
              DrawMovingRect(&rcDrop);
            nMouseDown=0;
            ReleaseCapture();
          }
        }
      }
    }
  }
  return 0;
}

LRESULT CALLBACK DummyProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DummyProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
