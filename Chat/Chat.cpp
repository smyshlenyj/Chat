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

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

struct Message
{
private:
	bool groupChat;
	std::string sender, recipient, message, chatName;

public:
	Message(std::string _sender, std::string _recipient, std::string _message) : sender(_sender), recipient(_recipient), message(_message)
	{
		groupChat = false;
		chatName = "-";
	}

	Message(bool _groupChat, std::string _chatName, std::string _sender, std::string _message) : groupChat(_groupChat), chatName(_chatName), sender(_sender), message(_message)
	{
		recipient = "_all"; //need to reserve name for public chat
	}

	void sendMessage() // push message to data base
	{
		std::ofstream out("messages.mdf", std::ios::app);
		if (out.is_open())
		{
			out << chatName << "\t" << groupChat << "\t" << sender << "\t" << recipient << "\t" << message << std::endl;
		}
		out.close();
	}
};

struct Chat
{
	std::list<std::string> buffer;

	Chat(std::string _sender, std::string _recipient) // private chat constructor
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
				while (std::getline(ss, token, '\t'))
				{
					array[i] = token;
					++i;
				}
				// showing messages linked with current users
				if ((array[2] == _sender && array[3] == _recipient) || (array[2] == _recipient && array[3] == _sender))
					buffer.push_back("From: " + array[2] + "\tTo: " + array[3] + "\tMessage: " + array[4]);
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
				while (std::getline(ss, token, '\t'))
				{
					array[i] = token;
					++i;
				}

				if (array[0] == _chatName) // showing messages linked with current chat
					buffer.push_back("Public chat: " + array[0] + "\tFrom: " + array[2] + "\tMessage: " + array[4]);
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
	int input;
	std::string current_user;
	bool alive = true;
	while (alive) {
		std::cout << "Welcome to Stack, next generation messenger!\n\n";
		std::cout << "\nPress\n '1' for sign in\n '2' for sign up\n '3' for exit\n";
		std::cin >> input;
		switch (input) {
		case 1: {
			current_user = sign_in();
			if (current_user != "") alive = false;

			std::cout << "Hi, " << current_user << ", please type recipient name or public chat name: ";
			std::string inputRecipient;
			std::cin >> inputRecipient;
			std::cout << '\n' << "Last messages in chat: " << '\n';
			Chat tempChat = { current_user, inputRecipient };
			tempChat.print();
			std::cout << '\n' << "Type your message here: " << '\n';
			std::string inputMessage;
			std::cin.ignore();
			std::getline(std::cin, inputMessage, '\n');
			Message tempMessage = { current_user, inputRecipient, inputMessage };
			tempMessage.sendMessage();
			std::cout << '\n' << "Your message has been sent. 50 cents will be debited from your account." << '\n';
		}; break;
		case 2: { if (sign_up()) break; }; break;
		case 3: { alive = false; }; break;
		default: std::cout << "Invalid input\n"; break;
		}
	}
}