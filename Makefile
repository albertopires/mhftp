LIBS=
CC=g++
CFLAGS=-Wall -Wextra -g -pedantic -std=c++11
CRYPT_DIR=./crypt
PROTO_DIR=./protocol
VERS='"v1.1.1 - 20/Aug/2017"'
LINK_OPTS=-lcrypto -lefence

all: mhftp mhclient initFile mhstatus

clean:
	find . -name "*.o" -exec rm {} \;
	rm -f mhftp
	rm -f mhclient
	rm -f initFile
	rm -f mhstatus
	rm -f testColors

mhftp: mhftp.o con_tcp.o tools.o $(CRYPT_DIR)/md5Utils.o $(PROTO_DIR)/chunk.o \
       $(PROTO_DIR)/mhproto_server.o $(PROTO_DIR)/metadata.o $(PROTO_DIR)/semaphore.o
	$(CC) $(LIBS) $(CFLAGS) -o mhftp mhftp.o con_tcp.o tools.o \
	$(CRYPT_DIR)/md5Utils.o $(PROTO_DIR)/chunk.o \
	$(PROTO_DIR)/mhproto_server.o $(PROTO_DIR)/metadata.o \
	$(PROTO_DIR)/semaphore.o $(LINK_OPTS)

mhftp.o: mhftp.cpp global.h
	$(CC) $(CFLAGS) -DVER=$(VERS) -c $*.cpp -o $@

mhclient: mhclient.o con_tcp.o tools.o $(PROTO_DIR)/mhproto_client.o \
    $(CRYPT_DIR)/md5Utils.o $(PROTO_DIR)/metadata.o $(PROTO_DIR)/chunk.o \
    $(PROTO_DIR)/semaphore.o global.h
	$(CC) $(LIBS) $(CFLAGS) -o mhclient mhclient.o con_tcp.o tools.o \
	$(PROTO_DIR)/mhproto_client.o \
	$(PROTO_DIR)/metadata.o $(PROTO_DIR)/chunk.o $(CRYPT_DIR)/md5Utils.o \
	$(PROTO_DIR)/semaphore.o $(LINK_OPTS)

mhstatus: mhstatus.o con_tcp.o tools.o $(PROTO_DIR)/metadata.o $(CRYPT_DIR)/md5Utils.o\
   	$(PROTO_DIR)/metadata.o global.h
	$(CC) $(LIBS) $(CFLAGS) -o mhstatus mhstatus.o con_tcp.o tools.o $(PROTO_DIR)/metadata.o \
	$(CRYPT_DIR)/md5Utils.o $(LINK_OPTS)


initFile: initFile.o con_tcp.o tools.o $(PROTO_DIR)/metadata.o \
    $(CRYPT_DIR)/md5Utils.o $(PROTO_DIR)/chunk.o $(PROTO_DIR)/semaphore.o global.h
	$(CC) $(LIBS) $(CFLAGS) -o initFile initFile.o con_tcp.o tools.o \
	$(CRYPT_DIR)/md5Utils.o $(PROTO_DIR)/metadata.o $(PROTO_DIR)/chunk.o \
	$(PROTO_DIR)/semaphore.o $(LINK_OPTS)

testColors: testColors.o tools.o global.h
	$(CC) $(LIBS) $(CFLAGS) -o testColors testColors.o tools.o

initFile.o: initFile.cpp global.h
	$(CC) $(CFLAGS) -DVER=$(VERS) -c $*.cpp -o $@

mhstatus.o: mhstatus.cpp global.h
	$(CC) $(CFLAGS) -DVER=$(VERS) -c $*.cpp -o $@

mhclient.o: mhclient.cpp global.h
	$(CC) $(CFLAGS) -DVER=$(VERS) -c $*.cpp -o $@

con_tcp.o: con_tcp.cpp con_tcp.h tools.o global.h
	$(CC) $(CFLAGS) -c $*.cpp -o $@

tools.o: tools.cpp tools.h global.h
	$(CC) $(CFLAGS) -c $*.cpp -o $@

$(CRYPT_DIR)/md5Utils.o: $(CRYPT_DIR)/md5Utils.cpp
	$(CC) $(CFLAGS) -c $*.cpp -o $@

$(PROTO_DIR)/chunk.o: $(PROTO_DIR)/chunk.cpp  $(PROTO_DIR)/chunk.h
	$(CC) $(CFLAGS) -c $*.cpp -o $@

$(PROTO_DIR)/metadata.o: $(PROTO_DIR)/metadata.cpp  $(PROTO_DIR)/metadata.h
	$(CC) $(CFLAGS) -c $*.cpp -o $@

$(PROTO_DIR)/mhproto_server.o: $(PROTO_DIR)/mhproto_server.cpp  $(PROTO_DIR)/mhproto_server.h
	$(CC) $(CFLAGS) -c $*.cpp -o $@

$(PROTO_DIR)/mhproto_client.o: $(PROTO_DIR)/mhproto_client.cpp  $(PROTO_DIR)/mhproto_client.h
	$(CC) $(CFLAGS) -c $*.cpp -o $@

$(PROTO_DIR)/semaphore.o: $(PROTO_DIR)/semaphore.cpp  $(PROTO_DIR)/semaphore.h
	$(CC) $(CFLAGS) -c $*.cpp -o $@

$(PROTO_DIR)/testColors.o: testColors.cpp tools.h
	$(CC) $(CFLAGS) -c $*.cpp -o $@
