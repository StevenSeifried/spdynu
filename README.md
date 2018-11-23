[![maintained status](https://canox.net/wp-content/uploads/2018/07/maintained.svg)](https://gitlab.com/CANOXNET/spdyn-updater/commits/master)
[![pipeline status](https://gitlab.com/CANOXNET/spdyn-updater/badges/master/pipeline.svg)](https://gitlab.com/CANOXNET/spdyn-updater/commits/master)
[![license status](https://canox.net/wp-content/uploads/2018/07/license.svg)](https://gitlab.com/CANOXNET/spdynu/blob/master/LICENSE)

The spdynu is based on the spdns Dynamic DNS Update-Client from http://my5cent.spdns.de/de/posts/spdns-dynamic-dns-update-client

Feel free to read the TODO section below and help with the client to improve or to expand with 
new features

I change the URLs from spdns.de to spdyn.de

spdyn is an Dynamic DNS Service from Securepoint GmbH. In 2016, the Securepoint GmbH has renamed his own DDNS service from spdns to spdyn.

The servers of Securepoint GmbH and also the servers of the dynamic DNS service spdyn are in Germany.

 - spdynUpdater.c is the Source Code in C
 - spdynu.conf is the config file

Packages are available for amd64, armhf and arm64 under [https://apt.canox.net/](https://apt.canox.net/)

wget --no-check-certificate https://apt.canox.net/apt.canox.net.gpg.key && sudo apt-key add apt.canox.net.gpg.key

For amd64

echo "deb [ARCH=amd64] https://apt.canox.net/ bionic main" | sudo tee /etc/apt/sources.list.d/canoxnet.list

For arm64

echo "deb [ARCH=arm64] https://apt.canox.net/ bionic main" | sudo tee /etc/apt/sources.list.d/canoxnet.list

For armf

echo "deb [ARCH=armhf] https://apt.canox.net/ bionic main" | sudo tee /etc/apt/sources.list.d/canoxnet.list

Please note the following open issues:

[https://gitlab.com/CANOXNET/spdynu/issues](https://gitlab.com/CANOXNET/spdynu/issues)

