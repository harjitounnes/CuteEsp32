// baca variabel dari server
const currentVersion = window.currentVersion;
const latestVersion = window.latestVersion;
const updateAvailable = window.updateAvailable;

document.getElementById('current-version').textContent = currentVersion;
document.getElementById('latest-version').textContent = latestVersion;

const msgEl = document.getElementById('update-msg');
const btnEl = document.getElementById('update-button');

if (updateAvailable) {
    msgEl.textContent = "Update tersedia!";
    msgEl.style.color = "red";
    btnEl.innerHTML = '<a href="/update"><button>Update Now</button></a>';
} else {
    msgEl.textContent = "Firmware sudah terbaru.";
    msgEl.style.color = "green";
    btnEl.innerHTML = '';
}
