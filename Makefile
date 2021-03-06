CFLAGS=`getconf LFS_CFLAGS`\
    -ggdb\
    -std=gnu99\
    `pkg-config --cflags fuse`\
    `pkg-config --cflags libcurl`

LDFLAGS=`getconf LFS_LDFLAGS`\
    `pkg-config --libs fuse`\
    `pkg-config --libs libcurl`

HEADERS=src/remotestorage-fuse.h
OBJECTS=src/main.o src/operations.o src/trie.o src/node_store.o src/helpers.o src/sync.o src/remote.o

rs-mount: $(OBJECTS) $(HEADERS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJECTS) rs-mount

## secondary targets (for testing stuff, some of this isn't in use right now):

btree: src/bplus.o $(HEADERS)
	$(CC) src/bplus.o -o $@ $(LDFLAGS)

trie: src/trie.c $(HEADERS)
	$(CC) src/trie.c -o $@ $(CFLAGS) $(LDFLAGS) -DDEBUG_TRIE

helpers: src/helpers.c $(HEADERS)
	$(CC) src/helpers.c -o $@ $(CFLAGS) $(LDFLAGS) -DDEBUG_HELPERS
