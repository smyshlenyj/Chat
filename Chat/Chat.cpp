#include <iostream>
#include <fstream>
#include <list>
#include <sstream>

struct User
{
	std::string login, password, userName;

	User(std::string const& _login, std::string const& _password, std::string const& _userName) : login(_login), password(_password), userName(_userName)
	{
	}

	User(std::string const& _login, std::string const& _password) : login(_login), password(_password)
	{
	}

	User()
	{
	}

	bool loginValid(std::string const& _login)
	{
		for (auto i : login) if (i == '\t') return false;
		if (_login == "" || _login == "_all") return false;
		return true;
	}

	bool passwordValid(std::string const& _password)
	{
		if (_password == "") return false;
		return true;
	}

	std::string getLogin()
	{
		return login;
	}

	std::string getPassword()
	{
		return password;
	}

	std::string getUserName()
	{
		return userName;
	}

	void setLogin(std::string const& _login)
	{
		login = _login;
	}

	void setPassword(std::string const& _password)
	{
		password = _password;
	}

	void setUserName(std::string const& _userName)
	{
		userName = _userName;
	}

};

struct Users
{
	std::list<User> users;

	Users()
	{
		std::ifstream readFromDB;
		readFromDB.open("users.mdf", std::ios::in);
		if (!readFromDB.is_open())
			std::ofstream outfile("users.mdf"); //create file in case it's not there
		else
		{
			while (!readFromDB.eof())
			{
				std::string array[3];
				std::string userRecord;

				std::getline(readFromDB, userRecord);
				if (!userRecord.empty())
				{
					std::istringstream ss(userRecord);
					std::string token;
					int i = 0; // iterator for while
					while (std::getline(ss, token, '\t'))
					{
						array[i] = token;
						++i;
					}
					User tempUser = { array[0], array[1], array[2] };
					users.push_back(tempUser);
				}
			}
		}
	}

	bool uniqueLogin(std::string const& login)
	{
		for (auto& i : users)
		{
			if (i.getLogin() == login)
				return false;
		}
		return true;
	}

	void printUsers()
	{
		for (auto i : users) {
			std::cout << "User: " << i.getLogin() << ", Name: " << i.getUserName() << '\n';
		}
	}
};

Users loadedUsers = Users();   // создать объект Users

User input()
{
	std::string login, password;
	User tempUser = { login, password };

	std::cout << "Enter login: ";
	try
	{

		std::cin >> login;
		if (login == "_all")
			throw (login);
	}

	catch (std::string const& login)
	{
		std::cout << "Exception! Please don't use " + login + " as login, it is hardcoded!\n";
	}

	std::cout << '\n';
	tempUser.setLogin(login);

	if (!tempUser.loginValid(login)) // ???????????????????????????
	{
		std::cout << "Invalid Login.\n";
		return User();
	}

	std::cout << "Enter password: ";
	std::cin >> password;
	std::cout << '\n';
	tempUser.setPassword(password);

	if (!tempUser.passwordValid(password)) // ?????????????????????????????
	{
		std::cout << "Invalid password.\n";
		return User();
	}

	return tempUser;
}

bool signUp()
{
	User user = input();

		if (!loadedUsers.uniqueLogin(user.getLogin()))
		{
			std::cout << "Invalid Login.\n";
			return false;
		}

	std::string name;
	std::cout << "Enter your name: ";
	std::cin.ignore();
	std::getline(std::cin, name, '\n');
	user.setUserName(name);
	std::cout << '\n';

	std::cout << "Check your entries.\nYour login: " << user.getLogin()
		<< "\nYour password: " << user.getPassword()
		<< "\nYour name: " << user.getUserName()
		<< "\nare those correct? Enter 'y' to proceed, 'q' to abort\n";
	std::string input;
	std::cin >> input;
	if (input != "y") {
		std::cout << "aborted\n";
		return false;
	}

	std::ofstream out("users.mdf", std::ios::app);

	if (out.is_open()) {
		out << user.getLogin() + "\t" + user.getPassword() + "\t" + user.getUserName() << std::endl;  //add new user to file
		std::cout << "Success! You are registered.\n";
		return true;
	}
	else {
		std::cout << "Something went wrong.\n";
		return false;
	}
}

User signIn() {
	User user = input();

		if (loadedUsers.uniqueLogin(user.getLogin()))
			std::cout << "Such user doesn't exist.\n";
		else return user;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

struct Message
{
private:
	std::string sender, recipient, message;

public:
	Message(std::string const& _sender, std::string const& _recipient, std::string const& _message) : sender(_sender), recipient(_recipient), message(_message)
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

	Chat(std::string const& _sender, std::string const& _recipient) // universal chat constructor
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
	bool alive = true;
	bool openSession = false;
	bool openChat = false;
	while (alive) {
		std::cout << "\nWelcome to Stack, past generation messenger!\n\n";
		std::cout << "\nPress\n '1' for sign in\n '2' for sign up\n 'q' for exit\n";
		std::cin >> input;
		switch (input) {
		case '1': {
			User currentUser = signIn();
			openSession = true;
			if (currentUser.getLogin() == "")
				openSession = false;

			while (openSession) {
				std::cout << "\nRegistered users:\n";
				loadedUsers.printUsers();
				std::cout << "\n";
				std::cout << "Hi, " << currentUser.getLogin() << ", please type recipient name or 'p' for public chat or 'q' to exit to main menu: ";
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
					messageMenu(true, openSession, currentUser.getLogin(), "_all");
					continue;
				}

				else
					openChat = true;

				if (!loadedUsers.uniqueLogin(inputRecipient)) messageMenu(openChat, openSession, currentUser.getLogin(), inputRecipient);
				else
				{
					openChat = false;
					std::cout << "User " << inputRecipient << " not found, please try again \n";
				}
			}
		}; break;
		case '2': { if (signUp())
		{
			loadedUsers = Users();
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