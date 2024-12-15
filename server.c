#include "server.h"
#include "logger.h"
#include "file_handler.h"
#include "rate_limit.h"

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
int send_file_efficient(int client_socket, const char *file_path) {
    // Membuka file untuk dibaca
    int file_fd = open(file_path, O_RDONLY);
    if (file_fd < 0) return -1;  // Jika file tidak dapat dibuka, kembalikan -1

    struct stat file_stat;
    // Mengambil informasi statistik file
    if (fstat(file_fd, &file_stat) < 0) {
        close(file_fd);
        return -1;  // Jika tidak dapat mendapatkan informasi file, kembalikan -1
    }

    // Mendapatkan jenis konten dari file berdasarkan ekstensi
    const char *content_type = get_content_type(file_path);

    // Membuat header HTTP untuk respon
    char header[BUFFER_SIZE];
    int header_len = snprintf(header, sizeof(header), 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n\r\n", 
        content_type, file_stat.st_size);
    
    // Mengirimkan header ke klien
    write(client_socket, header, header_len);
    
    // Mengirimkan file ke klien menggunakan sendfile
    off_t offset = 0;
    ssize_t sent_bytes = sendfile(client_socket, file_fd, &offset, file_stat.st_size);
    
    close(file_fd);
    return sent_bytes == file_stat.st_size ? 0 : -1;  // Mengembalikan 0 jika file terkirim seluruhnya
}

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
void handle_http_request(int client_socket, const char *client_ip, FILE *logfile) {
    // Memeriksa apakah klien terkena rate limiting
    if (is_rate_limited(client_ip)) {
        const char *rate_limited_response = 
            "HTTP/1.1 429 Too Many Requests\r\n"
            "Retry-After: 1\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n\r\n"
            "You have sent too many requests. Please try again later.\n";
        
        // Mengirimkan respons 429 jika klien dibatasi
        write(client_socket, rate_limited_response, strlen(rate_limited_response));
        log_message(logfile, client_ip, "429 Too Many Requests", 429);
        close(client_socket);
        return;
    }

    // Membaca permintaan dari klien
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    
    // Jika tidak ada data atau kesalahan membaca, tutup koneksi
    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }

    buffer[bytes_read] = '\0';  // Menambahkan karakter null terminator pada buffer
    printf("Request: %s\n", buffer);

    // Menafsirkan permintaan HTTP (method, path, dan version)
    char method[16], path[256], version[16];
    if (sscanf(buffer, "%15s %255s %15s", method, path, version) != 3) {
        const char *error_response = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n";
        write(client_socket, error_response, strlen(error_response));
        log_message(logfile, client_ip, buffer, 400);
        close(client_socket);
        return;
    }

    // Memeriksa apakah metode yang digunakan adalah GET
    if (strcmp(method, "GET") != 0) {
        const char *method_not_allowed = "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n";
        write(client_socket, method_not_allowed, strlen(method_not_allowed));
        log_message(logfile, client_ip, buffer, 405);
        close(client_socket);
        return;
    }

    // Menentukan path file berdasarkan permintaan
    char file_path[BUFFER_SIZE];
    snprintf(file_path, sizeof(file_path), "%s%s", 
             DOCUMENT_ROOT, 
             strcmp(path, "/") == 0 ? "/index.html" : path);

    // Mengirimkan file jika ditemukan
    if (send_file_efficient(client_socket, file_path) == 0) {
        log_message(logfile, client_ip, buffer, 200);
    } else {
        // Mengirimkan respons 404 jika file tidak ditemukan
        const char *not_found = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        write(client_socket, not_found, strlen(not_found));
        log_message(logfile, client_ip, buffer, 404);
    }
    close(client_socket);
}

/**
 * Fungsi untuk menginisialisasi server, termasuk konfigurasi socket, log file, dan rate limiting.
 * 
 * Fungsi ini mempersiapkan server untuk menerima koneksi, menginisialisasi log file, dan menetapkan pengaturan
 * untuk soket server. Jika terjadi kesalahan, server akan berhenti dan menampilkan pesan kesalahan.
 * 
 * @return Konfigurasi server yang berisi soket, alamat, dan file log.
 */
ServerConfig initialize_server() {
    ServerConfig config = {0};
    
    // Membuka file log untuk mencatat aktivitas server
    config.logfile = fopen(LOG_FILE, "a");
    if (!config.logfile) {
        perror("Log file opening failed");
        exit(EXIT_FAILURE);
    }

    // Menginisialisasi rate limiting
    initialize_rate_limit();

    // Membuat socket server
    config.server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (config.server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Mengatur alamat dan port server
    config.address.sin_family = AF_INET;
    config.address.sin_addr.s_addr = INADDR_ANY;
    config.address.sin_port = htons(PORT);

    // Mengikat soket ke alamat dan port
    if (bind(config.server_fd, (struct sockaddr *)&config.address, sizeof(config.address)) < 0) {
        perror("Bind failed");
        close(config.server_fd);
        exit(EXIT_FAILURE);
    }

    // Memulai mendengarkan koneksi masuk
    if (listen(config.server_fd, 10) < 0) {
        perror("Listen failed");
        close(config.server_fd);
        exit(EXIT_FAILURE);
    }

    // Menampilkan pesan bahwa server sedang mendengarkan
    printf("Server is listening on port %d...\n", PORT);
    return config;
}
