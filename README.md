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
### Penjelasan Soal
1. Client bisa register dan login pada server.
   * Saat register, client akan diminta input id dan passwordnya.
   * Login dinyatakan berhasil jika id dan password yang dikirim client sesuai dengan list akun pada server.
2. Sistem dapat menerima multi-connections.
3. Jika ada satu client yang sudah login, maka client yang ingin login berikutnya harus menunggu hingga client yang sudah login tersebut logout.
4. Data id dan password akun disimpan di file bernama `akun.txt` dengan format:
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
   2. Simpan input client di `akun.txt` sesuai format yang ditentukan.
5. Buat fitur login.
   1. Pastikan tidak ada client yang sedang terkoneksi.
      1. Jika client memilih login saat ada client lain yang sudah terloggin, kirim pesan `Server is busy. Wait other client to logout` ke client.
      2. Kembali ke menu pilihan input.
   2. Dapatkan input client.
   3. Pastikan data akun yang diinput client ada di `akun.txt`
      * Jika tidak ada, batalkan login
   4. Sambungkan client dengan server.

<br><br>

# Soal 2
* Soal ini dikerjakan oleh 05111940000074 - Nur Ahmad Khatim.
<br><br>

# Soal 3
* Soal ini dikerjakan oleh 05111940000212 - Fadhil Dimas Sucahyo.
<br><br>

# Kendala