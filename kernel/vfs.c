#include "vfs.h"
#include "mm.h"
#include "string.h"
#include "timer.h"
#include "power.h"
#include "printf.h"

static vfs_node_t *root = NULL;
static vfs_node_t *cwd = NULL;
static vfs_fd_t fd_table[VFS_MAX_OPEN];

static vfs_node_t *alloc_node(void) {
    vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
    if (node) memset(node, 0, sizeof(vfs_node_t));
    return node;
}

static ssize_t dev_null_read(vfs_node_t *node, void *buf, size_t size, size_t offset) {
    UNUSED(node); UNUSED(buf); UNUSED(size); UNUSED(offset);
    return 0;
}

static ssize_t dev_null_write(vfs_node_t *node, const void *buf, size_t size, size_t offset) {
    UNUSED(node); UNUSED(buf); UNUSED(offset);
    return (ssize_t)size;
}

static ssize_t dev_zero_read(vfs_node_t *node, void *buf, size_t size, size_t offset) {
    UNUSED(node); UNUSED(offset);
    memset(buf, 0, size);
    return (ssize_t)size;
}

static ssize_t dev_random_read(vfs_node_t *node, void *buf, size_t size, size_t offset) {
    UNUSED(node); UNUSED(offset);
    uint8_t *p = (uint8_t *)buf;
    uint64_t seed = timer_get_ticks();
    for (size_t i = 0; i < size; i++) {
        seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
        p[i] = (uint8_t)(seed >> 33);
    }
    return (ssize_t)size;
}

static ssize_t proc_uptime_read(vfs_node_t *node, void *buf, size_t size, size_t offset) {
    UNUSED(node); UNUSED(offset);
    char tmp[64];
    uint64_t s = timer_get_uptime_seconds();
    ksprintf(tmp, "%u seconds\n", s);
    size_t len = strlen(tmp);
    if (size > len) size = len;
    memcpy(buf, tmp, size);
    return (ssize_t)size;
}

static ssize_t proc_meminfo_read(vfs_node_t *node, void *buf, size_t size, size_t offset) {
    UNUSED(node); UNUSED(offset);
    mem_info_t info = mm_get_info();
    char tmp[256];
    ksprintf(tmp, "Total:  %u KB\nUsed:   %u KB\nFree:   %u KB\nPages:  %u / %u\n",
        (uint32_t)(info.total / 1024),
        (uint32_t)(info.used / 1024),
        (uint32_t)(info.free / 1024),
        info.pages_used, info.pages_total);
    size_t len = strlen(tmp);
    if (size > len) size = len;
    memcpy(buf, tmp, size);
    return (ssize_t)size;
}

static ssize_t proc_cpuinfo_read(vfs_node_t *node, void *buf, size_t size, size_t offset) {
    UNUSED(node); UNUSED(offset);
    system_power_t pwr = power_get_status();
    char tmp[256];
    ksprintf(tmp, "Architecture: AArch64\nARM Clock:    %u MHz\nCore Clock:   %u MHz\nTemperature:  %u C\nProfile:      %s\n",
        pwr.arm_clock / 1000000,
        pwr.core_clock / 1000000,
        pwr.cpu_temp / 1000,
        power_get_profile_name(pwr.current_profile));
    size_t len = strlen(tmp);
    if (size > len) size = len;
    memcpy(buf, tmp, size);
    return (ssize_t)size;
}

static ssize_t proc_version_read(vfs_node_t *node, void *buf, size_t size, size_t offset) {
    UNUSED(node); UNUSED(offset);
    char tmp[128];
    ksprintf(tmp, "LareOS %u.%u.%u (%s) AArch64\n",
        LAREOS_VERSION_MAJOR, LAREOS_VERSION_MINOR, LAREOS_VERSION_PATCH,
        LAREOS_CODENAME);
    size_t len = strlen(tmp);
    if (size > len) size = len;
    memcpy(buf, tmp, size);
    return (ssize_t)size;
}

vfs_node_t *vfs_create(vfs_node_t *parent, const char *name, uint8_t type) {
    if (!parent || parent->type != VFS_DIRECTORY) return NULL;
    if (vfs_find_child(parent, name)) return NULL;

    vfs_node_t *node = alloc_node();
    if (!node) return NULL;

    strncpy(node->name, name, VFS_MAX_NAME - 1);
    node->type = type;
    node->parent = parent;
    node->created = timer_get_ticks();
    node->modified = node->created;
    node->permissions = 0755;

    node->next = parent->children;
    parent->children = node;

    return node;
}

