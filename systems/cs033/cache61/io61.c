#include "io61.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define BLOCK 32768             /* I'm pretty sure stdio.h uses 32k buffers */
#define REVERSE 16384

typedef struct io61_file {
    int fd;
    int mode;

    size_t abs;                 /* position of cache line in file */
    size_t pos;                 /* position in buffer */
    size_t sz;                  /* total size of buffer */
    unsigned char buf[BLOCK];

    size_t wsz;                 /* total size of wbuf */
    write_data wbuf[BLOCK];
} io61_file;


// wbuf_index(a)
//    find correct index, so if f->wsz == 0 return 0, else return
//    f->wsz - 1

int wbuf_index(int a) {
  if(a)
    return (a - 1);
  else
   return a;
}


// linear_insert(f, pos, sz)
//    private helper function; let's place write metadata into wbuf.
//    the trick is to combine contiguous blocks. a write can either be
//    at the edge of a previous write, within a previous write, or outside
//    a write. this'll be a time sink as it'll run in O(n^2). in the case
//    where all writes are contiguous running time will be closer to n |
//    n is the number of writes. i'm going to be a little hackish here and only
//    call this in writec() since this'll ensure best case running time

void linear_insert(io61_file* f, size_t pos, size_t sz) {
  for(int i = 0; i < BLOCK; i++) {
    /* we didn't find anything in wbuf that was contiguous to the current write */
    if(!f->wbuf[i].wpos && !f->wbuf[i].wsz) {
      f->wbuf[i].wpos = pos;
      f->wbuf[i].wsz = sz;
      break;
    }
    else {
      size_t wpos = f->wbuf[i].wpos;
      size_t wsz = f->wbuf[i].wsz;

      /* if the current write is at the end of a previous write */
      if(pos == (wpos + wsz)) {
        f->wbuf[i].wsz += sz;
        break;
      }

      /* the position of the current write starts within a previous write */
      if(pos >= wpos && pos <= (wpos + wsz)) {
        /* current write ends outside the previous write */
        if(pos + sz > wpos + wsz) {
          f->wbuf[i].wsz += (pos + sz) - (wpos + wsz);
          break;
        }
        /* current write is subsumed by previous write */
        else
          break;
      }

      /* the current write will start before a previous write */
      if(pos < wpos) {
        /* current write will end within a previous write */
        if((pos + sz >= wpos) && (pos + sz <= wpos + wsz)) {
          f->wbuf[i].wsz += f->wbuf[i].wpos - pos;
          f->wbuf[i].wpos = pos;
          break;
        }

        /* the write has essentially completely overwritten a previous write */
        if(pos + sz > wpos + wsz) {
          f->wbuf[i].wsz = sz;
          f->wbuf[i].wpos = pos;
          break;
        }
      }
    }
  }
}


// bin_insert_helper(f, pos, sz, first, e)
//    place a new element in the correct position in the f->wbuf array

void bin_insert_helper(io61_file* f, size_t pos, size_t sz, int first, enum side e) {

  char buffer[f->wsz * sizeof(write_data)];

  if(!f->wsz) {
    f->wbuf[0].wsz = sz;
    f->wbuf[0].wpos = pos;
  }

  else if((e == right) && (first >= f->wsz)) {
    f->wbuf[first].wsz = sz;
    f->wbuf[first].wpos = pos;
  }

  /* this is the case where first > last and the index 'pos'
   * should be placed in is > than 'index'*/
  else if(e == right) {
    memcpy(buffer, f->wbuf + first, (f->wsz - first) * sizeof(write_data));
    memcpy(f->wbuf + first + 1, buffer, (f->wsz - first) * sizeof(write_data));
    f->wbuf[first].wsz = sz;
    f->wbuf[first].wpos = pos;
  }

  /* if first > last and the index 'pos' should be placed in
   * is < than index */
  else {
    memcpy(buffer, f->wbuf + first, (f->wsz - first) * sizeof(write_data));
    memcpy(f->wbuf + first + 1, buffer, (f->wsz - first) * sizeof(write_data));
    f->wbuf[first].wsz = sz;
    f->wbuf[first].wpos = pos;
  }
}


