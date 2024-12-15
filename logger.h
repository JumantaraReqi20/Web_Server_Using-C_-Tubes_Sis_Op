#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

/**
 * Fungsi untuk mencatat pesan log yang berisi informasi tentang permintaan yang diterima.
 * 
 * Fungsi ini mencatat informasi mengenai waktu permintaan, alamat IP klien, 
 * permintaan yang dilakukan oleh klien, dan status respons dari server ke dalam file log.
 * Format log yang dicatat adalah: [timestamp] IP: <client_ip> | Request: <request> | Status: <status>.
 * 
 * @param logfile Pointer ke file log yang akan digunakan untuk mencatat pesan log.
 * @param client_ip Alamat IP dari klien yang membuat permintaan.
 * @param request String yang berisi permintaan yang diajukan oleh klien.
 * @param status Status respons dari server terhadap permintaan (misalnya, 200, 404, dll).
 */
void log_message(FILE *logfile, const char *client_ip, const char *request, int status);

#endif // LOGGER_H
