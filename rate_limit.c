#include "rate_limit.h"
#include <stdlib.h>
#include "server.h"
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

// Pointer ke struktur RateLimit yang akan disimpan di memori bersama (shared memory)
RateLimit *shared_clients;

/**
 * Fungsi untuk menginisialisasi pembatasan laju (rate limiting) dengan memori bersama.
 * 
 * Fungsi ini mengalokasikan memori bersama untuk menyimpan data pembatasan laju bagi setiap klien.
 * Jika alokasi memori gagal, fungsi ini akan menampilkan pesan kesalahan dan menghentikan program.
 * 
 * Memori bersama ini digunakan untuk mengelola informasi tentang permintaan klien dan status
 * pembatasan laju (rate limiting) untuk setiap alamat IP.
 */
void initialize_rate_limit() {
    // Mengalokasikan memori bersama untuk menyimpan informasi rate limiting untuk klien
    shared_clients = mmap(
        NULL, 
        sizeof(RateLimit) * MAX_CLIENTS,  // Ukuran memori yang dialokasikan untuk MAX_CLIENTS klien
        PROT_READ | PROT_WRITE,            // Akses baca dan tulis
        MAP_SHARED | MAP_ANONYMOUS,        // Memori bersama dan anonim
        -1,                                // Tidak ada file yang digunakan
        0                                   // Tidak ada offset
    );

    // Jika alokasi memori gagal, tampilkan pesan kesalahan dan keluar
    if (shared_clients == MAP_FAILED) {
        perror("Failed to create shared memory");
        exit(EXIT_FAILURE);
    }

    // Inisialisasi semua data klien dengan nilai nol (kosongkan memori)
    memset(shared_clients, 0, sizeof(RateLimit) * MAX_CLIENTS);
}

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
int find_or_add_client(const char *ip) {
    // Mencari klien yang sudah ada berdasarkan IP
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(shared_clients[i].ip, ip) == 0) {
            return i;  // Klien ditemukan, kembalikan indeksnya
        }
    }

    // Menambahkan klien baru jika ada ruang
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (shared_clients[i].ip[0] == '\0') {
            // Salin IP klien dan inisialisasi informasi lainnya
            strncpy(shared_clients[i].ip, ip, INET_ADDRSTRLEN);
            shared_clients[i].request_count = 0;
            shared_clients[i].last_request_time = 0;
            return i;  // Klien baru ditambahkan, kembalikan indeksnya
        }
    }

    return -1;  // Tabel penuh, tidak dapat menambahkan klien baru
}

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
int is_rate_limited(const char *ip) {
    // Mencari atau menambahkan klien berdasarkan IP
    int index = find_or_add_client(ip);
    if (index == -1) {
        // Jika tabel pembatasan laju penuh, tampilkan pesan kesalahan
        fprintf(stderr, "Rate limiting table full!\n");
        return 1;  // Klien dibatasi
    }

    // Mendapatkan waktu saat ini
    time_t now = time(NULL);

    // Jika jendela waktu rate limit telah berlalu, reset hitungan permintaan
    if (now - shared_clients[index].last_request_time >= RATE_LIMIT_WINDOW) {
        shared_clients[index].request_count = 1;
        shared_clients[index].last_request_time = now;
        return 0;  // Permintaan sah
    }
    
    // Jika jendela waktu masih berlaku, tambahkan hitungan permintaan
    shared_clients[index].request_count++;

    // Jika jumlah permintaan melebihi batas, klien dibatasi
    return shared_clients[index].request_count > RATE_LIMIT;
}
