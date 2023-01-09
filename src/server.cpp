#include "socket.hpp"
#include "database.hpp"
#include "message.hpp" 
#include <pthread.h>

// Declare database
Database database("../src/database");

// Function declaration
static void* handleNewConnection(void* arg);
void proccessCommand(std::string command, Socket* client);

int main() {
    // Cream un socket server pe portul 8080
    Socket server(8080);

    // Asteptam conexiuni
    while (true)
    {
        // Acceptam conexiunea
        Socket* client = new Socket(server.getFD());

        // Cream un nou thread pentru a se ocupa de conexiunea noua
        pthread_t thread;
        pthread_create(&thread, NULL, handleNewConnection, client);
        pthread_detach(thread);
    }

    return 0;
}

// Functie pentru a se ocupa de o noua conexiune
static void* handleNewConnection(void* arg)
{
    // Castam argumentul la Socket*
    Socket* client = (Socket*)arg;
    while (true) {
        std::string message;
        try
        {
            message = client->receiveMessage();
        }
        catch (std::exception& e)
        {
            // In cazul in care apare o exceptie, inchidem conexiunea si iesim din bucla
            client->closeConnection();
            break;
        }

        // Verifiam daca am primit ceva mesaj
        if (message.size() == 0 || message == ":exit")
        {
            // Verificam daca userul este logat
            User* user = database.getUserBySocket(client);
            if (user != NULL)
            {
                // Stergem userul din baza de date
                database.deleteAllMessages(user->getUsername());
                database.removeUser(user->getUsername());
            }

            // Inchidem conexiunea
            client->closeConnection();

            // Stergem obiectul client
            delete client;

            break;
        }

        try
        {
            proccessCommand(message, client);
        }
        catch (std::exception& e)
        {
            // In cazul in care apare o exceptie, inchidem conexiunea si iesim din bucla
            client->closeConnection();
            break;
        }
    }

    return NULL;
}


