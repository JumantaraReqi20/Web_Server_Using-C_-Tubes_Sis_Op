#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <sys/mman.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define DOCUMENT_ROOT "./www"
#define LOG_FILE "web_server.log"

typedef struct {
    int server_fd;
    FILE *logfile;
    struct sockaddr_in address;
} ServerConfig;

/**
 * Fungsi untuk menginisialisasi server, termasuk konfigurasi socket, log file, dan rate limiting.
 * 
 * Fungsi ini mempersiapkan server untuk menerima koneksi, menginisialisasi log file, dan menetapkan pengaturan
 * untuk soket server. Jika terjadi kesalahan, server akan berhenti dan menampilkan pesan kesalahan.
 * 
 * @return Konfigurasi server yang berisi soket, alamat, dan file log.
 */
ServerConfig initialize_server();

/**
 * Fungsi untuk mengirimkan file dengan cara yang efisien menggunakan sistem operasi.
 * 
 * Fungsi ini mengirimkan file ke klien dengan cara menulis header HTTP yang sesuai dan
 * menggunakan fungsi `sendfile` untuk mentransfer data file ke socket klien secara langsung.
 * 
 * @param client_socket Socket klien yang digunakan untuk mengirimkan file.
 * @param file_path Path file yang akan dikirimkan ke klien.
 * @return 0 jika file berhasil dikirimkan, -1 jika terjadi kesalahan.
 */
int send_file_efficient(int client_socket, const char *file_path);

/**
 * Fungsi untuk menangani permintaan HTTP dari klien.
 * 
 * Fungsi ini memproses permintaan HTTP dari klien, memeriksa apakah klien dibatasi oleh rate limiting,
 * dan kemudian memproses permintaan tersebut. Jika permintaan valid, server akan mengirimkan file yang diminta,
 * jika tidak, server akan mengirimkan respons kesalahan yang sesuai.
 * 
 * @param client_socket Socket klien untuk berkomunikasi.
 * @param client_ip Alamat IP klien untuk melakukan rate limiting.
 * @param logfile Pointer ke file log untuk mencatat aktivitas.
 */
void handle_http_request(int client_socket, const char *client_ip, FILE *logfile);


#endif // SERVER_H