#ifndef RATE_LIMIT_H
#define RATE_LIMIT_H

#include <time.h>
#include <netinet/in.h>

#define MAX_CLIENTS 1000
#define RATE_LIMIT 10
#define RATE_LIMIT_WINDOW 1

typedef struct {
    char ip[INET_ADDRSTRLEN];
    int request_count;
    time_t last_request_time;
} RateLimit;

extern RateLimit *shared_clients;

/**
 * Fungsi untuk menginisialisasi pembatasan laju (rate limiting) dengan memori bersama.
 * 
 * Fungsi ini mengalokasikan memori bersama untuk menyimpan data pembatasan laju bagi setiap klien.
 * Jika alokasi memori gagal, fungsi ini akan menampilkan pesan kesalahan dan menghentikan program.
 * 
 * Memori bersama ini digunakan untuk mengelola informasi tentang permintaan klien dan status
 * pembatasan laju (rate limiting) untuk setiap alamat IP.
 */
void initialize_rate_limit();

/**
 * Fungsi untuk menemukan klien berdasarkan alamat IP atau menambahkan klien baru ke dalam tabel.
 * 
 * Fungsi ini mencari apakah alamat IP sudah ada dalam tabel pembatasan laju. Jika ada, fungsi akan
 * mengembalikan indeks klien tersebut. Jika tidak ada, maka klien baru akan ditambahkan ke dalam tabel
 * dan fungsi akan mengembalikan indeks klien yang baru ditambahkan.
 * 
 * @param ip Alamat IP klien yang ingin dicari atau ditambahkan.
 * @return Indeks klien dalam tabel (atau -1 jika tabel penuh).
 */
int is_rate_limited(const char *ip);

/**
 * Fungsi untuk memeriksa apakah alamat IP telah mencapai batas permintaan (rate limit).
 * 
 * Fungsi ini memeriksa jumlah permintaan yang telah dilakukan oleh klien dalam jendela waktu tertentu.
 * Jika jumlah permintaan melebihi batas yang ditentukan (RATE_LIMIT), maka klien dianggap terhalang 
 * dan tidak dapat melakukan permintaan lebih lanjut. Jika tidak, permintaan dianggap sah.
 * 
 * @param ip Alamat IP klien yang akan diperiksa.
 * @return 1 jika klien dibatasi (terkena rate limiting), 0 jika permintaan sah.
 */
int find_or_add_client(const char *ip);

#endif // RATE_LIMIT_H