// Functie pentru procesarea comenzilor
void proccessCommand(std::string command, Socket* client)
{
    // Split the comand in words
    std::vector<std::string> words;
    std::string word;
    std::stringstream ss(command);
    while (ss >> word)
        words.push_back(word);

    // Verificam daca am instroduct comanda "login"
    if (words.at(0) == ":login")
    {
        std::string login_error_msg = "* Comanda invalida, folositi :login <username> <password>";
        if (words.size() != 3)
        {
            client->sendMessage(login_error_msg);
            return;
        }

        if (database.getUser(words.at(1)) != NULL)
        {
            client->sendMessage("* Esti deja logat!");
            return;
        }
        // Verificam daca exista user
        User* user = database.findUser(words.at(1));

        // Verificam daca userul exista
        if (user == NULL)
        {
            client->sendMessage("* Userul nu exista!");
            return;
        }

        // Verificam daca parola este corecta
        if (user->getPassword() != words.at(2))
        {
            client->sendMessage("* Parola este incorecta!");
            return;
        }

        // Daca totul este ok, trimitem mesajul de succes
        client->sendMessage("* Te-ai logat cu succes!");

        // Setam userul ca fiind logat
        user->setLoggedIn(true);

        // Atribuim clientului userul
        user->setSocket(client);

        // Adaguam userul in vectorul de useri
        database.addUser(user);

        // Incarcam mesajele, daca sunt
        database.loadMessagesAndSend(user->getUsername(), client);
    }
    else if (words.at(0) == ":register") {

        std::string register_error_msg = "* Comanda invalida, folositi :register <username> <password>";
        if (words.size() != 3)
        {
            client->sendMessage(register_error_msg);
            return;
        }

        // Verificam daca acest user este deja logat
        if (database.getUser(words.at(1)) != NULL)
        {
            client->sendMessage("* Esti deja logat!");
            return;
        }

        // Verificam daca userul exista deja
        if (database.findUser(words.at(1)) != NULL)
        {
            client->sendMessage("* Userul exista deja!");
            return;
        }


        if (words.at(1).size() < 3 || words.at(1).size() > 12)
        {
            client->sendMessage("* Username-ul trebuie sa aiba intre [3, 12] caractere!");
            return;
        }

        if (words.at(2).size() < 3 || words.at(2).size() > 24)
        {
            client->sendMessage("* Parola trebuie sa aiba minim [3, 24] caractere!");
            return;
        }

        // Cream un nou user
        User* user = new User(words.at(1), words.at(2), client);
        user->setLoggedIn(true);
        user->setUserColor("\033[1;00m");
        user->setPersonalStatus("I am new! Online!");

        // Adaugam userul in baza de date
        database.addUser(user);
        database.createUserFile(user);

        // Trimitem mesajul de succes
        client->sendMessage("* Te-ai inregistrat cu succes!");
    }
    else if (words.at(0) == ":logout") {
        // Verificam daca userul este logat
        User* user = database.getUserBySocket(client);
        if (user == NULL)
        {
            client->sendMessage("* Esti deja delogat!");
            return;
        }

        // Stergem userul din baza de date
        database.deleteAllMessages(user->getUsername());
        database.removeUser(user->getUsername());

        // Trimitem mesajul de succes
        client->sendMessage("* Te-ai delogat cu succes!");
    }
    else if (words.at(0) == ":users") {
        // Verificam daca userul este logat
        User* user = database.getUserBySocket(client);
        if (user == NULL)
        {
            client->sendMessage("You are not logged in");
            return;
        }

        // Trimitem mesajul de succes
        client->sendMessage(database.getUsers());
    }
    else if (words.at(0) == ":send") {
        // Verificam daca userul este logat
        std::string send_error_msg = "* Comanda invalida, folositi :send <username> <message>";
        User* user = database.getUserBySocket(client);
        if (user == NULL) {
            client->sendMessage("* Nu esti logat!");
            return;
        }

        // Verificam daca a trimis cel putin 3 argumente, nu poti trimite ceva fara un mesaj in sine
        if (words.size() <= 2)
        {
            client->sendMessage(send_error_msg);
            return;
        }

        // Verificam daca a introdus "@all"
        if (words.at(1) == "@all")
        {
            std::string message = "";
            for (int i = 2; i < words.size(); i++)
                message += words.at(i) + " ";

            database.sendMessageToAll(user->getUsername(), message);
            return;
        }
        else {
            // Verificam daca userul exista
            User* receiver = database.findUser(words.at(1));
            if (receiver == NULL)
            {
                client->sendMessage("* Userul nu exista!");
                return;
            }

            // Verificam daca userul isi trimite singur mesaje
            if (receiver->getUsername() == user->getUsername())
            {
                client->sendMessage("* Nu-ti poti trimite mesaje singur!");
                return;
            }

            // Copiem restul cuvintelor in mesaj
            std::string message = "";
            for (int i = 2; i < words.size(); i++)
                message += words.at(i) + " ";

            std::string sender = user->getUserColor().c_str() + user->getUsername() + COLOR_DEFAULT;
            std::string reciver = receiver->getUserColor().c_str() + receiver->getUsername() + COLOR_DEFAULT;
            Message msg(message, sender, reciver);

            int number = database.getNumberOfMessages(receiver->getUsername());
            msg.setID(number);

            database.addMessageFile(&msg, receiver->getUsername());


            // Daca userul este logat ii trimitem mesajul
            User* temp_user = database.getUser(receiver->getUsername());
            if (temp_user != NULL)
                temp_user->getSocket()->sendMessage(msg.toString());
        }


    }
    else if (words.at(0) == ":help") {
        // Trimitem mesajul de succes
        std::string help_message = "Comenzi: \n* Comenzi pentru login | register | user settings : \n :login <username> <password> \n :register <username> <password> \n :logout \n :change [username|password|color|status] <value> \n\n\n* Comenzi pentru interatctiunea cu alti utilizatori : \n :users \n :send <username> <message> \n :reply <message ID> <message> \n :messages \n\n\n* Others :help \n:exit";
        client->sendMessage(help_message);
    }
    else if (words.at(0) == ":change") {
        std::string error_message = "* Comanda invalida, folositi :change [username|password|color|status] <value>";
        // Verificam daca userul este logat
        User* user = database.getUserBySocket(client);
        if (user == NULL) {
            client->sendMessage("* Nu esti logat!");
            return;
        }

        // Verificam daca a trimis cel putin 2 argumente
        if (words.size() <= 2)
        {
            client->sendMessage(error_message);
            return;
        }


        if (words.at(1) == "username") {
            if (words.size() > 3)
            {
                client->sendMessage(error_message);
                return;
            }
            // Verificam daca userul exista deja
            if (database.findUser(words.at(2)) != NULL)
            {
                client->sendMessage("* Userul exista deja!");
                return;
            }

            // Salvam vechiul username
            std::string oldUserName = user->getUsername();

            // Setam noul username
            user->setUsername(words.at(2));

            // Salvam modificarile
            database.updateUserFile(user, oldUserName);

            // Trimitem mesajul de succes
            client->sendMessage("* Username-ul a fost schimbat cu succes!");
        }
        else if (words.at(1) == "password") {
            if (words.size() > 3)
            {
                client->sendMessage(error_message);
                return;
            }
            // Setam noul password
            user->setPassword(words.at(2));

            // Salvam modificarile
            database.updateUserFile(user);

            // Trimitem mesajul de succes
            client->sendMessage("* Parola a fost schimbata cu succes!");
        }
        else if (words.at(1) == "color") {
            if (words.size() > 3)
            {
                client->sendMessage(error_message);
                return;
            }
            // Setam noul colorID
            std::string color = "";
            color = "\033[1;" + words.at(2) + "m";

            int colorID = std::stoi(words.at(2));


            std::string color_error_msg = "* Valoare invalida pentru culoare! \n Valori valide: 30-37, 90-97";
            if (colorID < 29 || colorID > 97 || (colorID > 37 && colorID < 90))
            {
                client->sendMessage(color_error_msg);
                return;
            }

            // Setam culoarea utilizatorului
            user->setUserColor(color);

            // Salvam modificarile
            database.updateUserFile(user);

            // Trimitem mesajul de succes
            client->sendMessage("* ColorID-ul a fost schimbat cu succes!");
        }
        else if (words.at(1) == "status") {
            // Setam noul status
            std::string status = "";
            for (int i = 2; i < words.size(); i++)
            {
                status += words.at(i) + " ";
            }
            if (status == "")
            {
                client->sendMessage(error_message);
                return;
            }
            else if (status.length() > 50) {
                client->sendMessage("* Error: Status-ul nu poate avea mai mult de 50 de caractere!");
                return;
            }
            user->setPersonalStatus(status);

            // Salvam modificarile
            database.updateUserFile(user);

            // Trimitem mesajul de succes
            client->sendMessage("* Status-ul a fost schimbat cu succes!");
        }
        else {
            client->sendMessage("* Comanda invalida, folositi :change [username|password|color|status] <value>");
        }
    }
    else if (words.at(0) == ":reply") {
        // Verificare corectitudine comanda
        if (words.size() < 3)
        {
            client->sendMessage("* Comanda invalida, folositi :reply <id> <message>");
            return;
        }

        if (words.at(1) == "-1")
        {
            client->sendMessage("* Nu puteti raspunde la un mesaj public!");
            return;
        }

        User* user = database.getUserBySocket(client);
        if (user == NULL) {
            client->sendMessage("* Nu esti logat!");
            return;
        }

        // Cautam ID-ul mesajului in baza de date
        // Cu remarci, se cauta doar continutul mesajului si expeditorul
        // Aceasta formatare se face in functia din baza de date de cautare
        std::string message_from_db = database.findMessage(words.at(1), user->getUsername());
        // Extragerea expeditorului din mesajul din baza de date
        std::string sender = std::string(message_from_db, 8, message_from_db.find(" ") - 12);
        message_from_db = "Reply for: (\" " + message_from_db + " \")\n";

        // Cautam userul in baza de date
        User* receiver = database.findUser(sender);

        // Verificam daca userul exista
        if (receiver == NULL)
        {
            client->sendMessage("* Userul nu exista!");
            return;
        }

        // Copiem restul cuvintelor in mesaj
        std::string message = "";
        for (int i = 2; i < words.size(); i++)
            message += words.at(i) + " ";

        sender = user->getUserColor().c_str() + user->getUsername() + COLOR_DEFAULT;
        std::string reciver = receiver->getUserColor().c_str() + receiver->getUsername() + COLOR_DEFAULT;
        Message msg(message, message_from_db, sender, reciver);

        int number = database.getNumberOfMessages(receiver->getUsername());
        msg.setID(number);

        database.addMessageFile(&msg, receiver->getUsername());


        // Daca userul este logat ii trimitem mesajul
        User* temp_user = database.getUser(receiver->getUsername());
        if (temp_user != NULL)
            temp_user->getSocket()->sendMessage(msg.toString());

    }
    else if (words.at(0) == ":messages") {
        // Veririficam daca este logat
        User* user = database.getUserBySocket(client);
        if (user == NULL) {
            client->sendMessage("* Nu esti logat!");
            return;
        }

        // Verificam daca are mesaje
        if (database.getNumberOfMessages(user->getUsername()) == 0)
        {
            client->sendMessage("* Nu aveti mesaje!");
            return;
        }

        // Afisam mesajele
        database.loadMessagesAndSend(user->getUsername(), client);
    }
    else {
        client->sendMessage("* Comanda invalida, folositi :help pentru a vedea lista de comenzi");
    }
}