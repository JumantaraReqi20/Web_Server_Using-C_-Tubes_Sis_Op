# Web Server Sederhana

## Deskripsi
Program ini adalah implementasi web server sederhana menggunakan bahasa C, dirancang untuk memenuhi kebutuhan tugas besar dalam mata kuliah Sistem Operasi. Web server ini mendukung permintaan HTTP dasar, seperti GET, dan dilengkapi dengan fitur pembatasan permintaan klien untuk mencegah serangan Denial of Service (DoS). Server juga dirancang menggunakan konsep Inter Process Communication (IPC) dengan shared memory untuk mengelola child process.

---

## Fitur Utama
1. **Handling Permintaan HTTP**
   - Mendukung metode HTTP GET.
   - Mengirim file statis dari direktori yang ditentukan (DOCUMENT_ROOT).

2. **Pembatasan Permintaan Klien (Rate Limiting)**
   - Membatasi jumlah permintaan dari klien dalam periode waktu tertentu untuk mencegah DoS.
   - Menggunakan shared memory untuk menyimpan data klien secara efisien.

3. **Log Aktivitas**
   - Mencatat setiap permintaan klien, termasuk alamat IP, status respons, dan timestamp ke dalam file log.

4. **Struktur Modular**
   - Pemrosesan file dan permintaan HTTP diorganisir secara modular untuk mempermudah pengembangan dan debugging.

---

## Persyaratan Sistem
- Sistem operasi: Linux atau Windows dengan lingkungan C compiler.
- GCC (GNU Compiler Collection) untuk kompilasi program.
- Perpustakaan standar C, seperti `stdio.h`, `stdlib.h`, `unistd.h`, dan lainnya.

---
