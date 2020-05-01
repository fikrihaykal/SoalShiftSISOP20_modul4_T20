#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

static const char *rootDir = "/home/fikri/Documents";
static const char *key = {"9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO"};
static const char *logsys = "/home/fikri/Modul4/fs.txt";

char enkripsiSatu[10] = "encv1_";

void encryptDecrypt(char *path, int method);
void catatLog(char *lv, char *command, int res, int lenDesc, const char *desc[]);
int findAwal(char *path, int st);
int findAkhir(char *path);

static int xmp_getattr(const char *path, struct stat * stbuf);
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int xmp_mkdir(const char *path, mode_t mode);
static int xmp_rename(const char *from, const char *to);
static int xmp_rmdir(const char *path);
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int xmp_mknod(const char *path, mode_t mode, dev_t rdev);
static int xmp_unlink(const char *path);
static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int xmp_truncate(const char *path, off_t size);
static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi);
static int xmp_utimens(const char *path, const struct timespec ts[2]);
static int xmp_access(const char *path, int mask);
static int xmp_open(const char *path, struct fuse_file_info *fi);
static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .mkdir = xmp_mkdir,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .truncate = xmp_truncate,
    .write = xmp_write,
    .create = xmp_create,
    .utimens = xmp_utimens,
    .access = xmp_access,
    .open = xmp_open,
    .mknod = xmp_mknod,
    .unlink = xmp_unlink,
};

int main(int argc, char *argv[]){
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}

static int xmp_getattr(const char *path, struct stat * stbuf){
    int res = 0;
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = lstat(fpath, stbuf);

    const char *desc[] = {fpath};
    catatLog("INFO", "GETATTR", res, 1, desc);
    
    if(res == -1){
        return -errno;
    }

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;
    int res = 0;

    dp = opendir(fpath);

    if(dp == NULL){
        return -errno;
    }

    while((de = readdir(dp)) != NULL){
        struct stat st;
        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        printf("Before: %s\n", de->d_name);

        if(dirTemp != NULL){
            encryptDecrypt(de->d_name, 1);
        }

        printf("After: %s\n", de->d_name);

        res = (filler(buf, de->d_name, &st, 0));

        if(res != 0){
            break;
        }
    }

    closedir(dp);

    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    int res=0;
    res = mkdir(fpath, mode);
    
    if(res == -1){
        return -errno;
    }

    const char *desc[] = {fpath};
    catatLog("INFO", "MKDIR", res, 1, desc);

    return 0;
}

static int xmp_rename(const char *from, const char *to){
    int res;
    char fpath[1000], fpathf[1000];

    char *dirTemp = strstr(to, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    sprintf(fpath, "%s%s", rootDir, from);
    sprintf(fpathf, "%s%s", rootDir, to);

    res = rename(fpath, fpathf);

    if(res == -1){
        return -errno;
    }

    const char *desc[] = {fpath, fpathf};
    catatLog("INFO", "RENAME", res, 2, desc);

    return 0;
}

static int xmp_rmdir(const char *path){
    int res;
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = rmdir(fpath);

    if(res == -1){
        return -errno;
    }

    const char *desc[] = {fpath};
    catatLog("WARNING", "RMDIR", res, 1, desc);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    int res, fd;
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
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
        res = -errno;
    }

    close(fd);

    return res;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){
    int res;
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    if(S_ISREG(mode)){
        res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
        if(res >= 0){
            res = close(res);
        }
    } else if(S_ISFIFO(mode)){
        res = mkfifo(path, mode);
    } else {
        res = mknod(path, mode, rdev);
    }

    if(res == -1){
        return -errno;
    }
    
    const char *desc[] = {fpath};
    catatLog("INFO", "MKNODE", res, 1, desc);

    return 0;
}

static int xmp_unlink(const char *path){
    int res;
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = unlink(fpath);

    if(res == -1){
        return -errno;
    }
    
    const char *desc[] = {fpath};
    catatLog("WARNING", "UNLINK", res, 1, desc);

    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }
    
    (void) fi;
    int fd, res;
    
    fd = open(fpath, O_WRONLY);

    if(fd == -1){
        return -errno;
    }
    
    res = pwrite(fd, buf, size, offset);

    if(res == -1){
        res = -errno;
    }
    
    close(fd);
    
    return res;
}

static int xmp_truncate(const char *path, off_t size){
    int res;
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = truncate(fpath, size);
    
    if(res == -1){
        return -errno;
    }

    return 0;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi){
    int res;
    char fpath[1000];
    (void)fi;

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = creat(fpath, mode);
    
    if(res == -1){
        return -errno;
    }

    close(res);

    return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2]){
    int res;
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

    res = utimes(fpath, tv);
    
    if(res == -1){
        return -errno;
    }

    return 0;
}

static int xmp_access(const char *path, int mask){
    int res;
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = access(fpath, mask);
    
    if(res == -1){
        return -errno;
    }

    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){
    int res;
    char fpath[1000];

    char *dirTemp = strstr(path, enkripsiSatu);
    if(dirTemp != NULL){
        encryptDecrypt(dirTemp, 0);
    }

    if(strcmp(path, "/") == 0){
        sprintf(fpath, "%s", rootDir);
    } else{
        sprintf(fpath, "%s%s", rootDir, path);
    }

    res = access(fpath, fi->flags);
    
    if(res == -1){
        return -errno;
    }

    close(res);
    return 0;
}

void encryptDecrypt(char *path, int method){
    if(!strcmp(path, ".") || !strcmp(path, "..")){
        return;
    }

    int cnt, sw, charAwal, charAkhir;
    
    charAkhir = findAkhir(path);

    if(method == 1){
        sw = 10;
        charAwal = findAwal(path, 0);
    } else if(method == 0){
        sw = strlen(key) - 10;
        charAwal = findAwal(path, charAkhir);
    }

    for(int i=charAwal; i<charAkhir; i++){
        if(path[i] != '/'){
            for(int j=0; j<strlen(key); j++){
                if(key[j] == path[i]){
                    cnt = (j+sw) % strlen(key);
                    path[i] = key[cnt];

                    break;
                }
            }
        }
    }
}

void catatLog(char *lv, char *command, int res, int lenDesc, const char *desc[]){
    FILE *file = fopen(logsys, "a+");
    time_t t;
    struct tm *tm;
    char timeTemp[100];
    time(&t);
    tm = localtime(&t);
    
    strftime(timeTemp, sizeof(timeTemp), "%y%m%d-%H:%M:%S", tm);

    char logTemp[1000];
    sprintf(logTemp, "%s::%s::%s::%d", lv, timeTemp, command, res);
    
    for(int i=0; i<lenDesc; i++){
        sprintf(logTemp, "%s::%s", logTemp, desc[i]);
    }

    sprintf(logTemp, "%s\n", logTemp);

    fputs(logTemp, file);

    fclose(file);
}

int findAwal(char *path, int st){
    int pLen = strlen(path);

    for(int i =0; i<pLen; i++){
        if(path[i] == '/'){
            return i+1;
        }
    }
    return st;
}

int findAkhir(char *path){
    int pLen = strlen(path);

    for(int i=pLen-1; i>=0; i--){
        if(path[i] == '.'){
            return i;
        }
    }
    return pLen;
}