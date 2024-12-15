#include "server.h"
#include "rate_limit.h"
#include "logger.h"

/**
 * Fungsi utama untuk menjalankan server HTTP.
 * 
 * Fungsi ini menginisialisasi server, menerima koneksi klien, dan menangani setiap permintaan
 * HTTP dengan membuat proses baru untuk setiap koneksi klien menggunakan `fork()`. Server akan
 * terus menerima koneksi hingga dihentikan.
 * 
 * @return 0 jika server selesai dengan sukses, meskipun pada kenyataannya server ini akan terus berjalan.
 */
int main() {
    // Menginisialisasi server dan mendapatkan konfigurasi server
    ServerConfig server = initialize_server();
    socklen_t addr_len = sizeof(server.address);

    // Loop utama server untuk terus menerima koneksi
    while (1) {
        // Menerima koneksi dari klien
        int client_socket = accept(server.server_fd, (struct sockaddr *)&server.address, &addr_len);
        
        if (client_socket < 0) {
            // Menangani kesalahan jika accept gagal
            perror("Accept failed");
            continue;
        }

        // Menentukan alamat IP klien
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &server.address.sin_addr, client_ip, sizeof(client_ip));

        printf("New connection from %s\n", client_ip);

        // Membuat proses anak menggunakan fork() untuk menangani setiap klien
        pid_t pid = fork();
        if (pid == 0) {
            // Di dalam proses anak, menutup socket server dan menangani permintaan HTTP klien
            close(server.server_fd);
            handle_http_request(client_socket, client_ip, server.logfile);
            exit(0);  // Setelah menangani permintaan, proses anak akan keluar
        } else if (pid > 0) {
            // Di dalam proses induk, menutup socket klien karena sudah ditangani oleh proses anak
            close(client_socket);
        } else {
            // Menangani kesalahan jika fork gagal
            perror("Fork failed");
        }
    }

    // Menutup file log dan socket server ketika server berhenti (meskipun dalam kasus ini server berjalan terus)
    fclose(server.logfile);
    close(server.server_fd);
    return 0;
}
