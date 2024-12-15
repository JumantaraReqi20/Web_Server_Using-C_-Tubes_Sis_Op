#include "file_handler.h"
#include <string.h>

/**
 * Fungsi untuk mendapatkan jenis konten berdasarkan ekstensi file.
 * 
 * Fungsi ini menerima path file sebagai input dan mengembalikan jenis konten 
 * (content type) yang sesuai berdasarkan ekstensi file tersebut. Jika ekstensi 
 * file tidak dikenali, maka jenis konten default yang dikembalikan adalah 
 * "application/octet-stream", yang merupakan jenis konten umum untuk file biner.
 * 
 * @param file_path Path atau nama file yang akan diperiksa ekstensi dan jenis kontennya.
 * @return Jenis konten (content type) sesuai dengan ekstensi file.
 */
const char* get_content_type(const char *file_path) {
    // Mencari posisi terakhir dari titik (.) pada path file untuk mendapatkan ekstensi
    const char *ext = strrchr(file_path, '.');
    
    // Jika tidak ada ekstensi (tidak ada titik), kembalikan jenis konten default
    if (ext == NULL) return "application/octet-stream";

    // Memeriksa jenis ekstensi dan mengembalikan jenis konten yang sesuai
    if (strcmp(ext, ".html") == 0) return "text/html; charset=utf-8";
    if (strcmp(ext, ".css") == 0) return "text/css; charset=utf-8";
    if (strcmp(ext, ".js") == 0) return "application/javascript; charset=utf-8";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";

    // Jika ekstensi tidak dikenali, kembalikan jenis konten default
    return "application/octet-stream";
}
