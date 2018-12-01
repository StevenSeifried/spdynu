The spdynu is based on the spdns Dynamic DNS Update-Client from http://my5cent.spdns.de/de/posts/spdns-dynamic-dns-update-client (Domain still offline)

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

echo "deb [arch=amd64] https://apt.canox.net/ bionic main" | sudo tee /etc/apt/sources.list.d/canoxnet.list

For arm64

echo "deb [arch=arm64] https://apt.canox.net/ bionic main" | sudo tee /etc/apt/sources.list.d/canoxnet.list

For armf

echo "deb [arch=armhf] https://apt.canox.net/ bionic main" | sudo tee /etc/apt/sources.list.d/canoxnet.list

# TODO

- Add IPv6 Support
