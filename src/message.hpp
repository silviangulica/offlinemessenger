#pragma once 
#include <string>
#include <ctime>

class Message
{
public:
    Message(std::string message, std::string sender, std::string receiver);

    // Conversie la string
    std::string toString();

    // Getters and setters
    std::string getMessage() { return message; }
    std::string getSender() { return sender; }
    std::string getReceiver() { return receiver; }
    std::string getDateTime() { return date_time; }

    void setMessage(std::string message) { this->message = message; }
    void setSender(std::string sender) { this->sender = sender; }
    void setReceiver(std::string receiver) { this->receiver = receiver; }
    void setDateTime(std::string date_time) { this->date_time = date_time; }


private:
    std::string message;
    std::string sender;
    std::string receiver;
    std::string date_time;
};