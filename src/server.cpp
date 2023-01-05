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
        if (words.size() == 1)
        {
            client->sendMessage("* Comanda invalida, folositi :login <username> <password>");
            return;
        }

        if (database.getUser(words.at(1)) != NULL)
        {
            client->sendMessage("* Esti deja logat!");
            return;
        }

        // Verificam daca am primit 3 argumente
        if (words.size() != 3)
        {
            client->sendMessage("* Comanda invalida, folositi :login <username> <password>");
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
        // Verificam daca acest user este deja logat
        if (database.getUser(words.at(1)) != NULL)
        {
            client->sendMessage("* Esti deja logat!");
            return;
        }

        // Verificam daca a trimis cel putin 3 argumente
        if (words.size() < 3)
        {
            client->sendMessage("* Comanda invalida, folositi :register <username> <password>");
            return;
        }

        // Verificam daca userul exista deja
        if (database.findUser(words.at(1)) != NULL)
        {
            client->sendMessage("* Userul exista deja!");
            return;
        }

        // Cream un nou user
        User* user = new User(words.at(1), words.at(2), client);
        user->setLoggedIn(true);

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
        User* user = database.getUserBySocket(client);
        if (user == NULL) {
            client->sendMessage("* Esti delogat!");
            return;
        }

        // Verificam daca a trimis cel putin 2 argumente
        if (words.size() == 1)
        {
            client->sendMessage("Comanda invalida, folositi :send <username> <message>");
            return;
        }

        // Verificam daca a trimis cel putin 3 argumente
        if (words.size() < 3)
        {
            client->sendMessage("Comanda invalida, folositi :send <username> <message>");
            return;
        }

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

        Message msg(message, user->getUsername(), receiver->getUsername());


        // Verificam daca userul este logat, daca nu este logat, atunci o sa ii creeam un fisier cu mesaje ofline
        User* tempUser = database.getUser(receiver->getUsername());
        if (tempUser == NULL)
        {
            database.updateMessageFile(&msg, receiver->getUsername());

            std::cout << "Un mesaj offline a fost inregistrat pentru: " << receiver->getUsername() << std::endl;
            return;
        }


        // Trimitem mesajul
        // ! Deoarece userul este logat, atunci ii trimitem mesajul direct
        receiver = tempUser;
        receiver->getSocket()->sendMessage(msg.toString());

    }
    else if (words.at(0) == ":help") {
        // Trimitem mesajul de succes
        client->sendMessage("Comenzi: \n:login <username> <password> \n:register <username> <password> \n:logout \n:users \n:send <username> <message>\n:help");
    }
    else {
        client->sendMessage("* Comanda invalida, folositi :help pentru a vedea lista de comenzi");
    }
}