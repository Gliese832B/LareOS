#ifndef VFS_H
#define VFS_H

#include "lareos.h"

#define VFS_MAX_NAME    32
#define VFS_MAX_PATH    256
#define VFS_MAX_OPEN    32

#define VFS_FILE        0
#define VFS_DIRECTORY   1
#define VFS_DEVICE      2
#define VFS_SYMLINK     3

#define VFS_O_READ      0x01
#define VFS_O_WRITE     0x02
#define VFS_O_CREATE    0x04
#define VFS_O_APPEND    0x08
#define VFS_O_TRUNC     0x10

typedef struct vfs_node {
    char name[VFS_MAX_NAME];
    uint8_t type;
    uint32_t size;
    uint8_t *data;
    uint32_t capacity;
    struct vfs_node *parent;
    struct vfs_node *children;
    struct vfs_node *next;
    uint64_t created;
    uint64_t modified;
    uint32_t permissions;
    ssize_t (*read_fn)(struct vfs_node *node, void *buf, size_t size, size_t offset);
    ssize_t (*write_fn)(struct vfs_node *node, const void *buf, size_t size, size_t offset);
} vfs_node_t;

typedef struct {
    vfs_node_t *node;
    uint32_t offset;
    uint32_t flags;
    bool in_use;
} vfs_fd_t;

void vfs_init(void);
vfs_node_t *vfs_get_root(void);
vfs_node_t *vfs_get_cwd(void);
void vfs_set_cwd(vfs_node_t *dir);

vfs_node_t *vfs_create(vfs_node_t *parent, const char *name, uint8_t type);
int vfs_remove(vfs_node_t *node);
vfs_node_t *vfs_find_child(vfs_node_t *parent, const char *name);
vfs_node_t *vfs_resolve_path(const char *path);
void vfs_get_path(vfs_node_t *node, char *buf, size_t size);

ssize_t vfs_read(vfs_node_t *node, void *buf, size_t size, size_t offset);
ssize_t vfs_write(vfs_node_t *node, const void *buf, size_t size, size_t offset);

int vfs_open(const char *path, uint32_t flags);
ssize_t vfs_fd_read(int fd, void *buf, size_t size);
ssize_t vfs_fd_write(int fd, const void *buf, size_t size);
int vfs_fd_close(int fd);

uint32_t vfs_count_children(vfs_node_t *dir);

#endif
