#pragma once
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <sstream>

#ifdef _WIN32
	#include <windows.h>
	#include <conio.h>
#else
	#include <termios.h>
	#include <unistd.h>
#endif

namespace{
	std::string input(const std::string& prompt = "",const std::string& data = "") {
		std::cout << "\033[1;1H\033[2K";
		std::cout << prompt << data << std::flush;
		std::string line = data;
		#ifdef _WIN32
			while (true) {
				char ch = _getch();
				if (ch == '\r') {
					std::cout << '\n';
					break;
				} else if (ch == 8 || ch == 127) { // Backspace
					if (!line.empty()) {
						line.pop_back();
						std::cout << "\b \b";
					}
				} else if (ch >= 32 && ch <= 126) { // Printable characters
					line += ch;
					std::cout << ch;
				}
			}
		#else
			struct termios oldt, newt;
			tcgetattr(STDIN_FILENO, &oldt);
			newt = oldt;
			newt.c_lflag &= ~(ICANON | ECHO);
			tcsetattr(STDIN_FILENO, TCSANOW, &newt);
			while (true) {
				char ch;
				if (read(STDIN_FILENO, &ch, 1) <= 0) break;
				if (ch == '\n') {
					std::cout << '\n';
					break;
				} else if (ch == 127 || ch == '\b') {
					if (!line.empty()) {
						line.pop_back();
						std::cout << "\b \b" << std::flush;
					}
				} else if (ch >= 32 && ch <= 126) {
					line += ch;
					std::cout << ch << std::flush;
				}
			}
			tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		#endif
		return line;
	}
};
class DVA {
public:
	DVA() : running(false) {
		running = true;
		inputThread = std::thread(&DVA::loop, this);
	}
	~DVA(){
		running = false;
		inputThread.join();
	}
	
	void addInt(std::string name, int* v){
		Value val;
		val.ip=v;
		std::lock_guard<std::mutex> lock(varMutex);
		variables[name] = val;
	}
	void addUint(std::string name, uint* v){
		Value val;
		val.up=v;
		std::lock_guard<std::mutex> lock(varMutex);
		variables[name] = val;
	}
	void addFloat(std::string name, float* v){
		Value val;
		val.fp=v;
		std::lock_guard<std::mutex> lock(varMutex);
		variables[name] = val;
	}
	void addDouble(std::string name, double* v){
		Value val;
		val.dp=v;
		std::lock_guard<std::mutex> lock(varMutex);
		variables[name] = val;
	}
	void addBool(std::string name, bool* v){
		Value val;
		val.bp=v;
		std::lock_guard<std::mutex> lock(varMutex);
		variables[name] = val;
	}
	void addString(std::string name, std::string* v){
		Value val;
		val.sp=v;
		std::lock_guard<std::mutex> lock(varMutex);
		variables[name] = val;
	}
	
private:
	struct Value{
		int* ip = nullptr;
		uint* up = nullptr;
		float* fp = nullptr;
		double* dp = nullptr;
		bool* bp = nullptr;
		std::string* sp = nullptr;
	};
	
	std::unordered_map<std::string, Value> variables;
	std::mutex varMutex;
	std::atomic<bool> running;
	std::thread inputThread;
	
	void handleCommand(const std::string& line) {
		std::istringstream iss(line);
		std::string cmd;
		iss >> cmd;
		
		if (cmd == "list") {
			std::lock_guard<std::mutex> lock(varMutex);
			for (auto& [name, var] : variables) {
				std::cout << name << " = ";
				if(var.ip){
					std::cout << (*var.ip);
				}else if(var.up){
					std::cout << (*var.up);
				}else if(var.fp){
					std::cout << (*var.fp);
				}else if(var.dp){
					std::cout << (*var.dp);
				}else if(var.bp){
					std::cout << ((*var.bp)?"true":"false");
				}else if(var.sp){
					std::cout << "\""<<(*var.sp)<<"\"";
				}
				std::cout<<std::endl;
			}
		} else if (cmd == "set") {
			std::string name, val;
			iss >> name;
			std::getline(iss, val);
			val.erase(0, val.find_first_not_of(" \t"));
			std::lock_guard<std::mutex> lock(varMutex);
			auto it = variables.find(name);
			if (it != variables.end()) {
				Value v = it->second;
				if(v.ip){
					(*v.ip) = std::stoi(val);
				}else if(v.up){
					(*v.up) = std::stoul(val);
				}else if(v.fp){
					(*v.fp) = std::stof(val);
				}else if(v.dp){
					(*v.dp) = std::stod(val);
				}else if(v.bp){
					if(val=="1"||val=="true"){
						(*v.bp) = true;
					}else{ (*v.bp) = false; }
				}else if(v.sp){
					(*v.sp) = val;
				}
			} else {
				std::cout << "Variable not found.\n";
			}
		} else if (cmd == "help") {
			std::cout << "Commands:\n";
			std::cout << "  list             Show all linked variables\n";
			std::cout << "  set name value   Change a value\n";
			std::cout << "  help             Show this help message\n";
		} else {
			std::cout << "Unknown command.\n";
		}
	}
	void loop(){
		std::string str;
		while(running){
			str = input("> ");
			std::cout << "\033[2;1H\033[J";
			handleCommand(str);
		}
	}
};
