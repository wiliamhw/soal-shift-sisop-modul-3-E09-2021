# Kelompok E09
* 05111940000074 - Nur Ahmad Khatim
* 05111940000087 - William Handi Wijaya
* 05111940000212 - Fadhil Dimas Sucahyo
<br><br>

# Soal 1
* Soal ini dikerjakan oleh 05111940000087 - William Handi Wijaya.
* Dilarang menggunakan system() dan execv(). Silahkan dikerjakan sepenuhnya dengan thread dan socket programming. 
* Untuk download dan upload silahkan menggunakan file teks dengan ekstensi dan isi bebas (yang ada isinya bukan touch saja dan tidak kosong) dan requirement untuk benar adalah percobaan dengan minimum 5 data.

## Subsoal a
<br><br>

# Soal 2
* Soal ini dikerjakan oleh 05111940000074 - Nur Ahmad Khatim.
* Semua matriks berasal dari input ke program.
* Dilarang menggunakan system().

## Subsoal a
### Penjelasan
* Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya.
* Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).

### Penyelesaian
* Membuat program input matriks 4x3 menggunakan for looping
* Membuat program input matriks 3x6 menggunakan for looping
* Hasil dari perkalian matriks distore ke variabel value yang akan digunakan bersama untuk soal2b
* Menampilkan Hasil perkalian sebelum matriks digunakan oleh program soal2b

## Subsoal b
### Penjelasan
* Membuat program dengan menggunakan matriks output dari program soal2a (menggunakan shared memory)
* Matriks akan dilakukan perhitungan dengan matrix baru (input user).
* Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya (dari paling besar ke paling kecil) (menggunakan thread).

### Penyelesaian
* Menerima input matriks value dari program soal2a menggunakan shared memory
* Membuat program input untuk matriks baru dengan ukuran 4x6
* Menggunakan thread untuk melakukan operasi di setiap cell pada matriks value
* Di setiap thread menjalankan fungsi factorial untuk menghasilkan matriks yang diinginkan
* Di setiap thread, dijoinkan yang kemudian semua hasilnya digabung dan dipassing untuk diprint

## Subsoal c
### Penjelasan
* Membuat program untuk mengecek 5 proses teratas apa saja yang memakan resource di komputer
* Menggunakan command "ps aux | sort -nrk 3,3 | head -5"
* Menggunakan IPC Pipes

### Penyelesaian
* Fork pertama : Menerima input dari stdin ps aux kemudian dikirim ke pipe1
* Fork kedua : Menerima input dari pipe1 kemudian kemudian diconcatenated dengan sort -nrk 3,3 dan dituliskan pada pipe2
* Fork ketiga : Menerima input dari pipe2 kemudian diconcatenated dengan head -5 dan dikirim ke stdout
<br><br>

# Soal 3
* Soal ini dikerjakan oleh 05111940000212 - Fadhil Dimas Sucahyo.
<br><br>

# Kendala
