editor
======
* [visual studio code](https://code.visualstudio.com) + NSIS(externsion)
* [Unicode NSIS](http://www.scratchpaper.com)

build
=====
* add `copy "$(TargetPath)" "$(SolutionDir)installer\res\$(TargetFileName)" /Y`
to *vs project setting* -- *post build event*

* build project in visual code,
* open adfilter.nsi in vs code build the installer.
* or use `makensis adfilter.nsi` command line tool
* this file should in res directory:
<br>`adf.dll`
<br> `adfilter.exe` 
<br> `adfilter.sys` 
<br> `adfilter64.exe` 
<br> `dotnetfx45_full_setup.exe` 
<br> `except.txt`
<br>  `MahApps.Metro.dll`
<br>   `segmdl2.ttf` 
<br>   `sys.txt` 
<br>   `System.Windows.Interactivity.dll` 
<br>   `user.txt`