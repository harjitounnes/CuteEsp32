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
- BlinkExample
Menjalankan blink secara FreeRTOS 

- LCDExample
Menjalankan Animasi running text di baris ke-2 pada LCD secara FreeRTOS bersamaan dengan blink yang juga DreeRTOS

- StorageExample
Interaksi dengan FileSystem (LittleFS) dikombinasikan dengan LCD dan Clink FreeRTOS

