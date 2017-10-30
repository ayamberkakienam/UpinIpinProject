# Tugas 1 IF3130 Jaringan Komputer

## Sliding Window

## Petunjuk Penggunaan Program

1. Lakukan kompilasi dengan menjalankan Makefile, cukup ketikkan make pada terminal.
2. Buka file recvfile.cpp, lalu buka sendfile.cpp pada terminal lain.

## Cara Kerja Sliding Window

1. Pertama-tama recvfile akan membuka koneksi baru dengn membuat socket dan melakukan bind dengan menggunakan fungsi bind(), begitu juga dengan sendfile.
2. Setelah terbentuk koneksi, sendfile.cpp mengirimkan data dan recvfile.cpp menunggu data datang.
3. Mekanisme pengiriman data ini menggunakan protokol sliding window, yaitu membagi data menjadi beberapa frame.
4. Data yang akan dikirim dibaca dari file eksternal dan dimasukkan ke dalam file buffer sebagai vektor.
5. Kemudian data tersebut dikirim sebagai frame sebanyak ukuran window, dengan penambahan identifikasi eror berupa checksum dengan menggunakan fungsi getChecksum().
6. Setelah frame tersebut sampai pada recvfile, frame tersebut akan disimpan pada file receive buffer dan dipindahkan ke file eksternal dengan urutan yang tepat. Selanjutnya recvfile akan mengirim ack kembali ke sendfile dengan sequence number selanjutnya. 

## Pembagian Tugas

Husnuldzaki Wibisono
Aya Aurora
Arfinda ilmania

## Pertanyaan

1. Apa yang terjadi jika advertised window yang dikirim bernilai 0? Apa cara untuk menangani hal tersebut?

Advertised window bernilai 0 berarti receive buffer dalam keadaan penuh. Sehingga data-data yang belum masuk ke dalam receive buffer harus dikirim secara berulang-ulang hingga data tersebut berhasil masuk.

2. Sebutkan field data yang terdapat TCP Header serta ukurannya, ilustrasikan, dan jelaskan kegunaan dari masing-masing field data tersebut!

- Source TCP port number (2 bytes), komunikasi endpoint untuk mengirim.
- Destination TCP port number (2 bytes), komunikasi endpoint untuk menerima.
- Sequence number (4 bytes), untuk menandakan urutan dari sekumpulan paket.
- Acknowledgment number (4 bytes), untuk mengomunikasikan sequence number dari paket yang baru saja diterima aau 
- TCP data offset (4 bits), menyimpan total size dari TCP header dalam kelipatan 4 bytes.
- Reserved data (3 bits), nilainya selalu 0. 
- Control flags (up to 9 bits), memiliki 9 jenis flags yang setiap flags berukuran 1 bit. Flags tersebut adalah: CWR, ECE, URG, ACK, PSH, RST, SYN, FIN
- Window size (2 bytes), untuk mengatur berapa banyak data yang dapat dikirim  ke receiver sebelum mendapatkan acknowledgement
- TCP checksum (2 bytes), nilainya didapat dari protokol pengirim yang didapat dari hasil perhitungan matematika untuk membantu receiver mendeteksi apabila paket korup dan sebagainya. 
- Urgent pointer (2 bytes), dapat dipakai sebagai data offset untuk menandakan suatu paket membutuhkan priortas pemrosesan tertentu. 
- TCP optional data (0-40 bytes), 




