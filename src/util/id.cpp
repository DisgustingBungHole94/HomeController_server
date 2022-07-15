#include "id.h"

#include <random>

ID::ID() {}
ID::~ID() {}

void ID::generate() {
    static const char charSet[] = 
    {   
        '0','1','2','3','4',
        '5','6','7','8','9',
        'A','B','C','D','E','F',
        'G','H','I','J','K',
        'L','M','N','O','P',
        'Q','R','S','T','U',
        'V','W','X','Y','Z',
        'a','b','c','d','e','f',
        'g','h','i','j','k',
        'l','m','n','o','p',
        'q','r','s','t','u',
        'v','w','x','y','z'
    };

    static const int ID_LENGTH = 32;

    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, sizeof(charSet) - 1);

    std::string id;
    id.reserve(ID_LENGTH);

    for(int i = 0; i < ID_LENGTH; i++) {
        id += charSet[dist(rng)];
    }

    m_id = id;
}

void ID::parse(const std::vector<uint8_t>& bytes) {
    m_id = std::string(bytes.begin(), bytes.end());
}