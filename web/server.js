const express = require("express");
const fs = require("fs");
const os = require("os");

const crypto = require("crypto");
const app = express();

function getAllIPs() {
  const nets = os.networkInterfaces();
  const results = [];

  const ignorePatterns = [
    /vmnet/i,               // VMware Linux/Mac (vmnet1, vmnet8)
    /vmware/i,              // Windows: "VMware Network Adapter"
    /vbox/i,                // Optional: VirtualBox (hapus kalau tidak ingin)
  ];

  for (const name of Object.keys(nets)) {
    if (ignorePatterns.some(re => re.test(name))) {
      continue;
    }
    for (const net of nets[name]) {
      if (net.family === "IPv4" && !net.internal) {
        results.push({
          iface: name,
          address: net.address
        });
      }
    }
  }

  return results;
}

const PORT = 3000;
const FIRMWARE_FILE = "./firmware/esp32.bin";

// =======================
//  HALAMAN UTAMA
// =======================
app.get("/", (req, res) => {
  res.send(`
    <html>
      <head>
        <title>ESP32 Firmware Server</title>
        <style>
          body {
            font-family: Arial;
            padding: 20px;
            background: #f5f5f5;
          }
          .box {
            background: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 3px 10px rgba(0,0,0,0.2);
            max-width: 500px;
            margin: auto;
          }
          .btn {
            padding: 10px 15px;
            background: #007bff;
            color: white;
            text-decoration: none;
            border-radius: 6px;
          }
        </style>
      </head>
      <body>
        <div class="box">
          <h2>ESP32 Firmware Update Server</h2>
          <p>Endpoint cek firmware:</p>
          <a class="btn" href="/firmware/check.json">Test Endpoint</a>
        </div>
      </body>
    </html>
  `);
});

// =======================
//  ENDPOINT CHECK FIRMWARE
// =======================
app.get("/firmware/check.json", (req, res) => {

  // 1️⃣ Cek file ada atau tidak
  if (!fs.existsSync(FIRMWARE_FILE)) {
    return res.status(404).send(`
      <html>
        <head>
          <title>404 Not Found</title>
          <style>
            body { font-family: Arial; background: #f5f5f5; padding: 20px; }
            .box {
              background: white; padding: 20px; border-radius: 8px;
              max-width: 500px; margin: auto; text-align: center;
              box-shadow: 0 3px 10px rgba(0,0,0,0.2);
            }
            .btn {
              display: inline-block; margin-top: 15px; padding: 10px 15px;
              background: #007bff; color: white; text-decoration: none;
              border-radius: 6px;
            }
          </style>
        </head>
        <body>
          <div class="box">
            <h2>404 – Firmware Tidak Ditemukan</h2>
            <p>File <b>esp32.bin</b> tidak tersedia di server.</p>
            <a href="javascript:history.back()" class="btn">Kembali</a>
          </div>
        </body>
      </html>
    `);
  }

  // 2️⃣ Ambil data firmware + hitung SHA256
  const firmware = fs.readFileSync(FIRMWARE_FILE);
  const sha = crypto.createHash("sha256").update(firmware).digest("hex");

  const proto = req.headers["x-forwarded-proto"] || req.protocol;
  const host = `${proto}://${req.headers.host}`;
  const url = `${host}/esp32.bin`;


  // 3️⃣ Jika user membuka lewat browser → HTML + tombol Download
  if (req.headers["user-agent"] && !req.headers["user-agent"].includes("ESP")) {
    return res.send(`
      <html>
        <head>
          <title>Firmware Info</title>
          <style>
            body { font-family: Arial; padding: 20px; background: #f0f0f0; }
            .box { background:white; padding:20px; border-radius:8px; max-width:600px; margin:auto; }
            .btn { padding:10px 15px; background:#28a745; color:white; text-decoration:none; border-radius:6px; }
            pre { background:#eee; padding:10px; border-radius:6px; }
          </style>
        </head>
        <body>
          <div class="box">
            <h2>Firmware Ditemukan</h2>
             <div class="warn">
                ⚠️ <b>Peringatan!</b><br>
                Jangan menutup browser atau mematikan perangkat selama proses update berlangsung.
            </div>
            <p>Versi terbaru: <b>1.0.6</b></p>
            <p>SHA256:</p>
            <pre>${sha}</pre>
            <a class="btn" href="${url}" download>Download Firmware</a>
            <br><br>
            <a href="/" style="text-decoration:none;">⬅ Kembali</a>
          </div>
        </body>
      </html>
    `);
  }

  // 4️⃣ Jika diakses dari ESP32 → kirim JSON
  res.json({
    latest_version: "1.1.1",
    firmware_url: url,
    sha256: sha
  });
});

// =======================
//  FILE BIN
// =======================
app.get("/esp32.bin", (req, res) => {
  res.sendFile(__dirname + "/firmware/esp32.bin");
});

// =======================
//  JALANKAN SERVER
// =======================
app.listen(PORT, () =>
     getAllIPs().forEach(ip => {
        console.log(`${ip.iface}    http://${ip.address}:${PORT}`);
    })
);
