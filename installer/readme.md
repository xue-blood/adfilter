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