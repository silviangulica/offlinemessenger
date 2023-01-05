#include "message.hpp"

Message::Message(std::string message, std::string sender, std::string receiver)
{
    // Citim timpul curent
    time_t timp = time(nullptr);
    struct tm* timp_local = localtime(&timp);

    // Setam campurile
    this->message = message;
    this->sender = sender;
    this->receiver = receiver;
    this->date_time = "[" +
        std::to_string(timp_local->tm_mday) + "/" +
        std::to_string(timp_local->tm_mon + 1) + "/" +
        std::to_string(timp_local->tm_year + 1900) + " " +
        std::to_string(timp_local->tm_hour) + ":" +
        std::to_string(timp_local->tm_min) + ":" +
        std::to_string(timp_local->tm_sec) + "]";
}

std::string Message::toString()
{
    return "{" + sender + " -> " + receiver + "}: " + message + " " + date_time;
}

