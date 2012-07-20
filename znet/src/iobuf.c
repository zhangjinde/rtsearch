#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "allocator.h"
#include "iobuf.h"
#include "util.h"

struct iobuf
iobuf_init(allocator_t *allocator,size_t size)
{
    struct iobuf iob;
    iob.size = size;
    iob.off = 0;
    iob.skip = 0;
    iob.error = (iob.buf = mmalloc(allocator,size)) == NULL ? 1 : 0;
    return iob;
}

void
iobuf_free(allocator_t *allocator,struct iobuf *iob)
{
    if (iob->buf != NULL)
        mfree(allocator,iob->buf - iob->skip);
    iob->buf = NULL;
    iob->error = 1;
}

void iobuf_reset(struct iobuf *iob)
{
	iob->off = 0;
	iob->skip = 0;
}

void
iobuf_consumed(struct iobuf *iob, size_t count)
{
    if (iob->error)
        return;
    assert(count <= iob->off);
    iob->skip += count;
    iob->buf += count;
    iob->off -= count;
}

int
iobuf_accommodate(allocator_t *allocator,struct iobuf *iob, size_t count)
{
    if (iob->error)
        return 0;
    size_t esize = iob->size - (iob->skip + iob->off);
    if (esize >= count)
        return 1;
    else if (esize + iob->skip >= count) {
        bcopy(iob->buf, iob->buf - iob->skip, iob->off);
        iob->buf -= iob->skip;
        iob->skip = 0;
        return 1;
    } else {
	    uint8_t *nbuf = mmalloc(allocator,iob->size + count);
	    if (nbuf == NULL) {
		    iob->error = 1;
		    return 0;
	    }
	    bcopy(iob->buf - iob->skip,nbuf,iob->size);
	    mfree(allocator,iob->buf - iob->skip);
	    //uint8_t *nbuf = realloc(iob->buf - iob->skip, iob->size + count);
	    iob->buf = nbuf + iob->skip;
	    iob->size += count;
	    return 1;
    }
}

int
iobuf_print(allocator_t *allocator,struct iobuf *iob, const char *fmt, ...)
{
    if (iob->error)
        return 0;
    int np;
    va_list ap;
    va_start(ap, fmt);
    np = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (!iobuf_accommodate(allocator,iob, np + 1))
        return 0;
    va_start(ap, fmt);
    vsnprintf((char *)(iob->buf + iob->off), np + 1, fmt, ap);
    va_end(ap);
    iob->off += np;
    return 1;
}

int
iobuf_write(allocator_t *allocator,struct iobuf *iob, const void *buf, size_t count)
{
    if (iob->error)
        return 0;
    if (!iobuf_accommodate(allocator,iob, count))
        return 0;
    bcopy(buf, iob->buf + iob->off, count);
    iob->off += count;
    return 1;
}

void *
iobuf_find(struct iobuf *iob, const void *p, size_t plen)
{
    return iob->error ? NULL : memfind(p, plen, iob->buf, iob->off);
}
