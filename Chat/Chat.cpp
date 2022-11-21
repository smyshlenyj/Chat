#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <unordered_map>

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
		std::ofstream outfile ("users.mdf"); //create file in case it's not there
	while (!readFromDB.eof()) 
	{
		std::getline(readFromDB, line);
		for (auto i : line) {
			if (i != '$') user.push_back(i);
			else break;
		}
		for (auto i = 32; i < 64; i++) {
			if (line[i] != '$') name.push_back(line[i]);
			else break;
		}
		return_map.insert( { user, std::make_pair(name, counter_of_lines) } );
		++counter_of_lines;
		user.clear();
		name.clear();
	}

	{ //delete last empty member
		auto it = return_map.find("");
		it = return_map.erase(it);
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

std::pair<std::string, std::string> input(){
	std::string login, password;
	
	{
		std::cout << "Enter login: ";
		std::cin >> login;
		std::cout << '\n';

		if (!is_valid_login(login)) {
			std::cout << "Invalid Login.\n";
			return {"", ""};
		}
	}

	{
		std::cout << "Enter password: ";
		std::cin >> password;
		std::cout << '\n';
		
		if (!is_valid_password(password)) {
			std::cout << "Invalid password.\n";
			return {"", ""};
		}
  }

	return {login, password};
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
	
	std::ofstream out ("users.mdf", std::ios::app);

	if (out.is_open()) {
		out << logpas.first;  //add new user to file
		for (auto i = logpas.first.size(); i < 32; i++) out << '$';
		out << name;
		for (auto i = 32 + name.size(); i < 64; i++) out << '$';
		out << logpas.second << '\n';
		std::cout << "Success! You are registered.\n";
		// g_users = populate_users(); //add new user to map	

		// for(auto it = g_users.begin(); it != g_users.end(); ++it)
		// {
		// 	std::cout << it->first << "\n";
		// }
		// std::cout << '\n' << "and" << '\n';
		// for(auto it = g_users.begin(); it != g_users.end(); ++it)
		// {
		// 	std::cout << it->second.first << "\n";
		// }	
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
			out << chatName << "$" << groupChat << "$" << sender << "$" << recipient << "$" << message << std::endl;
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
				while (std::getline(ss, token, '$'))
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
				while (std::getline(ss, token, '$'))
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
	bool alive = true;
	while (alive) {
		std::cout << "\nPress\n '1' for sign in\n '2' for sign up\n '3' for exit\n";
		std::cin >> input;
		switch (input) {
		case 1: {
			current_user = sign_in();
			//if (current_user != "") alive = false;

			std::cout << "Hi, " << current_user << ", please type recipient name or public chat name: ";
			std::string inputRecipient;
			std::cin >> inputRecipient;
			std::cout << '\n' << "Last messages in chat: " << '\n';
			Chat temp = { current_user, inputRecipient };
			temp.print();
			std::cout << '\n' << "Type your message here: " << '\n';
			std::string inputMessage;
			std::cin.ignore();
			std::getline(std::cin, inputMessage, '\n');
			Message tempMessage = { current_user, inputRecipient, inputMessage };
			tempMessage.sendMessage();
		}; break;
		case 2: { if (sign_up()) break; }; break;
		case 3: { alive = false; }; break;
		default: std::cout << "Invalid input\n"; break;
		}
	}



	//Message test = { "test_sender", "test_recipient", "hello world!" };
	//test.sendMessage();
	//Chat ch = { "test_sender", "test_recipient" };
	//ch.print();
	// Chat common = { "00-general" };
	// common.print();
	// Message testGroup = { true, "00-general", 3, "public chat \"TEST message" };
	// testGroup.sendMessage();
}