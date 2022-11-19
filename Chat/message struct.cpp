#include <iostream>
#include <fstream>

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



int main()
{
    std::cout << "Hello World!\n";
    Message test = { 1, 2, "hello world!" };
    test.sendMessage();
}