// binary_insert(f, pos, sz, first, last, e)
//    Unlike linear_insert, which has a complexity of O(n^2), let's use
//    something like a binary tree (using an array) to speed things up.
//    You'll never remove things out of wbuf (so it's not really a binary tree),
//    however, it'll improve the running time of inserting things to O(nlog[n])
//
//    We'll call this function in write(). Let's do this iteratively to hopefully
//    decrease any latency from recursive function calls.

void binary_insert(io61_file* f, size_t pos, size_t sz, int first, int last, enum side e) {
  while(1) {
    int index = (first + last) / 2;

    /* base case; this will increase the number of elements in the wbuf */
    if(first > last) {
      bin_insert_helper(f, pos, sz, first, e);
      f->wsz++;
      break;
    }

    /* let's find the position and size of the middle write_data struct */
    size_t mpos = f->wbuf[index].wpos;
    size_t msz = f->wbuf[index].wsz;
    /* if what we're trying to insert has a pos >= that of the current write_data struct */
    if(pos >= mpos) {
      /* if the current write is at the end of a previous write */
      if(pos == (mpos + msz)) {
        f->wbuf[index].wsz += sz;

        if(!f->wsz)
          f->wsz++;

        break;
      } else if (pos >= mpos && pos <= (mpos + msz)) {
        /* current write ends outside the previous write */
        if(pos + sz > mpos + msz) {
          f->wbuf[index].wsz += (pos + sz) - (mpos + msz);
          break;
        } else {
          break;
        }
      } else {
        first = index + 1;
        e = right;
        continue;
    }
    } else {
      /* current write will end within a previous write */
      if((pos + sz >= mpos) && (pos + sz <= mpos + msz)) {
        f->wbuf[index].wsz += f->wbuf[index].wpos - pos;
        f->wbuf[index].wpos = pos;
        break;
      } else if(pos + sz > mpos + msz) {
        f->wbuf[index].wsz = sz;
        f->wbuf[index].wpos = pos;
      } else {
        last = index - 1;
        e = left;
        continue;
      }
    }
  }
}


// flush_wbuf(f)
//    this'll be called to flush out the file's buffer (f->buf) when writing.
//    if f->buf is full or a single contiguous block, this should result in a single
//    write. if there are blocks scattered around from a program like reordercat61.c,
//    this should print them out in the least number of writes possible -- merging of
//    blocks should've taken place in insert_into_wbuf().
//
//    will return the number of characters written out

int flush_wbuf(io61_file* f) {
  int nwritten = 0;
  for(int i = 0; i < BLOCK; i++) {
    if(!f->wbuf[i].wpos && !f->wbuf[i].wsz)
      break;

    lseek(f->fd, f->abs + f->wbuf[i].wpos, SEEK_SET);
    int temp = write(f->fd, f->buf + f->wbuf[i].wpos, f->wbuf[i].wsz);

    if(temp < 0)
      return -1;

    nwritten += temp;

    /* set back to default */
    f->wbuf[i].wpos = 0;
    f->wbuf[i].wsz = 0;
  }

  f->wsz = 0;
  return nwritten;
}


// io61_fdopen(fd, mode)
//    Return a new io61_file for file descriptor `fd`. `mode` is
//    either O_RDONLY for a read-only file or O_WRONLY for a
//    write-only file. You need not support read/write files.

io61_file* io61_fdopen(int fd, int mode) {
    assert(fd >= 0);
    io61_file* f = (io61_file*) malloc(sizeof(io61_file));
    f->fd = fd;
    f->mode = mode;
    return f;
}


// io61_close(f)
//    Close the io61_file `f` and release all its resources.

int io61_close(io61_file* f) {
    io61_flush(f);
    int r = close(f->fd);
    free(f);
    return r;
}


// io61_readc(f)
//    Read a single (unsigned) character from `f` and return it. Returns EOF
//    (which is -1) on error or end-of-file.

