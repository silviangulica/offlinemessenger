#include "src/socket.hpp"
#include "src/user.hpp"
#include "src/database.hpp"
#include <pthread.h>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

// -- Portul pentru conectarea la server
int PORT = 1111;           // -- Voi folosi portul "1111" pentru serverul meu
char IP[16] = "127.0.0.1"; // -- Voi folosi IP-ul "127.0.0.1" pentru serverul meu

// -- Declararea functiilor
static void *threadServeClient(void *arg);
void processCommand(std::string command, User *user);

// -- Initializam baza de date
Database database;

// -- Declaram un vector pentru a stoca userii
std::vector<User> users;

// -- Functia main
int main()
{

    // -- Crearea unui socket de ascultare pentru server
    Socket server_socket(PORT);

    // -- Printam un mesaj pentru a vedea ca serverul ruleaza
    std::cout << "Serverul ruleaza pe portul " << PORT << std::endl;

    while (true)
    {
        // -- Acceptam un nou client
        User user(server_socket.getFD());

        // -- Declarearea thread-ului pentru procesarea fiecarui client in parte
        pthread_t thread_id;

        // -- Crearea thread-ului pentru procesarea fiecarui client in parte
        if (pthread_create(&thread_id, NULL, threadServeClient, (void *)&user) < 0)
        {
            std::error_code ec(user.getID(), std::generic_category());
            std::cerr << "Eroare la crearea thread-ului: " << ec.message() << std::endl;
            exit(1);
        }

        // -- Detacham thread-ul pentru a il face independent
        pthread_detach(thread_id);
    }
    return 0;
}

// -- Functia thread pentru procesarea fiecarui client in parte, se executa la accpetarea unei noi conexiuni
static void *threadServeClient(void *arg)
{
    // -- Castam argumentul la tipul User
    User *user = (User *)arg;

    while (true)
    {
        // -- Citim mesajul de la client
        std::string message = user->getMessage();

        // -- Verificam daca s-a primit mesaj de la client
        if (message.size() == 0)
        {
            std::error_code ec(user->getID(), std::generic_category());
            std::cerr << "Eroare la primirea mesajului de la client: " << ec.message() << std::endl;
            break;
        }

        // -- Prelucrare commanda de la client
        processCommand(message, user);
    }

    // -- Inchdem threadul
    pthread_exit(NULL);
    return 0;
}

void processCommand(std::string command, User *user)
{
    // -- Spargerea stringului in cuvinte si stocarea lor intr-un vector
    std::vector<std::string> words;
    std::stringstream ss(command);
    std::string word;
    while (ss >> word)
    {
        words.push_back(word);
    }

    // -- Verificarile primelor cuvinte din vector pentru a indentifica comenzi
    if (words.at(0) == ":say")
    {
        user->sendMessage("Inca in lucru!");
    }

    // -- Comanda pentru a se loga la server
    else if (words.at(0) == ":login")
    {
        // -- Verificam daca user-ul este deja logat
        if (user->isLogged())
        {
            user->sendMessage("Eroare: Esti deja logat!");
            return;
        }

        // -- Verificam daca user-ul a introdus un nume de utilizator
        if (words.size() < 2)
        {
            user->sendMessage("Eroare: Nu ai introdus un nume de utilizator!");
            return;
        }

        // -- Verificam daca user-ul a introdus o parola
        if (words.size() < 3)
        {
            user->sendMessage("Eroare: Nu ai introdus o parola!");
            return;
        }

        // -- Cautam userul in baza de date
        User *userDB = database.getInformation(words.at(1), user->getID());

        // -- Verificam daca user-ul exista in baza de date
        if (userDB == NULL)
        {
            user->sendMessage("Eroare: User-ul nu exista in baza de date!");
            return;
        }

        // -- Verificam daca parola este corecta
        if (userDB->getPassword() != words.at(2))
        {
            user->sendMessage("Eroare: Parola este incorecta!");
            return;
        }

        // -- Setam user-ul ca fiind logat
        user->setLogged(true);

        // -- Anuntat clientul ca s-a logat cu succes
        user->sendMessage("Te-ai logat cu succes!");

        // -- Adaugam user-ul in vectorul de useri
        users.push_back(*user);
    }

    // -- Procesare comanda ":logout"
    else if (words.at(0) == ":logout")
    {
        // -- Verificam daca user-ul este logat
        if (!user->isLogged())
        {
            user->sendMessage("Eroare: Nu esti logat!");
            return;
        }

        // -- Setam user-ul ca fiind delogat
        user->setLogged(false);

        // -- Anuntam clientul ca s-a delogat cu succes
        user->sendMessage("Te-ai delogat cu succes!");

        // -- Stergem user-ul din vectorul de useri
        for (int i = 0; i < users.size(); i++)
        {
            if (users.at(i).getID() == user->getID())
            {
                users.erase(users.begin() + i);
                break;
            }
        }
    }
}