int vfs_remove(vfs_node_t *node) {
    if (!node || node == root) return -1;
    if (node->type == VFS_DIRECTORY && node->children) return -1;

    vfs_node_t *parent = node->parent;
    if (!parent) return -1;

    vfs_node_t **pp = &parent->children;
    while (*pp) {
        if (*pp == node) {
            *pp = node->next;
            break;
        }
        pp = &(*pp)->next;
    }

    if (node->data) kfree(node->data);
    kfree(node);
    return 0;
}

vfs_node_t *vfs_find_child(vfs_node_t *parent, const char *name) {
    if (!parent || parent->type != VFS_DIRECTORY) return NULL;

    if (strcmp(name, ".") == 0) return parent;
    if (strcmp(name, "..") == 0) return parent->parent ? parent->parent : parent;

    vfs_node_t *child = parent->children;
    while (child) {
        if (strcmp(child->name, name) == 0) return child;
        child = child->next;
    }
    return NULL;
}

vfs_node_t *vfs_resolve_path(const char *path) {
    if (!path || !*path) return cwd;

    vfs_node_t *node = (*path == '/') ? root : cwd;
    if (*path == '/') path++;

    char component[VFS_MAX_NAME];
    while (*path) {
        int i = 0;
        while (*path && *path != '/' && i < VFS_MAX_NAME - 1) {
            component[i++] = *path++;
        }
        component[i] = '\0';
        if (*path == '/') path++;
        if (i == 0) continue;

        node = vfs_find_child(node, component);
        if (!node) return NULL;
    }
    return node;
}

void vfs_get_path(vfs_node_t *node, char *buf, size_t size) {
    if (!node || !buf || size == 0) return;

    if (node == root) {
        strncpy(buf, "/", size);
        return;
    }

    char tmp[VFS_MAX_PATH];
    tmp[0] = '\0';
    vfs_node_t *parts[32];
    int depth = 0;

    vfs_node_t *n = node;
    while (n && n != root && depth < 32) {
        parts[depth++] = n;
        n = n->parent;
    }

    buf[0] = '\0';
    for (int i = depth - 1; i >= 0; i--) {
        strcat(buf, "/");
        strcat(buf, parts[i]->name);
    }
    if (buf[0] == '\0') strncpy(buf, "/", size);
}

ssize_t vfs_read(vfs_node_t *node, void *buf, size_t size, size_t offset) {
    if (!node || !buf) return -1;

    if (node->read_fn) return node->read_fn(node, buf, size, offset);

    if (node->type != VFS_FILE) return -1;
    if (offset >= node->size) return 0;

    size_t avail = node->size - offset;
    if (size > avail) size = avail;
    memcpy(buf, node->data + offset, size);
    return (ssize_t)size;
}

ssize_t vfs_write(vfs_node_t *node, const void *buf, size_t size, size_t offset) {
    if (!node || !buf) return -1;

    if (node->write_fn) return node->write_fn(node, buf, size, offset);

    if (node->type != VFS_FILE) return -1;

    size_t needed = offset + size;
    if (needed > node->capacity) {
        size_t new_cap = needed * 2;
        if (new_cap < 256) new_cap = 256;
        uint8_t *new_data = (uint8_t *)kmalloc(new_cap);
        if (!new_data) return -1;
        if (node->data) {
            memcpy(new_data, node->data, node->size);
            kfree(node->data);
        }
        node->data = new_data;
        node->capacity = new_cap;
    }

    memcpy(node->data + offset, buf, size);
    if (offset + size > node->size) node->size = offset + size;
    node->modified = timer_get_ticks();
    return (ssize_t)size;
}

