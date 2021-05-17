# Kelompok E09
* 05111940000074 - Nur Ahmad Khatim
* 05111940000087 - William Handi Wijaya
* 05111940000212 - Fadhil Dimas Sucahyo
<br><br>

# Soal 1
* Soal ini dikerjakan oleh 05111940000087 - William Handi Wijaya.
* Dilarang menggunakan `system()` dan `execv()`. Silahkan dikerjakan sepenuhnya dengan thread dan socket programming. 
* Untuk download dan upload silahkan menggunakan file teks dengan ekstensi dan isi bebas (yang ada isinya bukan touch saja dan tidak kosong) dan requirement untuk benar adalah percobaan dengan minimum 5 data.


## Subsoal a
### Penjelasan Soal
* Client bisa register dan login pada server.
   * Saat register, client akan diminta input id dan passwordnya.
   * Login dinyatakan berhasil jika id dan password yang dikirim client sesuai dengan list akun pada server.
* Sistem dapat menerima multi-connections.
* Jika ada satu client yang sudah login, maka client yang ingin login berikutnya harus menunggu hingga client yang sudah login tersebut logout.
* Data id dan password akun disimpan di file bernama **akun.txt** dengan format:
   ```
   id:password
   id2:password2
   ```
   
### Penyelesaian Soal
1. Buat server yang dapat menerima multi-connections.
2. Buat thread untuk mengatur IO.
3. Buat dua pilihan input, yaitu "register" dam "login".
4. Buat fitur register.
   1. Dapatkan input client
   2. Simpan input client di **akun.txt** sesuai format yang ditentukan.
5. Buat fitur login.
   1. Pastikan tidak ada client yang sedang terkoneksi.
      1. Jika client memilih login saat ada client lain yang sudah terloggin, kirim pesan **Server is busy. Wait other client to logout** ke client.
      2. Kembali ke menu pilihan input.
   2. Dapatkan input client.
   3. Pastikan data akun yang diinput client ada di **akun.txt**
      * Jika tidak ada, batalkan login
   4. Sambungkan client dengan server.

