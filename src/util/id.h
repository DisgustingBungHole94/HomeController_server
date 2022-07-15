#pragma once

#include <string>
#include <vector>

class ID {
    public:
        ID();
        ~ID();

        void generate();
        void parse(const std::vector<uint8_t>& bytes);

        std::string str() { return m_id; }

    private:
        std::string m_id;
}; 