int vfs_open(const char *path, uint32_t flags) {
    vfs_node_t *node = vfs_resolve_path(path);

    if (!node && (flags & VFS_O_CREATE)) {
        char pathcopy[VFS_MAX_PATH];
        strncpy(pathcopy, path, VFS_MAX_PATH);

        char *last_slash = NULL;
        for (char *p = pathcopy; *p; p++) {
            if (*p == '/') last_slash = p;
        }

        vfs_node_t *parent;
        const char *filename;
        if (last_slash) {
            *last_slash = '\0';
            parent = vfs_resolve_path(pathcopy);
            filename = last_slash + 1;
        } else {
            parent = cwd;
            filename = path;
        }

        if (!parent) return -1;
        node = vfs_create(parent, filename, VFS_FILE);
        if (!node) return -1;
    }

    if (!node) return -1;

    for (int i = 0; i < VFS_MAX_OPEN; i++) {
        if (!fd_table[i].in_use) {
            fd_table[i].node = node;
            fd_table[i].offset = (flags & VFS_O_APPEND) ? node->size : 0;
            fd_table[i].flags = flags;
            fd_table[i].in_use = true;
            if (flags & VFS_O_TRUNC) {
                node->size = 0;
            }
            return i;
        }
    }
    return -1;
}

ssize_t vfs_fd_read(int fd, void *buf, size_t size) {
    if (fd < 0 || fd >= VFS_MAX_OPEN || !fd_table[fd].in_use) return -1;
    ssize_t ret = vfs_read(fd_table[fd].node, buf, size, fd_table[fd].offset);
    if (ret > 0) fd_table[fd].offset += ret;
    return ret;
}

ssize_t vfs_fd_write(int fd, const void *buf, size_t size) {
    if (fd < 0 || fd >= VFS_MAX_OPEN || !fd_table[fd].in_use) return -1;
    ssize_t ret = vfs_write(fd_table[fd].node, buf, size, fd_table[fd].offset);
    if (ret > 0) fd_table[fd].offset += ret;
    return ret;
}

int vfs_fd_close(int fd) {
    if (fd < 0 || fd >= VFS_MAX_OPEN || !fd_table[fd].in_use) return -1;
    fd_table[fd].in_use = false;
    return 0;
}

uint32_t vfs_count_children(vfs_node_t *dir) {
    if (!dir || dir->type != VFS_DIRECTORY) return 0;
    uint32_t count = 0;
    vfs_node_t *child = dir->children;
    while (child) {
        count++;
        child = child->next;
    }
    return count;
}

vfs_node_t *vfs_get_root(void) { return root; }
vfs_node_t *vfs_get_cwd(void) { return cwd; }
void vfs_set_cwd(vfs_node_t *dir) { if (dir && dir->type == VFS_DIRECTORY) cwd = dir; }

static vfs_node_t *create_device(vfs_node_t *parent, const char *name,
    ssize_t (*rfn)(vfs_node_t*, void*, size_t, size_t),
    ssize_t (*wfn)(vfs_node_t*, const void*, size_t, size_t)) {
    vfs_node_t *node = vfs_create(parent, name, VFS_DEVICE);
    if (node) {
        node->read_fn = rfn;
        node->write_fn = wfn;
    }
    return node;
}

void vfs_init(void) {
    memset(fd_table, 0, sizeof(fd_table));

    root = alloc_node();
    strncpy(root->name, "/", VFS_MAX_NAME);
    root->type = VFS_DIRECTORY;
    root->parent = root;
    root->created = timer_get_ticks();
    root->modified = root->created;
    root->permissions = 0755;

    cwd = root;

    vfs_node_t *dev = vfs_create(root, "dev", VFS_DIRECTORY);
    create_device(dev, "null", dev_null_read, dev_null_write);
    create_device(dev, "zero", dev_zero_read, dev_null_write);
    create_device(dev, "random", dev_random_read, NULL);

    vfs_node_t *proc = vfs_create(root, "proc", VFS_DIRECTORY);
    create_device(proc, "uptime", proc_uptime_read, NULL);
    create_device(proc, "meminfo", proc_meminfo_read, NULL);
    create_device(proc, "cpuinfo", proc_cpuinfo_read, NULL);
    create_device(proc, "version", proc_version_read, NULL);

    vfs_create(root, "tmp", VFS_DIRECTORY);
    vfs_create(root, "home", VFS_DIRECTORY);
    vfs_create(root, "etc", VFS_DIRECTORY);
    vfs_create(root, "var", VFS_DIRECTORY);

    vfs_node_t *etc = vfs_find_child(root, "etc");
    vfs_node_t *hostname = vfs_create(etc, "hostname", VFS_FILE);
    if (hostname) vfs_write(hostname, "lareos", 6, 0);

    vfs_node_t *motd = vfs_create(etc, "motd", VFS_FILE);
    if (motd) {
        const char *msg = "Welcome to LareOS 2.0.0 (Phoenix)\n";
        vfs_write(motd, msg, strlen(msg), 0);
    }
}
