#include "config.h"

#include <fstream>
#include <vector>

#include "string.h"

Config::Config() {}
Config::~Config() {}

void Config::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw GeneralException("Failed to open file " + path + ".", "Config::load");
    }

    int lineNum = 0;
    for (std::string line; std::getline(file, line); ) {
        lineNum++;

        if (line.length() < 1 || line[0] == '#') {
            continue;
        }

        std::vector<std::string> splitLine = Util::splitString(line, ' ');
        if (splitLine.size() < 2) {
            throw GeneralException("Variable not defined: " + splitLine[0] + " (" + std::to_string(lineNum) + ")", "Config::load");
        }

        ConfigValue val(splitLine[0]);
        int i;

        try {
            i = std::stoi(splitLine[1]);
            val.setInt(i);
        }
        catch (std::invalid_argument const& e) {
            val.setString(splitLine[1]);
        }
        catch (std::out_of_range const& e) {
            throw GeneralException("Variable integer out of range: " + splitLine[0] + " (" + std::to_string(lineNum) + ")", "Config::load");
        }

        m_vars.insert(std::make_pair(splitLine[0], val));
    }
}

ConfigValue& Config::operator[](const std::string& var) {
    auto mit = m_vars.find(var);
    if (mit == m_vars.end()) {
        throw GeneralException("Variable not defined: " + var, "ConfigValue::operator[]");
    }

    return mit->second;
}