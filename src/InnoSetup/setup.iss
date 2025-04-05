
; Inno Setup Script für LanguageSwitcher Setup

[Setup]
AppName=LanguageSwitcher
AppVersion=1.0
DefaultDirName={pf}\LanguageSwitcher
DefaultGroupName=LanguageSwitcher
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes

[Files]
Source: "bin\LanguageSwitcher.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "resources\resources_de.dll"; DestDir: "{app}\resources"; Flags: ignoreversion
Source: "resources\resources_en.dll"; DestDir: "{app}\resources"; Flags: ignoreversion
Source: "settings.ini"; DestDir: "{app}"; Flags: ignoreversion
Source: "LICENSE"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\LanguageSwitcher"; Filename: "{app}\LanguageSwitcher.exe"
Name: "{group}\Uninstall LanguageSwitcher"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\LanguageSwitcher.exe"; Description: "{cm:LaunchProgram,LanguageSwitcher}"; Flags: nowait postinstall skipifsilent
