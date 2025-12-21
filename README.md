# 🚀 CuteEsp32 - ESP32 PlatformIO Project

Project ini adalah contoh implementasi ESP32 menggunakan framework **PlatformIO**.  
Dirancang agar mudah dikembangkan lebih lanjut untuk berbagai aplikasi IoT, sensor, maupun kontrol perangkat.

---

## ✨ Fitur Utama
- Terintegrasi dengan **PlatformIO** (build system + dependency manager)
- Struktur kode modular dan mudah dikembangkan
- Mendukung **WiFi**, **Serial Debug**, **Bluetooth Clasic**,  **OTA Update**, **Web Server** dan **OTA Update lewat IP maupun Website**
- Mendukung FreeRTOS task 
- Logging terstruktur

---

## 🛠️ Persyaratan
Pastikan sudah terinstal:

- [PlatformIO](https://platformio.org/install) (di VS Code atau CLI)
- Python 3.12
- Board: **ESP32 Dev Module**
- Driver USB to UART (CH340/CP2102 jika diperlukan)

---

## 📂 Instalasi
### Install Python 3.12
### Buat Virtual environtment
```
python -m venv venv
```
### Aktifkan virtual environtment
#### Windows
```
    .\venv\Script\Activate

```
#### Mac / Linux
```
    source venv/bon/activate
```
### Install PlatformIO
```
    pip install platformio
```
### Compile 
``` 
pio run -d ProjectXX
```

## Example
### BlinkExample
Menjalankan blink secara FreeRTOS. Blink module bisa dimanfaatkan untuk indikator system normal dengan cara
- jika sistem normal blink berkedip dengan durasi 500 ms
- Jika sistem ada kendala blin berkedip lebih cepat dari 500ms  

### LCDExample
Menjalankan Animasi running text di baris ke-2 pada LCD secara FreeRTOS bersamaan dengan blink yang juga DreeRTOS. Dengan LCD onteraksi dengan manusia menjadi lebih mudah


### StorageExample
Interaksi dengan FileSystem (LittleFS) dikombinasikan dengan LCD dan Clink FreeRTOS. Dengan memanfaatkan penyimpanan di file system, program bisa lebih dinamis misalnyaa untuk penyimpanan SSID dan password WiFi

### BluetoothExample
Interaksi dengan Bluetooth dipadukan dengan FileSystem (LittleFS), LCD dan Clink FreeRTOS. Komunikasi ini bisa dimanfaatkan contohnya untuk penggantian SSID dan password WiFi

