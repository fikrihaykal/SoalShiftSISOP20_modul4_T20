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
