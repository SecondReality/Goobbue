#include "jsonLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>

Json::Value loadJson(const std::string& filePath)
{
    std::ifstream fileStream(filePath);
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    fileStream.close();

    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(buffer.str(), root);

    if (!parsingSuccessful)
    {
        std::cout << "Error parsing json file at " << filePath << std::endl;
    }

    return root;
}
