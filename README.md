# spdynu
![GitHub release (latest by date)](https://img.shields.io/github/v/release/StevenSeifried/spdynu?style=flat-square) ![GitHub](https://img.shields.io/github/license/StevenSeifried/spdynu?style=flat-square)
![GitHub issues](https://img.shields.io/github/issues-raw/StevenSeifried/spdynu?style=flat-square) ![GitHub closed issues](https://img.shields.io/github/issues-closed-raw/StevenSeifried/spdynu?style=flat-square)
![GitHub pull requests](https://img.shields.io/github/issues-pr-raw/StevenSeifried/spdynu?style=flat-square) ![GitHub closed pull requests](https://img.shields.io/github/issues-pr-closed-raw/StevenSeifried/spdynu?style=flat-square)
![AUR version](https://img.shields.io/aur/version/spdynu?style=flat-square)

The spdynu is based on the spdns Dynamic DNS Update-Client from http://my5cent.spdns.de/de/posts/spdns-dynamic-dns-update-client (Domain still offline)

Feel free to read the TODO section below and help with the client to improve or to expand with 
new features. Please use the development Branch.

I change the URLs from spdns.de to spdyn.de

spdyn is an Dynamic DNS Service from Securepoint GmbH. In 2016, the Securepoint GmbH has renamed his own DDNS service from spdns to spdyn.

The servers of Securepoint GmbH and also the servers of the dynamic DNS service spdyn are in Germany.

 - spdynUpdater.c is the Source Code in C
 - spdynu.conf is the config file

Packages are available for amd64, armhf and arm64 under [https://apt.canox.net/](https://apt.canox.net/)
```
wget --no-check-certificate https://apt.canox.net/apt.canox.net.gpg.key && sudo apt-key add apt.canox.net.gpg.key
```
For amd64
```
echo "deb [arch=amd64] https://apt.canox.net/ bionic main" | sudo tee /etc/apt/sources.list.d/canoxnet.list
```
For armf
```
echo "deb [arch=armhf] https://apt.canox.net/ raspbian main" | sudo tee /etc/apt/sources.list.d/canoxnet.list
```
# TODO

- Add IPv6 Support
- Add Version Parameter
- Make RPM Package
