[![maintained status](https://canox.net/wp-content/uploads/2018/07/maintained.svg)](https://gitlab.com/CANOXNET/spdyn-updater/commits/master)
[![pipeline status](https://gitlab.com/CANOXNET/spdyn-updater/badges/master/pipeline.svg)](https://gitlab.com/CANOXNET/spdyn-updater/commits/master)
[![license status](https://canox.net/wp-content/uploads/2018/07/license.svg)](https://gitlab.com/CANOXNET/spdynu/blob/master/LICENSE)

The spdynu is based on the spdns Dynamic DNS Update-Client from http://my5cent.spdns.de/de/posts/spdns-dynamic-dns-update-client

Feel free to read the TODO section below and help with the client to improve or to expand with 
new features

I change the URLs from spdns.de to spdyn.de and add support for IPv6

spdyn is an Dynamic DNS Service from Securepoint GmbH. In 2016, the Securepoint GmbH has renamed his own DDNS service from spdns to spdyn.

The servers of Securepoint GmbH and also the servers of the dynamic DNS service spdyn are in Germany.

 - spdynUpdater.c is the Source Code in C
 - spdynu.conf is the config file

Compile it with the command:

gcc spdynUpdater.c -o spdynu

Downloads are available at the following URL:

[https://cloud.canox.net/index.php/s/Lo6zYaEBPL2GtzF](https://cloud.canox.net/index.php/s/Lo6zYaEBPL2GtzF)

Please note the following open issues:

[https://gitlab.com/CANOXNET/spdynu/issues](https://gitlab.com/CANOXNET/spdynu/issues)

# TODO

- Support for IPv6 was removed with Release 2018.08, but will add back in the future
- Build .deb Packages for armhf and arm64 for Debian 8, Debian 9, Ubuntu 16.04 and 18.04 with 
GitLab CI (Unfortunately, this does not work at the moment)
- Add --version and/or -v Parameter
