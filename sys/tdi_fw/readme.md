# build config setting
* first change the `General -> Project Defaults -> Configuration Type` to `Static library (.lib)`<br>
`Target Extension` to `.lib`
* change `C/C++ -> Treat Warnings As Erros` to `NO(/WX)`
* **Add** a new Macro `USE_PACKET_ENGINE` in `C/C++ -> PreProcessor -> Preprocessor Definitions`


## option
* the driver default can't unload dynamic, if you want to enable it Add a new Macro `CAN_UNLOAD` in `C/C++ -> PreProcessor -> Preprocessor Definitions`,but **it's not stable and not recommented**