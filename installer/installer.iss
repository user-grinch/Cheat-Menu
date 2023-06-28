; Based on Seeman's CLEORedux Installer 
; https://github.com/cleolibrary/CLEO-Redux

#define AppName "Cheat Menu"
#define AppVersion "3.52"
#define AppPublisher "Grinch_"
#define AppURL "https://github.com/user-grinch/Cheat-Menu"
#define OutputDir "."

#define CheatMenu = "https://github.com/user-grinch/Cheat-Menu/releases/download/"
#define UAL32 = "https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/Win32-latest"
#define D3D8to9Wrapper = "https://github.com/user-grinch/ImGuiRedux/releases/download/Win64-latest/d3d8.zip"
#define SilentPatchIII = "https://silent.rockstarvision.com/uploads/SilentPatchIII.zip"
#define SilentPatchVC = "https://silent.rockstarvision.com/uploads/SilentPatchVC.zip"
#define SilentPatchSA = "https://silent.rockstarvision.com/uploads/SilentPatchSA.zip"
#define Redist = "https://aka.ms/vs/17/release/vc_redist.x86.exe"
#define DX9 = "https://download.microsoft.com/download/1/7/1/1718CCC4-6315-4D8E-9543-8E28A4E18C4C/dxwebsetup.exe"

[Setup]
AppId={{511AFCDA-FD5E-491C-A1B7-22BAC8F93711}}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion} by {#AppPublisher}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppCopyright=Copyright (c) 2019-2023, {#AppPublisher}
DefaultDirName=New folder
LicenseFile=../LICENSE
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
;PrivilegesRequiredOverridesAllowed=dialog
OutputDir={#OutputDir}
OutputBaseFilename=Installer
;SetupIconFile=cr4.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
AppendDefaultDirName=false
DirExistsWarning=false
EnableDirDoesntExistWarning=true
UsePreviousAppDir=no
Uninstallable=false

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Types]
Name: "full"; Description: "Full"; Flags: iscustom

[Components]
Name: "program"; Description: "Cheat Menu"; Types: full; Flags: fixed
Name: "plugins"; Description: "Prerequisites"; Types: full
Name: "plugins/d3d8to9"; Description: "d3d8to9 Wrapper - needed for III & VC"; Types: full; Flags: fixed
Name: "plugins/SilentPatch"; Description: "SilentPatch - needed for the mouse to work properly"; Types: full
Name: "plugins/asiloader"; Description: "Ultimate ASI Loader (by ThirteenAG)"; Types: full
Name: "plugins/redist"; Description: "Visual C++ Redistributable 2022 x86"; Types: full
Name: "plugins/dx9"; Description: "DirectX End-User Runtime Version 9.0"; Types: full

[Dirs]
Name: "{app}"; Permissions: users-modify
Name: "{app}\CheatMenuSA"; Check: IsSA; Permissions: users-modify
Name: "{app}\CheatMenuVC"; Check: IsVC; Permissions: users-modify
Name: "{app}\CheatMenuIII"; Check: IsIII; Permissions: users-modify

[Files]
Source: "{tmp}\vorbisFile.zip"; DestDir: "{app}"; Flags: deleteafterinstall external; Check: IsSA; AfterInstall: Extract('{app}\vorbisFile.zip', 'vorbisFile.dll', '{app}');  Components: plugins/asiloader;
Source: "{tmp}\d3d8.zip"; DestDir: "{app}"; Flags: deleteafterinstall external; Check: not IsSA; AfterInstall: Extract('{app}\d3d8.zip', 'd3d8.dll', '{app}'); Components: plugins/d3d8to9;
Source: "{tmp}\SilentPatch.zip"; DestDir: "{app}"; Flags: deleteafterinstall external; AfterInstall: ExtractAll('{app}\SilentPatch.zip', '{app}'); Components: plugins/SilentPatch;
Source: "{tmp}\CheatMenu.zip"; DestDir: "{app}"; Flags: deleteafterinstall external; AfterInstall: ExtractAll('{app}\CheatMenu.zip', '{app}'); Components: program;
Source: "{tmp}\redist.exe"; DestDir: "{app}"; Flags: deleteafterinstall external; Components: plugins/redist;
Source: "{tmp}\dx9.exe"; DestDir: "{app}"; Flags: deleteafterinstall external; Components: plugins/dx9;
Source: "{tmp}\dinput8.zip"; DestDir: "{app}"; Flags: deleteafterinstall external; Check: not IsSA; AfterInstall: Extract('{app}\dinput8.zip', 'dinput8.dll', '{app}');  Components: plugins/asiloader;

[Run]
Filename: "{tmp}\redist.exe"; StatusMsg: "Installing Visual C++ Redistributable 2022 x86"; Check: WizardIsComponentSelected('plugins/redist'); Parameters: "/quiet"; Flags: waituntilterminated
Filename: "{tmp}\dx9.exe"; StatusMsg: "Installing DirectX End-User Runtime"; Check: WizardIsComponentSelected('plugins/dx9'); Flags: waituntilterminated

[InstallDelete]
Name: "{app}\Scripts\Globals.ini"; Type: files

[Code]
const
  SHCONTCH_NOPROGRESSBOX = 4;
  SHCONTCH_RESPONDYESTOALL = 16;

type
  eGameVer = (GTA_III, GTA_VC, GTA_SA, UNKNOWN);

var
  GameId: eGameVer;
  DownloadPage: TDownloadWizardPage;
  DefaultDirOnce: Boolean;

function IsSA(): Boolean;
begin
  Result := GameId = GTA_SA;
end;

function IsVC(): Boolean;
begin
  Result := GameId = GTA_VC;
end;

function IsIII(): Boolean;
begin
  Result := GameId = GTA_III;
end;

procedure unzipFile(Src, FileName, TargetFldr: PAnsiChar);
var
  Shell: variant;
  Item: variant;
  SrcFldr, DestFldr: variant;
begin
  if FileExists(Src) then 
  begin
    ForceDirectories(TargetFldr);

    Shell := CreateOleObject('Shell.Application');

    SrcFldr := Shell.NameSpace(string(Src));
    DestFldr := Shell.NameSpace(string(TargetFldr));
    Item := SrcFldr.ParseName(FileName);

    if not VarIsClear(Item) then
      DestFldr.CopyHere(Item, SHCONTCH_NOPROGRESSBOX or SHCONTCH_RESPONDYESTOALL);
 
  end;
end;

procedure unzipFolder(Src, TargetFldr: PAnsiChar);
var
  Shell: variant;
  SrcFldr, DestFldr: variant;
begin
  if FileExists(Src) then 
  begin
    ForceDirectories(TargetFldr);

    Shell := CreateOleObject('Shell.Application');

    SrcFldr := Shell.NameSpace(string(Src));
    DestFldr := Shell.NameSpace(string(TargetFldr));
    
    DestFldr.CopyHere(SrcFldr.Items, SHCONTCH_NOPROGRESSBOX or SHCONTCH_RESPONDYESTOALL);
 
  end;
end;

procedure Extract(Src, FileName, Target : String);
begin
  unzipFile(ExpandConstant(Src), ExpandConstant(FileName), ExpandConstant(target));
end;

procedure ExtractAll(Src, Target : String);
begin
  unzipFolder(ExpandConstant(Src), ExpandConstant(target));
end;

function GetSilentPatchName(): String;
begin
  if GameId = GTA_III then 
    Result := 'SilentPatchIII.asi'
  else if GameId = GTA_VC then 
    Result := 'SilentPatchVC.asi'
  else if GameId = GTA_SA then 
    Result := 'SilentPatchSA.asi';
end;

function IdentifyGame(Dir: String): eGameVer;
begin
  if FileExists(Dir + '\gta3.exe') then
  begin
    Result := GTA_III;
    Exit;
  end;
  if FileExists(Dir + '\gta-vc.exe') then
  begin
    Result := GTA_VC;
    Exit;
  end;
  if FileExists(Dir + '\gta_sa.exe') or FileExists(Dir + '\gta-sa.exe') or FileExists(Dir + '\gta_sa_compact.exe') then
  begin
    Result := GTA_SA;
    Exit;
  end;  
  Result := UNKNOWN; // unknown
end;

function InformIncompatibleMods(Dir: String): Boolean;
var
  Size: Integer;

begin
  if FileExists(Dir + '\enbseries.ini') then
    MsgBox('CheatMenu may not work properly with ENB modifications', mbInformation, MB_OK);
  
  if GameId = GTA_III then
  begin
    if FileSize(Dir + '\gta3.exe', Size) then
    begin
      if not Size = 2383872 then
        MsgBox('Unknown game version. Only GTA3 v1.0 EN is supported', mbInformation, MB_OK);
    end;
  end;

  if GameId = GTA_VC then
  begin
    if FileSize(Dir + '\gta-vc.exe', Size) then
    begin
      if Size <> 3088896 then
      begin
        MsgBox('Unknown game version. Only GTA VC v1.0 EN is supported', mbInformation, MB_OK);
      end;
    end;
  end;

  if GameId = GTA_SA then
  begin
    if FileSize(Dir + '\gta_sa.exe', Size) then
    begin
      if Size <> 14383616 then
      begin
        MsgBox('Unknown game version. Only GTA SA v1.0 US is supported', mbInformation, MB_OK);
      end;
    end;
  end;
    
  Result := True; // unknown
end;

function IsUnknown: Boolean;
begin
  Result := (GameId = UNKNOWN);
end;

function OnDownloadProgress(const Url, FileName: String; const Progress, ProgressMax: Int64): Boolean;
begin
  if Progress = ProgressMax then
    Log(Format('Successfully downloaded file to {tmp}: %s', [FileName]));
  Result := True;
end;  

procedure InitializeWizard;
var
  WinHttpReq: Variant;
begin
  DownloadPage := CreateDownloadPage(SetupMessage(msgWizardPreparing), SetupMessage(msgPreparingDesc), @OnDownloadProgress);

  // Check for active connection
  try
    WinHttpReq := CreateOleObject('WinHttp.WinHttpRequest.5.1');
    WinHttpReq.Open('GET', 'https://github.com/', False);
    WinHttpReq.Send('');
  except
    SuppressibleMsgBox('Cannot reach server. Please check your Internet connection.',
              mbError, MB_OK, IDOK);
    Abort;
  end;
end;
   
procedure CurPageChanged(CurPageID: Integer);
var
  I: Integer;
begin
  if CurPageID = wpSelectDir then 
  begin
    // delete \New Folder from the path
    if (DefaultDirOnce) then Exit;
    WizardForm.DirEdit.Text := ExpandConstant('{src}');
    DefaultDirOnce := True;
  end;

  if CurPageID = wpWelcome then 
  begin
    WizardForm.DirEdit.Text := 'thisi s a aasdjadljsalkdkll';
  end;

  if CurPageID = wpSelectComponents then
  begin
    GameId := IdentifyGame(WizardDirValue);
    InformIncompatibleMods(WizardDirValue);
      // reset all checkboxes to their initial state first
    for I := 1 to 4 do
    begin
      WizardForm.ComponentsList.ItemEnabled[I] := True;
      WizardForm.ComponentsList.Checked[I] := True;
    end;
  end;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
 
  if (CurPageID = wpSelectDir) and (WizardDirValue <> '') then
  begin
    GameId := IdentifyGame(WizardDirValue);
  end;

  if CurPageID = wpReady then 
  begin
    if (GameId = UNKNOWN) then
      GameId := IdentifyGame(WizardDirValue);

    DownloadPage.Clear;

    if GameId = GTA_III then
      DownloadPage.Add('{#CheatMenu}/{#AppVersion}/CheatMenuIII.zip', 'CheatMenu.zip', '');
    if GameId = GTA_VC then
      DownloadPage.Add('{#CheatMenu}/{#AppVersion}/CheatMenuVC.zip', 'CheatMenu.zip', '');
    if GameId = GTA_SA then
      DownloadPage.Add('{#CheatMenu}/{#AppVersion}/CheatMenuSA.zip', 'CheatMenu.zip', '');
      
    if WizardIsComponentSelected('plugins/asiloader') then
    begin
      if GameId = GTA_SA then
        DownloadPage.Add('{#UAL32}/vorbisFile.zip', 'vorbisFile.zip', '')
      else
        DownloadPage.Add('{#UAL32}/dinput8.zip', 'dinput8.zip', '');
    end;

    if WizardIsComponentSelected('plugins/d3d8to9') then
      DownloadPage.Add('{#D3D8to9Wrapper}', 'd3d8.zip', '');

    if (GameId <> UNKNOWN) and (WizardIsComponentSelected('plugins/SilentPatch')) then
    begin
      if GameId = GTA_III then
        DownloadPage.Add('{#SilentPatchIII}', 'SilentPatch.zip', '');
      if GameId = GTA_VC then
        DownloadPage.Add('{#SilentPatchVC}', 'SilentPatch.zip', '');
      if GameId = GTA_SA then
        DownloadPage.Add('{#SilentPatchSA}', 'SilentPatch.zip', '');
    end;

    if WizardIsComponentSelected('plugins/redist') then
      DownloadPage.Add('{#Redist}', 'redist.exe', '');

    if WizardIsComponentSelected('plugins/dx9') then
      DownloadPage.Add('{#DX9}', 'dx9.exe', '');

    DownloadPage.Show;
    try
      try
        DownloadPage.Download; // This downloads the files to {tmp}
        Result := True;
      except
        if DownloadPage.AbortedByUser then
          Log('Aborted by user.')
        else
          SuppressibleMsgBox(AddPeriod(GetExceptionMessage), mbCriticalError, MB_OK, IDOK);
        Result := False;
      end;
    finally
      DownloadPage.Hide;
    end;
  end 
    else
      Result := True;
end;



