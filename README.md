# SoalShiftSISOP20_modul4_T20

Praktikum Sistem Operasi 2020 - Modul 4
Departemen Teknologi Informasi

Kelompok T20

Anggota :

- Fikri Haykal
- Hana Ghaliyah Azhar

## SOAL
### SOAL 1
Enkripsi versi 1: <br />
- Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
- Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
- Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip.
- Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
- Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key. <br />
```9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO``` <br />
Misal kan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10 <br />
<b> “encv1_rahasia/FOTO_PENTING/kelincilucu.jpg” => </b> 
<b> “encv1_rahasia/ULlL@u]AlZA(/g7D.|_.Da_a.jpg </b> <br />
<b> Note </b>: Dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di encrypt.
- Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya.

### Penyelesaian SOAL 1
- Masuk ke directory soal <br />
Syntax : `cd Documents/modul4/soal`
- Membuat file <b>ssfs.c</b> yang berisi program FUSE<br />
Syntax : `nano ssfs.c` <br />
___source code : [ssfs.c](https://github.com/fikrihaykal/SoalShiftSISOP20_modul4_T20/blob/master/ssfs.c)___ <br />
- Compile FUSE <br />
Perintah ``` gcc -Wall `pkg-config fuse --cflags` [file.c] -o [output] `pkg-config fuse --libs` ``` digunakan untuk mengcompile program FUSE. <br />
Syntax : ``` gcc -Wall `pkg-config fuse --cflags` ssfs.c -o ssfs `pkg-config fuse --libs` ``` <br />
- Membuat sebuah direktori <br />
Perintah `mkdir [direktori tujuan]` digunakan sebagai tujuan pembuatan FUSE <br />
Disini kami membuat direktori bernama <b> encv1_ </b>
- Menjalankan FUSE <br />
Perintah `./[output] [direktori tujuan]` digunakan untuk menjalankan FUSE pada direktori yang telah dibuat. <br />
Untuk menjalankannya kami menggunakan perintah `./ssfs encv1_`

#### Penjelasan Source Code
Untuk mendukung pembuatan program mengenai metode enkripsi tersebut, maka kita harus membuat fungsi `system call` yang bertujuan untuk mengatur 3 (tiga) keperluan tertentu sesuai permintaan soal :
- `mkdir`  : kondisi dimana terjadi pembuatan directory yang telah ditentukan.
- `create` : kondisi dimana terjadi proses rename pada directory sesuai dengan nama yang telah ditentukan.
- `write`  : kondisi dimana terjadi penyimpanan perubahan pada database/log pada file.
<br />

List library yang akan digunakan pada program ini
```
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
```

Fungsi ini digunakan agar FUSE dapat menampilkan apa yang ada di dalam folder `/home/fikri/Documents` <br />
```static const char *rootDir = "/home/fikri/Documents";```

Fungsi ini akan melakukan input key untuk enkripsi caesar chiper pada file yang ada pada suatu direktori dengan direktori `encv1_` <br />
```
static const char *key = {"9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO"};
```

Fungsi dibawah ini digunakan untuk mengenkripsi sebuah direktori yang dibuat dengan awalan `encv1_` <br />
```
char enkripsiSatu[10] = "encv1_";
```

Pendeklarasian fungsi `encryptDecrypt, catatLog, findAwal dan findAkhir`
```
void encryptDecrypt(char *path, int method);
void catatLog(char *lv, char *command, int res, int lenDesc, const char *desc[]);
int findAwal(char *path, int st);
int findAkhir(char *path);
```

Pada program ini memiliki `struct fuse_operations` yang jika dipanggil akan menjalankan fungsinya, `fuse_operations` didefinisikan seperti dibawah:
```
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
```
- Semua atribut pada `struct` tersebut adalah pointer yang menuju ke fungsi. Setiap fungsi tersebut disebut FUSE saat suatu kejadian yang spesifik terjadi di file system. Sebagai contoh saat user menulis di sebuah file, sebuah fungsi yang ditunjuk oleh atribut "write" di `struct` akan terpanggil. 
- Selain itu, atribut pada `struct` tersebut tertulis seperti fungsi yang biasa digunakan di linux. Contohnya ialah saat kita membuat directory di FUSE maka fungsi mkdir akan dipanggil.
<br />

Fungsi `fuse_main()` (lib/helper.c) = sebagai fungsi _main_ (userspace), program user memanggil fungsi fuse_main() kemudian fungsi fuse_mount() dipanggil.
```
int main(int argc, char *argv[]){
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
```
<br />

Fungsi `getattr` digunakan untuk Mendapatkan atribut file. <br />
Fungsi `getattr` yang dipanggil saat sistem mencoba untuk mendapatkan atribut dari sebuah file. 
```
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
```
<br />

Fungsi `readdir` digunakan untuk Membaca directory. <br />
Fungsi `readdir` yang dipanggil saat user mencoba untuk menampilkan file dan direktori yang berada pada suatu direktori yang spesifik.
```
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
```
<br />

Fungsi `mkdir` digunakan untuk membuat sebuah directory
```
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
```
<br />

Fungsi `rename` digunakan untuk mengganti nama file maupun folder.
```
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
```
<br />

Fungsi `rmdir` digunakan untuk Remove sebuah directory
```
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
```
<br />

Fungsi `read` digunakan untuk membaca data dari file yang telah dibuka (terbuka) <br />
Fungsi read yang dipanggil saat sistem mencoba untuk membaca potongan demi potongan data dari suatu file.
```
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
```
<br />

Fungsi `mknod` digunakan untuk membuat sebuah file node.
```
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
```
<br />

Fungsi `unlink` digunakan untuk Remove sebuah file.
```
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
```
<br />

Fungsi `write` digunakan untuk Write data to an open file.
```
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
```
<br />

Fungsi `truncate` digunakan untuk mengubah ukuran file.
```
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
```
<br />

Fungsi `create` digunakan untuk membuat dan membuka sebuah file.
```
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
```
<br />

Fungsi `utimens` digunakan untuk mengubah waktu akses dan modifikasi file dengan resolusi nanodetik.
```
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
```
<br />

Fungsi `access` digunakan untuk melakukan pemeriksaan izin akses file. <br />
Fungsi ini akan dipanggil untuk panggilan sistem `access()`. Jika opsi mount `'default_permissions'` diberikan, metode ini tidak dipanggil.
```
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
```
<br />

Fungsi `open` digunakan untuk membuka sebuah file.
```
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
```
<br />

Fungsi `encryptDecrypt` digunakan untuk melakukan proses enkripsi dan dekripsi pada suatu file/folder.
```
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
```
<br />

Fungsi `int findAwal` digunakan untuk mencari huruf pertama nama file setelah `/`.
```
int findAwal(char *path, int st){
    int pLen = strlen(path);

    for(int i =0; i<pLen; i++){
        if(path[i] == '/'){
            return i+1;
        }
    }
    return st;
}

```
<br />

Fungsi `int findAkhir` digunakan untuk mencari huruf setelah `.`, pencarian dimulai dari urutan paling akhir.
```
int findAkhir(char *path){
    int pLen = strlen(path);

    for(int i=pLen-1; i>=0; i--){
        if(path[i] == '.'){
            return i;
        }
    }
    return pLen;
}
```
<br />

#### Tampilan pada Linux
Menjalankan program <br />
![proses 2](https://user-images.githubusercontent.com/16980689/80815401-c9f9dd80-8bf7-11ea-81de-d3181da6c9b0.PNG)
<br />
Direktori `soal` menampilkan file dan folder sebelum di enkripsi
![sebelum enkrip](https://user-images.githubusercontent.com/16980689/80815787-7c31a500-8bf8-11ea-87ab-65569f7aadbf.PNG)
<br />
Direktori `encv1_` menampilkan file dan folder hasil enkripsi (setelah program dijalankan)
![sesudah enkrip](https://user-images.githubusercontent.com/16980689/80815817-85bb0d00-8bf8-11ea-9914-410aab11704c.PNG)
<br />


### SOAL 2
![S__11214850](https://user-images.githubusercontent.com/16980689/80863047-a0968b80-8ca3-11ea-8020-abf94165b0e4.jpg)


### SOAL 3
![S__11214850](https://user-images.githubusercontent.com/16980689/80863047-a0968b80-8ca3-11ea-8020-abf94165b0e4.jpg)


### SOAL 4
Log system:
- Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan daftar perintah system call yang telah dijalankan.
- Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING.
- Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.
- Sisanya, akan dicatat dengan level INFO.
- Format untuk logging yaitu: `[LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]`
```
LEVEL     : Level logging 
yy   	    : Tahun dua digit
mm    	  : Bulan dua digit
dd    	  : Hari dua digit
HH    	  : Jam dua digit
MM    	  : Menit dua digit
SS    	  : Detik dua digit
CMD       : System call yang terpanggil
DESC      : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::)
```
- Contoh format logging nantinya seperti:
```
INFO::200419-18:29:28::MKDIR::/iz1
INFO::200419-18:29:33::CREAT::/iz1/yena.jpg
INFO::200419-18:29:33::RENAME::/iz1/yena.jpg::/iz1/yena.jpeg
```

### Penyelesaian SOAL 4
- Masuk ke directory Modul4 <br />
Syntax : `cd Documents/modul4/`
- Melihat isi file `fs.txt` yang berisi daftar perintah system call yang telah dijalankan. <br />
Syntax : `cat fs.txt`

#### ___Source Code : [ssfs.c](https://github.com/fikrihaykal/SoalShiftSISOP20_modul4_T20/blob/master/ssfs.c)___ 
#### Penjelasan Source Code: 

```
static const char *logsys = "/home/fikri/Modul4/fs.txt";

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
```
- Fungsi `static const char *logsys = "/home/fikri/Modul4/fs.txt";` berfungsi untuk meletakkan `log file` dengan nama <b>fs.txt</b> pada direktori <b>/home/fikri/Modul4</b>.
- Fungsi `void catatLog(char *lv, char *command, int res, int lenDesc, const char *desc[]){` akan mengatur format _logging_ yang sesuai dengan ketentuan _log file_ pada soal tersebut antara lain `level` untuk menunjukkan _system_ call yang terjadi termasuk ke dalam level mana, `cmd` akan menunjukkan _system call_ yang terpanggil, `res` akan menyimpan status dari file tersebut, `lenDesc` akan menunjukkan panjang _file path_ dan `desc[]` akan menunjukkan _absolute file path_.
- Fungsi `FILE *file = fopen(logsys, "a+");` merupakan proses untuk membuat _log file_ sesuai dengan _directory path_ yang telah ditentukan.
- Fungsi `char timeTemp[100];` merupakan sebuah _buffer_ yang berguna untuk menyimpan waktu pemanggilan _system call_ tersebut dengan menggunakan fungsi `strftime(timeTemp, sizeof(timeTemp), "%y%m%d-%H:%M:%S", tm);`
- Format logging dalam fungsi <b>catatLog()</b> akan dituliskan ke dalam _log file_ menggunakan fungsi `sprintf(logTemp, "%s::%s::%s::%d", lv, timeTemp, command, res);`
- Untuk menunjukkan _absolute file path_ `desc[]`, maka perlu digunakan fungsi iterasi di bawah ini
```
for(int i=0; i<lenDesc; i++){
        sprintf(logTemp, "%s::%s", logTemp, desc[i]);
    }
``` 
karena _absolute file path_ akan ditemukan sepanjang `lenDesc` yang menunjukkan panjang _file path_ tersebut.
<br />

#### Tampilan pada Linux
Melihat daftar perintah system call yang telah dijalankan pada terminal
![soal4](https://user-images.githubusercontent.com/16980689/80818738-f153a900-8bfd-11ea-8dbc-0b0a203d2284.PNG) <br />
Melihat daftar perintah system call yang telah dijalankan pada file <b>fs.txt</b>
![soal4(2)](https://user-images.githubusercontent.com/16980689/80818733-ef89e580-8bfd-11ea-992e-cb6f726f3ca7.PNG)


## KENDALA
Bobot soal semakin susah dan sulit dipahami <br />  
Kami hanya sanggup mengerjakan soal nomor 1 dan 4 :)
