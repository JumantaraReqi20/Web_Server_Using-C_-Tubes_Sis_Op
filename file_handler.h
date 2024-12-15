#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

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
const char* get_content_type(const char *file_path);

#endif // FILE_HANDLER_H
