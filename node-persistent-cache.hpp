#ifndef NODE_PERSISTENT_CACHE_H
#define NODE_PERSISTENT_CACHE_H

#include "node-ram-cache.hpp"
#include <boost/shared_ptr.hpp>

#define MAXIMUM_INITIAL_ID 2600000000

#define READ_NODE_CACHE_SIZE 10000
#define READ_NODE_BLOCK_SHIFT 10l
#define READ_NODE_BLOCK_SIZE (1l << READ_NODE_BLOCK_SHIFT)
#define READ_NODE_BLOCK_MASK 0x03FFl

#define WRITE_NODE_BLOCK_SHIFT 20l
#define WRITE_NODE_BLOCK_SIZE (1l << WRITE_NODE_BLOCK_SHIFT)
#define WRITE_NODE_BLOCK_MASK 0x0FFFFFl

#define PERSISTENT_CACHE_FORMAT_VERSION 1

struct persistentCacheHeader {
	int format_version;
	int id_size;
    osmid_t max_initialised_id;
};

struct node_persistent_cache : public boost::noncopyable
{
    node_persistent_cache(const struct options_t *options, const int append,
                          boost::shared_ptr<node_ram_cache> ptr);
    ~node_persistent_cache();

    int set(osmid_t id, double lat, double lon);
    int get(struct osmNode *out, osmid_t id);
    int get_list(struct osmNode *nodes, const osmid_t *ndids, int nd_count);

private:

    int set_append(osmid_t id, double lat, double lon);
    int set_create(osmid_t id, double lat, double lon);

    void writeout_dirty_nodes(osmid_t id);
    int replace_block();
    int find_block(osmid_t block_offset);
    void expand_cache(osmid_t block_offset);
    void nodes_prefetch_async(osmid_t id);
    int load_block(osmid_t block_offset);
    void nodes_set_create_writeout_block();

    int node_cache_fd;
    const char * node_cache_fname;
    int append_mode;
    
    struct persistentCacheHeader cacheHeader;
    struct ramNodeBlock writeNodeBlock; /* larger node block for more efficient initial sequential writing of node cache */
    struct ramNodeBlock * readNodeBlockCache;
    struct binary_search_array * readNodeBlockCacheIdx;
    
    int scale_;
    int cache_already_written;

    boost::shared_ptr<node_ram_cache> ram_cache;
};

#ifdef _MSC_VER
/* Emulate fsync on platforms that lack it, primarily Windows and
   cross-compilers like MinGW.

   This is derived from sqlite3 sources.
   http://www.sqlite.org/cvstrac/rlog?f=sqlite/src/os_win.c
   http://www.sqlite.org/copyright.html

   Written by Richard W.M. Jones <rjones.at.redhat.com>

   Copyright (C) 2008-2014 Free Software Foundation, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <windows.h>
#include <io.h>
#include <errno.h>

inline int fsync (int fd)
{
  HANDLE h = (HANDLE) _get_osfhandle (fd);
  DWORD err;

  if (h == INVALID_HANDLE_VALUE)
    {
      errno = EBADF;
      return -1;
    }

  if (!FlushFileBuffers (h))
    {
      /* Translate some Windows errors into rough approximations of Unix
       * errors.  MSDN is useless as usual - in this case it doesn't
       * document the full range of errors.
       */
      err = GetLastError ();
      switch (err)
        {
        case ERROR_ACCESS_DENIED:
          /* For a read-only handle, fsync should succeed, even though we have
             no way to sync the access-time changes.  */
          return 0;

          /* eg. Trying to fsync a tty. */
        case ERROR_INVALID_HANDLE:
          errno = EINVAL;
          break;

        default:
          errno = EIO;
        }
      return -1;
    }

  return 0;
}
#endif
#endif
