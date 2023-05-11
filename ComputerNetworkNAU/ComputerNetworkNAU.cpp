#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out) {
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

json fetchData(const std::string& countryName) {
    const std::string url = "https://restcountries.com/v3.1/name/" + countryName;
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return nullptr;
    }
    curl_easy_cleanup(curl);

    return json::parse(response);
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::string countryName;
    std::cout << "Enter country name: ";
    std::getline(std::cin, countryName);

    json data = fetchData(countryName);

    if (data.is_null() || data.empty()) {
        std::cerr << "Failed to fetch data or country not found" << std::endl;
        return 1;
    }

    std::string country = data[0]["name"]["common"];
    std::string capital = data[0]["capital"][0];
    std::string region = data[0]["region"];

    std::cout << "Country: " << country << std::endl;
    std::cout << "Capital: " << capital << std::endl;
    std::cout << "Region: " << region << std::endl;

    curl_global_cleanup();
    return 0;
}
