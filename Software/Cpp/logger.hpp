#pragma once
#include <functional>
#include <string>
#include <fstream>
#include <iostream>

class Logger {
public:
	Logger(std::string filename);
    ~Logger();
	void SetCallback(int timeout, std::function<void()> callback);
    std::string GetFilePath();
    void Log(std::string data);
private:
    std::ofstream file_descriptor_;
    std::string current_date_;
    std::string file_name_;
    std::string file_path_;
    std::string GetDateString();
    bool CheckOpenNewFile();
    std::ofstream TryOpenFile(std::string path);
};
