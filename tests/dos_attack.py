import requests
import time
import concurrent.futures

# Konfigurasi pengujian
TARGET_URL = "http://localhost:8080"
NUM_THREADS = 10  # Jumlah thread concurrent
TOTAL_REQUESTS = 15  # Total permintaan yang akan dikirim
DELAY_BETWEEN_REQUESTS = 0.1  # Delay antar request (detik)

def send_request(thread_id):
    """Fungsi untuk mengirim request ke server"""
    try:
        start_time = time.time()
        response = requests.get(TARGET_URL, timeout=5)
        end_time = time.time()
        
        print(f"Thread {thread_id}: Status {response.status_code} | Time: {end_time - start_time:.4f} sec")
        return response.status_code
    except requests.RequestException as e:
        print(f"Thread {thread_id}: Error - {e}")
        return None

def test_dos():
    """Melakukan DoS testing"""
    print("🚀 Memulai DoS Testing...")
    
    # Pencatat statistik
    status_codes = {200: 0, 429: 0}
    
    # Gunakan ThreadPoolExecutor untuk concurrent requests
    with concurrent.futures.ThreadPoolExecutor(max_workers=NUM_THREADS) as executor:
        # Kirim request secara bersamaan
        futures = [
            executor.submit(send_request, i) 
            for i in range(TOTAL_REQUESTS)
        ]
        
        # Tunggu dan catat hasilnya
        for future in concurrent.futures.as_completed(futures):
            result = future.result()
            if result in status_codes:
                status_codes[result] += 1
            time.sleep(DELAY_BETWEEN_REQUESTS)
    
    # Tampilkan ringkasan
    print("\n📊 Ringkasan Pengujian:")
    print(f"Total Permintaan: {TOTAL_REQUESTS}")
    for code, count in status_codes.items():
        print(f"Status {code}: {count} kali")

def main():
    print("⚠️ Peringatan: Gunakan script ini hanya untuk pengujian di lingkungan terkendali!")
    test_dos()

if __name__ == "__main__":
    main()