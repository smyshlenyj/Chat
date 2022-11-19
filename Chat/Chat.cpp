#include <iostream>
#include <fstream>
#include <list>
#include <sstream>

struct Message
{
private:
    int idSender, idRecipient;
    std::string message;

public:
    Message(int _idSender, int _idRecipient, std::string _message) : idSender(_idSender), idRecipient(_idRecipient), message(_message)
    {
    }

    void sendMessage() //пишем в БД (файл)
    {
        std::ofstream out ("messages.mdf", std::ios::app);
        if (out.is_open())
        {
            out << idSender << "$" << idRecipient << "$" << message << std::endl;
        }
        out.close();
    }
};

struct Chat
{
    std::list<std::string> buffer;

    Chat(int _idSender, int _idRecipient)
    {
        std::ifstream readFromDB;
        readFromDB.open("messages.mdf", std::ios::in);
        std::string array[3];
        int i = 0; // iterator for while
        while (!readFromDB.eof()) 
        {
            i = 0; // iterator for while
            std::string msg;
            std::getline(readFromDB, msg);

            std::istringstream ss(msg);
            std::string token;

            while (std::getline(ss, token, '$')) 
            {
                array[i] = token;
                ++i;
            }

            buffer.push_back("From: " + array[0] + " To: " + array[1] + " Message: " + array[2]);
        }
    }
    std::list<std::string> getChat() { return buffer; }

    void print()
    {
        for (auto const& i : buffer) {
            std::cout << i << std::endl;
        }
    }
};


int main()
{
    //std::cout << "Hello World!\n";
    Message test = { 1, 2, "hello world!" };
    test.sendMessage();
    Chat ch = {1, 2};
    ch.print();

}
