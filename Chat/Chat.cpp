#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <unordered_map>
#include <algorithm>

typedef std::unordered_map<std::string, std::pair<std::string, int>> special_map;

special_map populate_users() {
	special_map return_map;
	int counter_of_lines = 0;
	std::string line;
	std::string user;
	std::string name;

	std::ifstream readFromDB;
	readFromDB.open("users.mdf", std::ios::in);

	if (!readFromDB.is_open())
		std::ofstream outfile("users.mdf"); //create file in case it's not there
	else {
		while (!readFromDB.eof())
		{
			std::getline(readFromDB, line);
			if (line != "") {
				for (auto i : line) {
					if (i != '$') user.push_back(i);
					else break;
				}
				for (auto i = 32; i < 64; i++) {
					if (line[i] != '$') name.push_back(line[i]);
					else break;
				}
				return_map.insert({ user, std::make_pair(name, counter_of_lines) });
				user.clear();
				name.clear();
			}
			++counter_of_lines;
		}
	}
	return return_map;
}

special_map g_users = populate_users();


bool is_valid_login(std::string login)
{
	for (auto i : login) if (i == '$') return false;
	if (login == "") return false;
	return true;
}

bool is_valid_password(std::string password)
{
	// for (auto i : password) if (i == '$') return false;
	if (password == "") return false;
	return true;
}

bool is_unique_login(std::string login)
{
	auto it = g_users.find(login);
	if (it != g_users.end()) return false;
	return true;
}

std::pair<std::string, std::string> input() {
	std::string login, password;

	{
		std::cout << "Enter login: ";
		std::cin >> login;
		std::cout << '\n';

		if (!is_valid_login(login)) {
			std::cout << "Invalid Login.\n";
			return { "", "" };
		}
	}

	{
		std::cout << "Enter password: ";
		std::cin >> password;
		std::cout << '\n';

		if (!is_valid_password(password)) {
			std::cout << "Invalid password.\n";
			return { "", "" };
		}
	}

	return { login, password };
}

bool sign_up() {
	std::pair<std::string, std::string> logpas = input();
	if (!is_unique_login(logpas.first)) {
		std::cout << "Invalid Login.\n";
		return false;
	}

	std::string name;
	std::cout << "Enter your name: ";
	std::cin >> name;
	std::cout << '\n';

	std::cout << "Check your entries.\nYour login: " << logpas.first
		<< "\nYour password: " << logpas.second
		<< "\nYour name: " << name
		<< "\nare those correct? Enter 'y' to proceed, 'q' to abort\n";
	std::string inputt;
	std::cin >> inputt;
	if (inputt != "y") {
		std::cout << "aborted\n";
		return false;
	}

	std::ofstream out("users.mdf", std::ios::app);

	if (out.is_open()) {
		out << logpas.first;  //add new user to file
		for (auto i = logpas.first.size(); i < 32; i++) out << '$';
		out << name;
		for (auto i = 32 + name.size(); i < 64; i++) out << '$';
		out << logpas.second << '\n';
		std::cout << "Success! You are registered.\n";
		return true;
	}
	else {
		std::cout << "Something went wrong.\n";
		return false;
	}
}

std::string sign_in() {
	std::pair<std::string, std::string> logpas = input();

	auto it = g_users.find(logpas.first);
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
		if (it->second.second == current_line) {
			for (auto i = 64; i < line.size(); ++i) {
				if (line[i] != '$') {
					password.push_back(line[i]);
				}
				else {
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

std::string get_name(std::string login) {
	auto it = g_users.find(login);
	if (it != g_users.end()) return it->second.first;
	else                     return "";
}

void list_users() {
	for (auto i : g_users) {
		std::cout << "User: " << i.first << ", Name: " << i.second.first << '\n';
	}
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

struct Message
{
private:
	std::string sender, recipient, message;

public:
	Message(std::string _sender, std::string _recipient, std::string _message) : sender(_sender), recipient(_recipient), message(_message)
	{
	}

	void sendMessage() // push message to data base
	{
		std::ofstream out("messages.mdf", std::ios::app);
		if (out.is_open())
		{
			out << sender << "\t" << recipient << "\t" << message << std::endl;
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
		if (!readFromDB.is_open())
			std::ofstream outfile("messages.mdf"); //create file in case it's not there
		else {
			while (!readFromDB.eof())
			{
				std::string array[3];
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

					if (array[1] == "_all" && _recipient == "_all") // showing messages linked with current chat
						buffer.push_back("Public chat\tFrom: " + array[0] + "\tMessage: " + array[2]);

					else if (_recipient != "_all" && ((array[0] == _sender && array[1] == _recipient) || (array[0] == _recipient && array[1] == _sender)))
						buffer.push_back("From: " + array[0] + "\tTo: " + array[1] + "\tMessage: " + array[2]);
				}
			}
		}
	}

	void printChat()
	{
		for (auto const& i : buffer) {
			std::cout << i << std::endl;
		}
	}
};

void messageMenu(bool openChat, bool openSession, std::string current_user, std::string inputRecipient)
{
	while (openChat && openSession) {
		std::cout << '\n' << "Last messages in chat: \n";
		Chat tempChat = { current_user, inputRecipient };
		tempChat.printChat();
		std::cout << '\n' << "Type your message here: \n";
		std::string inputMessage;
		std::cin.ignore();
		std::getline(std::cin, inputMessage, '\n');
		Message tempMessage = { current_user, inputRecipient, inputMessage };
		tempMessage.sendMessage();
		std::cout << '\n' << "Your message has been sent. 50 cents will be debited from your account.\n\nEnter '1' to type new message or 'q' to exit to chat menu\n";
		char messageMenuChoice;
		std::cin >> messageMenuChoice;
		switch (messageMenuChoice)
		{
		case '1':
		{
			continue;
		};
		case 'q':
		{
			openChat = false;
			break;
		};
		}
	}
}

int main()
{
	char input;
	std::string current_user;
	bool alive = true;
	bool openSession = false;
	bool openChat = false;
	while (alive) {
		std::cout << "Welcome to Stack, past generation messenger!\n\n";
		std::cout << "\nPress\n '1' for sign in\n '2' for sign up\n 'q' for exit\n";
		std::cin >> input;
		switch (input) {
		case '1': {
			current_user = sign_in();
			openSession = true;
			if (current_user == "")
				openSession = false;

			while (openSession) {
				std::cout << "\nRegistered users:\n";
				list_users();
				std::cout << "\n";
				std::cout << "Hi, " << current_user << ", please type recipient name or 'p' for public chat or 'q' to exit to main menu: ";
				std::string inputRecipient;
				std::cin >> inputRecipient;

				if (inputRecipient == "q")
				{
					inputRecipient.clear();
					openSession = false;
					break;
				}

				if (inputRecipient == "p")
				{
					messageMenu(true, openSession, current_user, "_all");
					continue;
				}

				else
				openChat = false;
				bool recipientExists = false;
				for (auto i : g_users)
				{
					if (i.first == inputRecipient)
					{
						openChat = true;
						recipientExists = true;
						break;
					}
				}

				if (recipientExists) messageMenu(openChat, openSession, current_user, inputRecipient);
				else std::cout << "User " << inputRecipient << " not found, please try again \n";

			}
		}; break;
		case '2': { if (sign_up())
		{
			g_users = populate_users();
			break;
		}; break;
		case 'q':
		{
			openSession = false;
			alive = false;
		}; break;
		default: std::cout << "Invalid input\n"; break;
		}
		}
	}
}