int io61_readc(io61_file* f) {
    /* check if we've already used everything in the buffer */
    if(!(f->pos ^ f->sz)) {
      size_t nread = 0;

      /* we'll always reverse prefetch either REVERSE bytes or f->abs + f->pos
       * bytes, whichever is smaller */
      if(f->abs + f->pos >= REVERSE) {
	off_t r = lseek(f->fd, -REVERSE, SEEK_CUR);

	if(r != (off_t) (f->abs + f->pos - REVERSE) && !f->sz)
	  return -1;

        nread = read(f->fd, f->buf, BLOCK);

	/* if we can't read enough to get back to the previous
	 * position in the file */
	if(nread <= REVERSE)
	  return EOF;

	f->abs += f->pos - REVERSE;
	f->pos = REVERSE;
	f->sz = nread;
	return f->buf[f->pos++];
      }
      else {
	/* this should be the same as lseek(f->fd, 0, SEEK_SET) */
	off_t r = lseek(f->fd, -(f->abs + f->pos), SEEK_CUR);
	if(r)
	  return -1;

	nread = read(f->fd, f->buf, BLOCK);

	/* if read() brings us back to our original position, there is nothing
	 * new to read in */
        if(nread <= f->abs + f->pos)
          return EOF;

	f->pos += f->abs;
        f->abs = 0;
        f->sz = nread;
        return f->buf[f->pos++];
      }
    }
    else {
      return f->buf[f->pos++];
    }
}


// io61_read(f, buf, sz)
//    Read up to `sz` characters from `f` into `buf`. Returns the number of
//    characters read on success; normally this is `sz`. Returns a short
//    count, which might be zero, if the file ended before `sz` characters
//    could be read. Returns -1 if an error occurred before any characters
//    were read.

ssize_t io61_read(io61_file* f, char* buf, size_t sz) {
    size_t nread = 0;
    size_t nread_total = 0;

    /* if read can be fulfilled by the buffer */
    if(sz <= (f->sz - f->pos)) {
      memcpy(buf, f->buf + f->pos, sz);
      f->pos += sz;
      nread_total = sz;
    }
    else {
      /* let's flush the buffer TODO: problem here -- what if f->buf is full when we get here? */
      size_t diff = f->sz - f->pos;
      if(diff)
        memcpy(buf, f->buf + f->pos, diff);

      sz -= diff;
      nread_total += diff;
      f->pos = f->sz;

      /* at this point f->pos == f->sz; I'll read in more from the current
       * pos, so update f->abs to point to this pos before I do */
      f->abs += f->pos;

      /* let's read in enough to fulfill read request and refill buffer */
      unsigned char buffer[sz + BLOCK];
      nread = read(f->fd, buffer, sz + BLOCK);

      if(nread <= sz) {
	memcpy(buf + nread_total, buffer, nread);
	f->pos = 0;
	f->sz = 0;
	nread_total += nread;
      }
      else {
	memcpy(buf + nread_total, buffer, sz);
	nread_total += sz;

	/* reload the file's buffer cache and update metadata */
	memcpy(f->buf, buffer + sz, nread - sz);
	f->pos = 0;
	f->abs += sz;
	f->sz = (nread - sz);
      }
    }

    /* if there was a problem s.t. you couldn't read anything in, but you
     * were supposed to (sz != 0) and you're not at the EOF */
    if (nread_total || !sz || io61_eof(f))
      return nread_total;
    else
      return -1;
}


// io61_writec(f)
//    Write a single character `ch` to `f`. Returns 0 on success or
//    -1 on error.

int io61_writec(io61_file* f, int ch) {
    /* you want to compare f->pos since even if f->sz equals BLOCK, if
     * f->pos < BLOCK you can still edit the buffer */
    if(!(BLOCK ^ f->pos)) {
      if(flush_wbuf(f) != BLOCK)
        return -1;

      f->abs += BLOCK;
      f->pos = 0;

      linear_insert(f, f->pos, 1);
      f->buf[f->pos++] = ch;
    }
    else {
      linear_insert(f, f->pos, 1);
      f->buf[f->pos++] = ch;
    }

    return 0;
}


// io61_write(f, buf, sz)
//    Write `sz` characters from `buf` to `f`. Returns the number of
//    characters written on success; normally this is `sz`. Returns -1 if
//    an error occurred before any characters were written.

