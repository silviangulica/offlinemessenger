#include "src/socket.hpp"
#include "src/user.hpp"
#include <pthread.h>
#include <vector>
#include <map>

// -- Portul pentru conectarea la server
int PORT = 1111;           // -- Voi folosi portul "1111" pentru serverul meu
char IP[16] = "127.0.0.1"; // -- Voi folosi IP-ul "127.0.0.1" pentru serverul meu

// -- Declararea functiilor
static void *threadServeClient(void *arg);

// -- Declaram un map pentru a stoca userii
std::map<socklen_t, User> users;

// -- Functia main
int main()
{

    // -- Crearea unui socket de ascultare pentru server
    Socket server_socket(PORT);

    while (true)
    {
        // -- Acceptam un nou client
        User user(server_socket.getFD());

        // -- Adaugam user-ul in map
        users.insert(std::pair<socklen_t, User>(user.getID(), user));

        // -- Declarearea thread-ului pentru procesarea fiecarui client in parte
        pthread_t thread_id;

        // -- Crearea thread-ului pentru procesarea fiecarui client in parte
        if (pthread_create(&thread_id, NULL, threadServeClient, (void *)&user) < 0)
        {
            std::error_code ec(user.getID(), std::generic_category());
            std::cerr << "Eroare la crearea thread-ului: " << ec.message() << std::endl;
            exit(1);
        }

        // -- Stergem thread-ul cand nu mai este nevoie de el
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

        // -- Afisam mesajul primit de la client
        std::cout << "Clientul " << user->getID() << " a trimis mesajul: " << message << std::endl;
    }

    return 0;
}