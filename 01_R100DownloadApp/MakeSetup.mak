# FILE: MakeSetup.mak
#
# DESCRIPTION: MakeSetup.mak Copies various files from the build process into 
# their appropriate locations in the setup source tree. MakeSetup.mak should 
# be run from the setup project's root directory.
#
# USAGE: nmake /f MakeSetup.mak BuildType=bt SetupSourceFiles=ssf
#  BuildType is the name of the build type (typically Debug or Release).
#  SetupSourceFiles is the (relative) path to the setup source files.

all: \
	"$(SetupSourceFiles)\Application\DesktopGUI.exe" \
	"$(SetupSourceFiles)\_Setup.exe" \
	"$(SetupSourceFiles)\_Setup.hlp"

"$(SetupSourceFiles)\Application\DesktopGUI.exe": "..\current\DesktopGUI\$(BuildType)\DesktopGUI.exe"
	copy "..\current\DesktopGUI\$(BuildType)\DesktopGUI.exe" "$(SetupSourceFiles)\Application\DesktopGUI.exe"

"$(SetupSourceFiles)\_Setup.exe": "$(BuildType)\setup.exe"
	copy "$(BuildType)\setup.exe" "$(SetupSourceFiles)\_Setup.exe"

"$(SetupSourceFiles)\_Setup.hlp": "$(BuildType)\setup.hlp"
	copy "$(BuildType)\setup.hlp" "$(SetupSourceFiles)\_Setup.hlp"

