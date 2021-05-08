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
1. Client bisa register dan login pada server.
   * Saat register, client akan diminta input id dan passwordnya.
   * Login dinyatakan berhasil jika id dan password yang dikirim client sesuai dengan list akun pada server.
2. Sistem dapat menerima multi-connections.
3. Jika ada satu client yang sudah login, maka client yang ingin login berikutnya harus menunggu hingga client yang sudah login tersebut logout.
4. Data id dan password akun disimpan di file bernama **akun.txt** dengan format:
   ```
   id:password
   id2:password2
   ```
   
### Penyelesaian Soal
1. Buat server dengan epoll.
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

### Outputt
![Output soal 1a](https://res.cloudinary.com/dx3jourdf/image/upload/v1620456233/Output_soal_1_update_ntq180.png)

## Subsoal b & c
### Penjelasan Soal
1. Sistem memiliki file bernama **files.tsv** yang dengan format:  
   `<path file di server>|<publisher>|<tahun publikasi>`
2. File **files.tsv** merupakan database dari input file yang disimpan di folder **FILES**.
3. Struktur dari direktori **FILES** adalah sebagai berikut:
   ```
   File1.ekstensi
   File2.ekstensi
   ```
4. Client dapat mengirim input file ke server dengan command `add` dengan format prompt message sebagai berikut.
   ```
   Publisher:
   Tahun Publikasi:
   Filepath: <*client's local filepath*>
   ```
5. Simpan file yang baru di upload ke folder **FILES** dan update **files.tsv**.
   
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
1. Client dapat mendownload file yang ada di folder **FILES**.
2. Perintah untuk mendownload file adalah `download <namafile>.<ekstensi>`.

### Penyelesaian Soal
1. Pastikan client sudah login ke server.
2. Dapatkan namafile dan ekstensi dari file yang ingin didownload.
3. Pastikan file tersebut ada di **files.tsv**.
   1. Jika tidak ada, tampilkan pesan **Error, file hasn't been downloaded** ke client.
   2. Kembali ke menu pemilihan perintah.
4. Kirim file ke client.

## Subsoal e
### Penjelasan Soal
1. Perintah pada subsoal ini adalah `delete <nama file>.<ekstensi>`.
2. Saat perintah ini dijalankan:
   1. Nama file pada server akan berubah menjadi `old-<nama file>.<ekstensi>`.
   2. Hapus row dari file tersebut di **file.tsv**.

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
<br><br>

# Soal 2
* Soal ini dikerjakan oleh 05111940000074 - Nur Ahmad Khatim.
<br><br>

# Soal 3
* Soal ini dikerjakan oleh 05111940000212 - Fadhil Dimas Sucahyo.
<br><br>

# Kendala
* Soal 1
  * Debug pada thread, server, dan client sulit untuk dilakukan.
  * Kesulitan untuk mentransfer file gambar dan `.pdf`.