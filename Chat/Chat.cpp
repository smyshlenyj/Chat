#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <unordered_map>

std::unordered_map<std::string, int> populate_users() {
	std::unordered_map<std::string, int> return_map;
	std::ifstream readFromDB;
	readFromDB.open("users.mdf", std::ios::in);
	int counter_of_lines = 0;
	std::string line;
	std::string user;
	while (!readFromDB.eof())
	{
		std::getline(readFromDB, line);
		for (auto i : line) {
			if (i != '$') user.push_back(i);
			else break;
		}
		return_map.insert({ user, counter_of_lines });
		++counter_of_lines;
		user.clear();
	}
	return return_map;
}

std::unordered_map<std::string, int> g_users = populate_users();


bool is_valid_login(std::string login)
{
	for (auto i : login) if (i == '$') return false;
	if (login == "") return false;
	return true;
}

bool is_valid_password(std::string password)
{
	for (auto i : password) if (i == '$') return false;
	if (password == "") return false;
	return true;
}

bool is_unique_login(std::string login)
{
	std::unordered_map<std::string, int>::iterator it;
	it = g_users.find(login);
	if (it != g_users.end()) return false;
	return true;
}

std::pair<std::string, std::string> input() {
	std::string login, password;

	std::cout << "Enter login: ";
	std::cin >> login;
	std::cout << '\n';

	if (!is_valid_login(login)) {
		std::cout << "Invalid Login.\n";
		return { "", "" };
	}

	std::cout << "Enter password: ";
	std::cin >> password;
	std::cout << '\n';

	if (!is_valid_password(password)) {
		std::cout << "Invalid password.\n";
		return { "", "" };
	}

	return { login, password };
}

bool sign_up() {
	std::pair<std::string, std::string> logpas = input();
	if (!is_unique_login(logpas.first)) {
		std::cout << "Invalid Login.\n";
		return false;
	}

	std::ofstream out("users.mdf", std::ios::app);

	if (out.is_open()) {
		out << logpas.first << "$" << logpas.second << '\n';  //add new user to file
		std::cout << "Success! You are registered.\n";
		g_users = populate_users(); //add new user to map	


		for (std::unordered_map<std::string, int>::const_iterator it = g_users.begin(); it != g_users.end(); ++it)
		{
			std::cout << it->first << "\n";
		}
		std::cout << '\n' << "and" << '\n';
		for (std::unordered_map<std::string, int>::const_iterator it = g_users.begin(); it != g_users.end(); ++it)
		{
			std::cout << it->second << "\n";
		}
		return true;
	}
	else {
		std::cout << "Something went wrong.\n";
		return false;
	}
}

std::string sign_in() {
	std::pair<std::string, std::string> logpas = input();

	// if (logpas.first == "") return "";


	std::unordered_map<std::string, int>::const_iterator it = g_users.find(logpas.first);
	if (it == g_users.end()) {
		std::cout << "Such user doesn't exist.\n";
		return "";
	}

	std::ifstream readFromDB;
	readFromDB.open("users.mdf", std::ios::in);
	std::string line;
	std::string password = "";

	int current_line = 0;
	while (!readFromDB.eof()) {
		std::getline(readFromDB, line);
		if (it->second == current_line) {
			for (int i = 0; i < line.size(); ++i) {
				if (line[i] == '$') {
					for (int j = i + 1; j < line.size(); ++j) {
						password.push_back(line[j]);
					}
					break;
				}
			}
			break;
		}
		current_line++;
	}
	if (password != logpas.second) {
		std::cout << "Incorrect password.\n";
		return "";
	}

	return logpas.first;
}

struct Message
{
private:
	bool groupChat;
	int idSender, idRecipient;
	std::string message, chatName;

public:
	Message(int _idSender, int _idRecipient, std::string _message) : idSender(_idSender), idRecipient(_idRecipient), message(_message)
	{
		groupChat = false;
		chatName = "-";
	}

	Message(bool _groupChat, std::string _chatName, int _idSender, std::string _message) : groupChat(_groupChat), chatName(_chatName), idSender(_idSender), message(_message)
	{
		idRecipient = 0; //need to reserve 0 for public chat
	}

	void sendMessage() // push message to data base
	{
		std::ofstream out("messages.mdf", std::ios::app);
		if (out.is_open())
		{
			out << chatName << "$" << groupChat << "$" << idSender << "$" << idRecipient << "$" << message << std::endl;
		}
		out.close();
	}
};

struct Chat
{
	std::list<std::string> buffer;

	Chat(int _id1, int _id2) // private chat constructor
	{
		std::ifstream readFromDB;
		readFromDB.open("messages.mdf", std::ios::in);

		while (!readFromDB.eof())
		{
			std::string array[5];
			std::string msg;

			std::getline(readFromDB, msg);
			if (!msg.empty())
			{
				std::istringstream ss(msg);
				std::string token;
				int i = 0; // iterator for while
				while (std::getline(ss, token, '$'))
				{
					array[i] = token;
					++i;
				}
				// showing messages linked with current users
				if ((std::stoi(array[2]) == _id1 && std::stoi(array[3]) == _id2) || (std::stoi(array[2]) == _id2 && std::stoi(array[3]) == _id1))
					buffer.push_back("From: " + array[2] + " To: " + array[3] + " Message: " + array[4]);
			}
		}
	}

	Chat(std::string _chatName) // public chat constructor
	{
		std::ifstream readFromDB;
		readFromDB.open("messages.mdf", std::ios::in);
		std::string array[5];

		while (!readFromDB.eof())
		{
			std::string msg;
			std::getline(readFromDB, msg);

			if (!msg.empty())
			{
				std::istringstream ss(msg);
				std::string token;
				int i = 0; // iterator for while
				while (std::getline(ss, token, '$'))
				{
					array[i] = token;
					++i;
				}

				if (array[0] == _chatName) // showing messages linked with current chat
					buffer.push_back("Public chat: " + array[0] + " From: " + array[2] + " Message: " + array[4]);
			}
		}
	}

	// std::list<std::string> getChat() { return buffer; } not used

	void print()
	{
		for (auto const& i : buffer) {
			std::cout << i << std::endl;
		}
	}
};

int main()
{
	for (std::unordered_map<std::string, int>::const_iterator it = g_users.begin(); it != g_users.end(); ++it)
	{
		std::cout << it->first << "\n";
	}
	std::cout << '\n' << "and" << '\n';
	for (std::unordered_map<std::string, int>::const_iterator it = g_users.begin(); it != g_users.end(); ++it)
	{
		std::cout << it->second << "\n";
	}

	int input;
	std::string current_user;
	bool escape = true;
	while (escape) {
		std::cout << "sign in, sign up, or leave: Enter '0', '1' or '2'\n";
		std::cin >> input;
		switch (input) {
		case 0: {
			current_user = sign_in();
			if (current_user != "") escape = false;
		}; break;
		case 1: { if (sign_up()) break; }; break;
		case 2: { escape = false; }; break;
		default: std::cout << "Invalid input\n"; break;
		}
	}

	std::cout << '\n' << current_user << '\n';

	// Message test = { 56, 65, "hello world!"};
	// test.sendMessage();
	// Chat ch = { 56, 65 };
	// ch.print();
	// Chat common = { "00-general" };
	// common.print();
	// Message testGroup = { true, "00-general", 3, "public chat \"TEST message" };
	// testGroup.sendMessage();
}
