Unofficial Linux agent for OCS Inventory NG
=====
A couple of years ago, where I work, we started using OCS Inventory NG (http://www.ocsinventory-ng.org), which
is a really useful application in enterprises. It allows system administrators to inventory servers and workstations,
in order to plan obsolescenses and investments.
In the last two years, we also started using Thinstation (http://www.thinstation.org) to recycle old computers
as Citrix terminals, by booting them via PXE.
Unfortunately, the OCS Inventory Agent for Unix is written in Perl. The Perl interpreter is quite big (~16MB) and
can't be put into the Thinstation images without getting performance hits on booting. Moreover, the agent doesn't
work on a system with a read only filesystem (actually, the filesystem is read-write, but changes are thrown out
when the terminal is powered off).
So I decided to write a small agent in C++ to be able to inventory also those machines which I'm not able to do, yet.
The agent is not complete yet, but it can already produce an incomplete inventory of the machine, which can be imported
into OCSInventory.
