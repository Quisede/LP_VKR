#include "Base64.h"

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64Encode(const std::string& input) {
    std::string output;

    // val — «буфер», куда побитово складываются входные байты.
    // valb — счётчик валидных бит в val. Изначально -6, чтобы после первого байта (+8 бит) сразу стало 2, и цикл смог извлечь первые 6 бит.
    int val = 0, valb = -6;

    for(unsigned char c : input) {
        val = (val << 8) + c; // Сдвигаем буфер на 8 бит и добавляем новый байт
        valb += 8; // В буфере стало на 8 бит больше

        while (valb >= 0) { // Пока накопилось ≥6 бит
            output.push_back(base64_chars[(val >> valb) & 0x3F]); // Берём старшие 6 бит
            valb -= 6; // "Убираем" их из счётчика
        }
    }

    if (valb > -6) {
        output.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (output.size() % 4) output.push_back('=');

    return output;
}

std::string base64Decode(const std::string& input) {
    std::string output;
    std::vector<int> T(256, -1);

    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    int val = 0, valb = -8;

    for (unsigned char c : input) {
        if (T[c] == -1) break;

        val = (val << 6) + T[c];
        valb += 6;

        if (valb >= 0) {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return output;
}
