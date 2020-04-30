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
- Masuk ke directory Modul4 <br />
Syntax : `cd Documents/modul4/fuse`
- Membuat file <b>ssfs.c</b> yang berisi program FUSE<br />
Syntax : `nano ssfs.c` <br />
___source code : [ssfs.c](https://github.com/fikrihaykal/SoalShiftSISOP20_modul4_T20/blob/master/ssfs.c)___ <br />
- Compile FUSE <br />
Perintah ``` gcc -Wall `pkg-config fuse --cflags` [file.c] -o [output] `pkg-config fuse --libs` ``` digunakan untuk mengcompile program FUSE. <br />
Syntax : `gcc soal1.c -o soal1` <br />
- Membuat sebuah direktori <br />
Perintah `mkdir [direktori tujuan]` digunakan sebagai tujuan pembuatan FUSE <br />
Disini kami membuat direktori bernama <b> encv1_ </b>
- Menjalankan FUSE <br />
Perintah `./[output] [direktori tujuan]` digunakan untuk menjalankan FUSE pada direktori yang telah dibuat. <br />
Untuk menjalankannya kami menggunakan perintah `./ssfs encv1`

##### Tampilan _ssfs_ setelah dijalankan pada linux


### SOAL 4
Log system:
- Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan daftar perintah system call yang telah dijalankan.
- Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING.
- Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.
- Sisanya, akan dicatat dengan level INFO.
- Format untuk logging yaitu: `[LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]`
```
LEVEL    : Level logging 
yy   	 : Tahun dua digit
mm    	 : Bulan dua digit
dd    	 : Hari dua digit
HH    	 : Jam dua digit
MM    	 : Menit dua digit
SS    	 : Detik dua digit
CMD     	 : System call yang terpanggil
DESC      : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::)
```
- Contoh format logging nantinya seperti:
```
INFO::200419-18:29:28::MKDIR::/iz1
INFO::200419-18:29:33::CREAT::/iz1/yena.jpg
INFO::200419-18:29:33::RENAME::/iz1/yena.jpg::/iz1/yena.jpeg
```
