#include "logger.hpp"
#include <chrono>
#include <ctime>

Logger::Logger(std::string filename){
    current_date_ = GetDateString();
    file_name_ = filename;
    file_path_ = file_name_ + "_" + current_date_ + ".txt";
    
    // std::cout << "Created logger: " << filename << std::endl;
    
    do{
        // std::cout << "Trying to open: " << file_path_ << std::endl;
        file_descriptor_.open(file_path_, std::ios::app);
    }while (!file_descriptor_.is_open());
}

Logger::~Logger(){
    // std::cout << "Closing: " << file_path_ << std::endl;
    file_descriptor_.close();
}

std::string Logger::GetDateString(){
    auto time = std::chrono::system_clock::now();
	std::time_t time_decoded = std::chrono::system_clock::to_time_t(time);
	struct tm* t = localtime(&time_decoded);
    
    char result[32];
    snprintf(result, 32, "%.4d-%.2d-%.2d", t->tm_year - 100 + 2000, t->tm_mon + 1, t->tm_mday);
    return std::string(result);
}

std::string Logger::GetFilePath(){
    return file_path_;
}

void Logger::Log(std::string data){
    CheckOpenNewFile();
    if(file_descriptor_.is_open()){
        file_descriptor_ << data;
        file_descriptor_.flush();
    }
}

bool Logger::CheckOpenNewFile(){
    std::string new_date = GetDateString();
    
    if (new_date != current_date_){
        // std::cout << "New file needed" << std::endl;
        current_date_ = new_date;
        if(file_descriptor_.is_open()){
            // std::cout << "Closing: " << file_path_ << std::endl;
            file_descriptor_.close();
        }
        
        file_path_ = file_name_ + "_" + current_date_ + ".txt";
        
        do{
            // std::cout << "Trying to open: " << file_path_ << std::endl;
            file_descriptor_.open(file_path_, std::ios::app);
        }while (!file_descriptor_.is_open());
     
        return true;
    }
    return false;
}