ssize_t io61_write(io61_file* f, const char* buf, size_t sz) {
    size_t nwritten = 0;

    if(sz <= (BLOCK - f->pos)) {
      memcpy(f->buf + f->pos, buf, sz);
      int last = wbuf_index(f->wsz);
      binary_insert(f, f->pos, sz, 0, last, none);
      f->pos += sz;

      nwritten = sz;
     } else {
      int cur_pos = f->abs + f->pos;
      int temp = flush_wbuf(f);
      lseek(f->fd, cur_pos, SEEK_SET);
      temp += write(f->fd, buf, sz);

      nwritten += temp;
      f->abs += sz + f->pos;
      f->pos = 0;
    }

    if (nwritten || !sz) {
      return nwritten;
    } else
      return -1;
}


// io61_flush(f)
//    Forces a write of all buffered data written to `f`.
//    If `f` was opened read-only, io61_flush(f) may either drop all
//    data buffered for reading, or do nothing.

int io61_flush(io61_file* f) {
    /* I'm assuming file modes are values and a mode like O_RDWR is the bitwise OR of O_RDONLY
     * and O_WRONLY. If this is the case, bitwise AND of mode with O_WRONLY should check if file
     * was opened in any mode capable or writing */
    if(f->mode & O_WRONLY) {
      int nwritten = flush_wbuf(f);

      if(nwritten)
        return 0;
      else
        return -1;
      }

    return 0;
}


// io61_seek(f, pos)
//    Change the file pointer for file `f` to `pos` bytes into the file.
//    Returns 0 on success and -1 on failure.

int io61_seek(io61_file* f, off_t pos) {
    /* the big problem with this approach is it doesn't take into account files where
     * you might be switching between reads and writes. i'm not sure if this comes up
     * in the tests, but it makes this version much less robust. */
    off_t r = lseek(f->fd, (off_t) pos, SEEK_SET);
    if (r == (off_t) pos) {

      /* let's handle the read buffer */
      if(f->mode == O_RDONLY) {
        if((size_t) pos >= f->abs && (size_t) pos <= f->abs + f->sz) {
	  f->pos = pos - f->abs;
	  /* this one line of code protects against scenarios where using the buffer
	   * overlaps with the current position of future reads -- this caused issues
	   * in io61_read() */
	  lseek(f->fd, f->abs + f->sz, SEEK_SET);
	} else {
	  f->pos = 0;
	  f->sz = 0;
	  f->abs = pos;
	}
      }

      /* let's now deal with the write buffer */
      if(f->mode & O_WRONLY) {
        if((size_t) pos >= f->abs && (size_t) pos <= f->abs + BLOCK) {
	  f->pos = pos - f->abs;
        } else {
	  flush_wbuf(f);
	  f->pos = 0;
	  f->sz = 0;
	  f->abs = pos;
	}
      }

      return 0;
    }

    return -1;
}


// You shouldn't need to change these functions.

// io61_open_check(filename, mode)
//    Open the file corresponding to `filename` and return its io61_file.
//    If `filename == NULL`, returns either the standard input or the
//    standard output, depending on `mode`. Exits with an error message if
//    `filename != NULL` and the named file cannot be opened.

io61_file* io61_open_check(const char* filename, int mode) {
    int fd;
    if (filename)
        fd = open(filename, mode, 0666);
    else if ((mode & O_ACCMODE) == O_RDONLY)
        fd = STDIN_FILENO;
    else
        fd = STDOUT_FILENO;
    if (fd < 0) {
        fprintf(stderr, "%s: %s\n", filename, strerror(errno));
        exit(1);
    }
    return io61_fdopen(fd, mode & O_ACCMODE);
}


// io61_filesize(f)
//    Return the size of `f` in bytes. Returns -1 if `f` does not have a
//    well-defined size (for instance, if it is a pipe).

off_t io61_filesize(io61_file* f) {
    struct stat s;
    int r = fstat(f->fd, &s);
    if (r >= 0 && S_ISREG(s.st_mode))
        return s.st_size;
    else
        return -1;
}


// io61_eof(f)
//    Test if readable file `f` is at end-of-file. Should only be called
//    immediately after a `read` call that returned 0 or -1.

int io61_eof(io61_file* f) {
    char x;
    ssize_t nread = read(f->fd, &x, 1);
    if (nread == 1) {
        fprintf(stderr, "Error: io61_eof called improperly\n\
  (Only call immediately after a read() that returned 0 or -1.)\n");
        abort();
    }
    return nread == 0;
}
