#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char * rootDir = "/home/fikri/Documents";

static int xmp_getattr(const char *path, struct stat * stbuf);
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int xmp_open(const char *path, struct fuse_file_info *fi);
static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi);
static int xmp_truncate(const char *path, off_t size);
static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .open = xmp_open,
    .write = xmp_write,
    .create = xmp_create,
    .truncate = xmp_truncate,
};


int main(int argc, char *argv[]){
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}


static int xmp_getattr(const char *path, struct stat * stbuf){
    int res;
    char fpath[1000];

    if(strcmp(path, "/") == 0){
        path = rootDir;
        sprintf(fpath, "%s", path);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = lstat(fpath, stbuf);
    
    if(res == -1){
        return -errno;
    }

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
    int res;
    char fpath[1000];

    if(strcmp(path, "/") == 0){
        path = rootDir;
        sprintf(fpath, "%s", path);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(fpath);

    if(dp == NULL){
        return -errno;
    }

    while((de = readdir(dp)) != NULL){
        struct stat st;
        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        res = (filler(buf, de->d_name, &st, 0));

        if(res != 0){
            break;
        }
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    int res, fd;
    char fpath[1000];

    if(strcmp(path, "/") == 0){
        path = rootDir;
        sprintf(fpath, "%s", path);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = 0;
    fd = 0;
    (void) fi;

    fd = open(fpath, O_RDONLY);

    if(fd == -1){
        return -errno;
    }

    res = pread(fd, buf, size, offset);

    if(res == -1){
        res = - errno;
    }

    close(fd);

    return res;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){
    int res;
    char fpath[1000];

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = open(fpath, fi->flags);

    if(res == -1){
        return -errno;
    }

    fi->fh = res;
    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    char fpath[1000];
    char fpath2[1000];
    char fname[1000];
    char fext[1000];
    char fbackup[1000];

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }
    
    (void) fi;

    sprintf(fbackup, "%s/backup", rootDir);
    int backupfd = open(fbackup, O_RDONLY);
    
    if(backupfd == -1){
        mkdir(fbackup, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    } else{
        close(backupfd);
    }

    memset(fname, 0, sizeof(fname));
    memset(fext, 0, sizeof(fext));
    getNamaFile(fname, fext, fpath);

    time_t t;
    char ftime[1000];
    struct tm* tm_info;
    t = time(NULL);
    tm_info = localtime(&t);
    strftime(ftime, 1000, "%H:%M:%S_%d-%m-%Y", tm_info);

    if(strcmp(fext, "") == 0){
        sprintf(fpath2, "%s/backup/%s.%s", rootDir, fname, ftime);
    } else{
        sprintf(fpath2, "%s/backup/%s.%s.%s", rootDir, fname, ftime, fext);
    }

    int fd1, fd2, res;
    
    if(fi == NULL){
        fd1 = open(fpath, O_WRONLY);
    } else{
        fd1 = fi->fh;
    }

    fd2 = open(fpath2, O_WRONLY | O_CREAT);

    res = pwrite(fd1, buf, size, offset);
    pwrite(fd2, buf, size, offset);
    chmod(fpath2, 0);

    if(fi == NULL){
        close(fd1);
    }
    
    close(fd2);

    if(res == -1){
        res = -errno;
    }
    
    return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi){
    int res;
    char fpath[1000];

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = open(fpath, fi->flags, mode);

    if(res == -1){
        return -errno;
    }

    fi->fh = res;
    return 0;  
}

static int xmp_truncate(const char *path, off_t size){
    int res;
    char fpath[1000];

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = truncate(fpath, size);

    if(res == -1){
        return -errno;
    }

    return res;
}
