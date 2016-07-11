# Multi-homed File Transfer Protocol #

MhFtp is a tool to allow file transfer over a multi-homed host, that is, a host with more than one Internet link, using one or more links, summing up all bandwidths.

## How it works ##
Basically it breaks a file into a number of parts (Chunks), open one connection per link, and start transferring each chunk separately. All information about each chunk, like its size, md5sum, status (DOWNLOADING, UPLOADING, PENDING, ERROR) and complete file md5 are stored in a metadata file, using a **.mdata** extension. For example, if you download a file named **archive.tar**, then an **archive.tar.mdata** will be stored in your current directory.

This metadata file will be generated by the server and sent to the client on first time transfer and will be stored on the client side. Later this **.mdata** file may be used to resume transmission, check for corrupted chunks, and retransmit single  failed chunks.
If a metadata file matching the file name is present on the current directory then it'll be used to resume a transfer. One scenario where this may happen is when the user presses CTRL+C and interrupts the transfer, in that case the file will be partially downloaded and the **mhclient** will be able to resume the transfer.
<br>This may also happen if the client program aborts unexpectedly like a SIGKILL, power failure, network error, etc.


As of this version, the chunk size is fixed at 1MB, but very soon it will be configurable. Although the size of 1MB suits a good range of file sizes, for larger files the metadata file may become too big.

## Before you ask ##
It could be a **FAQ**, but so far there are not Frequently Asked Questions, or any asked question for that matter, but, a few things you should know:

* To take advantage of **n** links, the server must be running on a multi-homed host.
* Your multi-homed host must be configured properly. I'm talking about symmetric routing. If you don't know what it that is, take a look at this [howto](http://www.microhowto.info/howto/ensure_symmetric_routing_on_a_server_with_multiple_default_gateways.html).

## Compiling ##

Requirements:

* GNU g++ 4.8
* libssl-dev 1.0.1f
* Electric Fence (optional)

It's possible to compile MhFtp on older versions of g++, although it may be necessary to comment the compile options **-pedantic** and **-std=c++11** on your Makefile.

``CFLAGS=-Wall -g #-pedantic -std=c++11``

and then, just run make.

## Usage ##
After a successful build, there will be the following binaries:

* mhftp
* mhclient
* mhstatus
* initFile

***mhftp*** is the server program. There are two options:

* **-v** shows version
* **-p port** The default port is 5000.

***mhclient*** is the client program. Example:

`
user@server:~/git/mhftp (master)$ ./mhclient download archive.tar 192.168.120.10:5000 172.16.30.2:5000
`

or

`
user@server:~/git/mhftp (master)$ ./mhclient upload archive.tar 192.168.120.10:5000 172.16.30.2:5000
`


***mhstatus*** inspects and modifies the metadata file.<br>
Just running `mhstatus file.mdata` will show the number of chunks and a sequence of letters indicating the status of each chunk. These status are; **P** for pending, **D** for Downloading, **E** for error and **O** for Ok or successfully downloaded.<br>

Run `mhstatus -v file.mdata`, for verbose, it will list all chunks status, md5sum and size.<br>

The option `--recover or -r` can be used if the **mhclient** program terminates unexpectedly. When that happens, some chunks may be marked as in Downloading state or the metadata file itself may be locked. The recover option clears this lock and sets all chunks with status a not OK to pending.<br>
The option `--reset or -e` is used to reset all chunks to pending state. This is useful only if you want to download the file again, as the metadata file will have all the chunks marked OK after the download. If another download for the same file is attempted, the client program will not download the file. Another way to re-download the file is to erase the metadata file.<br>
The option `--check or -c` will re-calculate and check all md5sum's for all chunks and detect any corrupted chunk and mark it as Error. After that, the user may issue the **mhclient** again to download the corrupted chunks.

***initFile*** creates and inspects metadata file. <br>
It's used for debugging only.

## Contributing ##
For now it's just me, so I'll try to keep it simple. Basically, **master** branch will have the latest (hopefully stable) version, and there will be a **next** branch where I'll be working on upcoming features.

Contributing is not only about code. Besides a pull request, you can help by testing, documenting, reporting bugs, sending money, beer, etc...

About code contribution, I'll kindly ask to stick to the actual code style. I decided to use the Google Code Style. I know people have different opinions and I respect them all, but, code style is about readability and that can not be achieved without consistency. So there can be only one. And Google has [cpplint](https://github.com/google/styleguide/tree/gh-pages/cpplint) (thanks Google) that helps a lot to keep a clean code.

## Contributing - Documentation ##
Some of you may have noticed a **gh-pages** branch. It's used by github to publish a web page and it has some nice features. But what you need to know is that I'm just using a [Jekyll template](http://jekyllrb.com/) to render my README.md to a nice page. So all you need to do is just edit the README.md file in the master branch and send a pull request. No need go through the **gh-pages** branch, unless, of course, you found something wrong with the template itself.

### Workflow ###
So far, what we have is; **master** for latest "stable" version, **next** for developing upcoming features and I may create some temporary bug fix branch from **master** for small bugs, that will later be merged to **next**.<br>
That workflow is not written in stone and is not the best one in the world, so it may change in the future.




So, fork it and have fun.
