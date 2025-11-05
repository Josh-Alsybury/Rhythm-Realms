#include <cpr/cpr.h>
#include <iostream>

int main() {
    auto response = cpr::Get(cpr::Url{ "http://127.0.0.1:8888/callback" });
    std::cout << "Status code: " << response.status_code << std::endl;
    std::cout << "Body (first 200 chars): " << response.text.substr(0, 200) << "..." << std::endl;
}
