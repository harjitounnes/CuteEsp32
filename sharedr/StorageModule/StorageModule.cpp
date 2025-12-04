#include "StorageModule.h"
#include <sys/stat.h>

static const char* TAG = "StorageModule";

StorageModule::StorageModule(bool formatOnFail)
    : _formatOnFail(formatOnFail) {}

bool StorageModule::begin() {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = basePath.c_str(),
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = _formatOnFail
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS (%s)", esp_err_to_name(ret));
        return false;
    }

    ESP_LOGI(TAG, "SPIFFS mounted at %s", basePath.c_str());
    return true;
}

std::string StorageModule::fullPath(const std::string& path) {
    // Jika path sudah absolute, kembalikan tanpa modifikasi
    if (!path.empty() && path[0] == '/')
        return basePath + path;
    return basePath + "/" + path;
}

bool StorageModule::exists(const std::string& path) {
    std::string fp = fullPath(path);
    struct stat st;
    return stat(fp.c_str(), &st) == 0;
}

cJSON* StorageModule::readJSON(const std::string& path) {
    std::string fp = fullPath(path);

    FILE* file = fopen(fp.c_str(), "r");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file for reading: %s", fp.c_str());
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);

    cJSON* json = cJSON_Parse(buffer);
    free(buffer);

    if (!json) {
        ESP_LOGE(TAG, "Failed to parse JSON: %s", fp.c_str());
        return nullptr;
    }

    return json;
}

bool StorageModule::writeJSON(const std::string& path, cJSON* root) {
    std::string fp = fullPath(path);

    char* out = cJSON_Print(root);
    if (!out) {
        ESP_LOGE(TAG, "cJSON_Print failed");
        return false;
    }

    FILE* file = fopen(fp.c_str(), "w");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", fp.c_str());
        free(out);
        return false;
    }

    fwrite(out, 1, strlen(out), file);
    fclose(file);
    free(out);

    return true;
}
