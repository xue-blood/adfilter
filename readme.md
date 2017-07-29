# ADFilter
Filter AD in windows kernel.

![demo](https://user-images.githubusercontent.com/18024882/28399960-dc6acb18-6d42-11e7-9725-dc71e34e7927.png)

## How to use
just add host any you want to filter.

## How to build
To build driver and application, you need [Visual Studio 2013]() and [WDK8.1]()<br>
To build installer package, you need [nsis]().

This is repository contain those projects:
* [Tdi fw](./sys/tdi_fw) <br>
    A *kernel lib* implement the commuication with windows **TDI** Driver,<br>
    and provide a interface for handling network packet.
* [ADFilter driver](./sys/adfilter)<br>
    This is the driver we write for filter dns message in kernel.
* [ADFCon](./exe/adfcon)<br>
    This project provides a *C dll* for User Application access to the driver.
    (It also has a command line interface,but not recommended)
* [ADFilter application](./exe/adfilter)<br>
    This is a WPF application for control the driver.(make sure it has administrator privalge)
* [Installer](./installer)<br>
    You can use the nsis script use build a package for install and uninstall,for more information please see this [document](./installer/readme.md)

## Update
2017.7.29: 
* add a project to build nsis installer in visual studio.
* use Regular Expression [SLRE](https://github.com/cesanta/slre) replace the absolute compare.
![regexp](./img/regexp.png)