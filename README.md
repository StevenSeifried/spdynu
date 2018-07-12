The spdyn-updater is based on the spdns Dynamic DNS Update-Client from http://my5cent.spdns.de/de/posts/spdns-dynamic-dns-update-client

Feel free to read the To Do list and help with the client to improve or to expand with new features

I change the URLs from spdns.de to spdyn.de and add support for IPv6

spdyn is an Dynamic DNS Service from Securepoint GmbH. In 2016, the Securepoint GmbH has renamed his own DDNS service from spdns to spdyn.

The servers of Securepoint GmbH and also the servers of the dynamic DNS service spdyn are in Germany.

spdynUpdater.c is the Source Code in C
spdynUpdater is the executable program
spdynu.conf is the config file

compile with the command:

gcc spdynUpdater.c -o spdynu

[![pipeline status](https://gitlab.com/CANOXNET/spdyn-updater/badges/master/pipeline.svg)](https://gitlab.com/CANOXNET/spdyn-updater/commits/master)