### Output
![Output soal 1a](https://res.cloudinary.com/dx3jourdf/image/upload/v1620456233/Output_soal_1_update_ntq180.png)


## Subsoal b & c
### Penjelasan Soal
* Sistem memiliki file bernama **files.tsv** yang dengan format:  
   `<path file di server>|<publisher>|<tahun publikasi>`
* File **files.tsv** merupakan database dari input file yang disimpan di folder **FILES**.
* Struktur dari direktori **FILES** adalah sebagai berikut:
   ```
   File1.ekstensi
   File2.ekstensi
   ```
* Client dapat mengirim input file text ke server dengan command `add` dengan format prompt message sebagai berikut.
   ```
   Publisher:
   Tahun Publikasi:
   Filepath: <*client's local filepath*>
   ```
* Simpan file yang baru di upload ke folder **FILES** dan update **files.tsv**.
   
### Penyelesaian Soal
1. Pastikan client sudah login ke server.
2. Saat client menggunakan command `add`, dapatkan input sesuai format prompt message di atas.
3. Pastikan folder **FILES** dan file **files.tsv** sudah terbuat.
4. Kirim file dari *client's local filepath* ke folder **FILES** di server.
   * Jika file tidak ada di *client's local filepath*
      1. Tampilkan pesan **Error, file not found** ke client.
      2. Kembali ke menu pemilihan perintah.
   * Jika file sudah ada di **files.tsv**
      1. Tampilkan pesan **Error, file is already uploaded** ke client.
      2. Kembali ke menu pemilihan perintah.
5. Tulis data file yang baru terinput ke file **files.tsv** sesuai format di atas.


## Subsoal d
### Penjelasan Soal
* Client dapat mendownload file text yang ada di folder **FILES**.
* Perintah untuk mendownload file text adalah `download <namafile>.<ekstensi>`.

### Penyelesaian Soal
1. Pastikan client sudah login ke server.
2. Dapatkan namafile dan ekstensi dari file yang ingin didownload.
3. Pastikan file tersebut ada di **files.tsv**.
   1. Jika tidak ada, tampilkan pesan **Error, file hasn't been downloaded** ke client.
   2. Kembali ke menu pemilihan perintah.
4. Kirim file ke client.


## Subsoal e
### Penjelasan Soal
* Perintah pada subsoal ini adalah `delete <nama file>.<ekstensi>`.
* Saat perintah ini dijalankan:
   * Nama file pada server akan berubah menjadi `old-<nama file>.<ekstensi>`.
   * Hapus row dari file tersebut di **file.tsv**.

### Pennyelesaian Soal
1. Pastikan client sudah login ke server.
2. Dapatkan nama file dan ekstensi dari file yang ingin dihapus.
3. Pastikan file tersebut ada di **files.tsv**.
   2. Jika tidak ada, tampilkan pesan **Error, file hasn't been downloaded** ke client.
   3. Kembali ke menu pemilihan perintah.
4. Hapus kolom dari file tersebut di **file.tsv**.
   1. Copy setiap baris dari **file.tsv** ke file baru bernama **temp**.
      * Nama file yang ingin dihapus tidak ikut dicopy.
   2. Hapus **file.tsv** .
   3. Ganti nama **temp** menjadi **file.tsv**.
5. Ganti nama file.


## Subsoal f
### Penjelasan Soal
* Perintah pada subsoal ini adalah `see`.
* Saat perintah ini dijalankan, akan tampil isi **files.tsv** di terminal client dengan format berikut:
   ```
   Nama:
   Publisher:
   Tahun publishing:
   Ekstensi File: 
   Filepath: 

   Nama:
   Publisher:
   Tahun publishing:
   Ekstensi File: 
   Filepath: 

   ```

### Penyelesaian Soal
1. Pastikan client sudah login ke server.
2. Scan isi **files.tsv** dengan perintah `fscanf`.
3. Parse setiap baris dari **files.tsv** untuk mendapatkan lima data yang akan diprint.
4. Print kelima data tersebut ke terminal client dengan perintah `send` atau `write`.

### Output
![Output soal 1f](https://res.cloudinary.com/dx3jourdf/image/upload/v1620745746/Output_soal1f_prufvj.png)


## Subsoal g
### Penjelasan Soal
* Perintah pada subsoal ini adalah `find <query string>`
* Hasilnya adalah semua nama file yang mengantung `<query string>` yand ditulis pada perintah `find`.
* Format output sama dengan format pada subsoal f.

### Penyelesaian Soal
1. Pastikan client sudah login ke server.
2. Dapatkan `<query string>`.
3. Lakukan modifikasi berikut ini pada penyelesaian soal 1f.
4. Setelah langkah 3 pada penyelesaian soal 1f, cek apakah nama file mengandung `<query string>`.
5. Jika iya, print detail dari file tersebut ke terminal client.

### Output
![Output soal 1g](https://res.cloudinary.com/dx3jourdf/image/upload/v1620748554/Output_soal1g_nygix9.png)


## Subsoal h
### Penjelasan Soal
* Buat log untuk perintah `add` dan `delete` di file `running.log`.
* Format log:
  ```
   Tambah : File1.ektensi (id:pass)
   Hapus : File2.ektensi (id:pass)
  ```

### Penyelesaian Soal
1. Simpan id dan pass user yang ter-login ke dalam array `auth_user`.
2. Catat log setelah perintah berhasil dijalankan.

### Output
![Output soal 1h](https://res.cloudinary.com/dx3jourdf/image/upload/v1620788360/Output_soal1h_rld5bl.jpg)
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
* Soal 1
  * Debug pada `thread`, `server.c`, dan `client.c` sulit untuk dilakukan.
