# build config setting
* To build this driver, you need build the **tdi_fw** lib first.
* change `C/C++ -> Treat Warnings As Erros` to `NO(/WX)`

* if build for x64 pc, change the `General -> Target Name` to `adfilter64`

* add **header path** `$(SolutionDir)sys/tdi_fw/src;$(IncludePath)` in `VC++ Directories -> Include Directories`
* add **lib path** `$(SolutionDir)sys/tdi_fw/src;$(IncludePath)` in `VC++ Directories -> Include Directories`

* in `Linker -> Input ` add the `tdi.lib` and `tdi_fw.lib`

* for installer add a post script `copy "$(TargetPath)" "$(SolutionDir)installer\res\$(TargetFileName)" /Y` to `Build Events -> Post Build Event -> Command Line`

