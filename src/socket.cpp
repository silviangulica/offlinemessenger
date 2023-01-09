#include "socket.hpp"

// Socket care primeste un parametru
// Folosit pentru a crea un socket de tip server
Socket::Socket(int PORT)
{
    // Initializam structura si lungimea
    bzero(&socket_details, sizeof(socket_details));
    socket_details_len = sizeof(socket_details);

    // Creaza un socket de tip TCP/IP
    sockFD = socket(AF_INET, SOCK_STREAM, 0);

    // Verifica daca socketul a fost creat cu succes
    if (sockFD == -1)
    {
        std::cout << "[Eroare]: Nu s-a putut crea socketul" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "[Info]: Socketul a fost creat cu succes" << std::endl;
    }

    // Seteaza structura sockaddr_in
    socket_details.sin_family = AF_INET;
    socket_details.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_details.sin_port = htons(PORT);

    // Bind la server
    int bind_rez = bind(sockFD, (sockaddr*)&socket_details, socket_details_len);

    // Verificam eventualele erori ce pot aparea la bind
    if (bind_rez == -1)
    {
        std::cout << "[Eroare]: Nu s-a putut face \"bind\" la socket!" << std::endl;
        close(sockFD);
        exit(1);
    }
    else
    {
        std::cout << "[Info]: \"Bind\" realizat cu succes! " << std::endl;
    }

    // Plasam serverul in listen mode
    int listen_rez = listen(sockFD, 5);

    // Verificam daca putem face listen
    if (listen_rez == -1)
    {
        std::cout << "[Eroare]: Nu am putut plasa serverul in listen mode!" << std::endl;
        close(sockFD);
        exit(1);
    }
    else
    {
        std::cout << "[Info]: Serverul este online si asculta pe PORT-ul: " << PORT << std::endl;
    }
}

// Socket care primeste 2 parametri
// Folosit pentru a crea un socket de tip client sau connectare
Socket::Socket(int PORT, std::string IP)
{
    // Initializam structura si lungimea
    bzero(&socket_details, sizeof(socket_details));
    socket_details_len = sizeof(socket_details);

    // Creaza un socket de tip TCP/IP
    sockFD = socket(AF_INET, SOCK_STREAM, 0);

    // Verifica daca a fost creat socketul cu succes
    if (sockFD == -1)
    {
        std::cout << "[Eroare]: Nu s-a putut crea socketul" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "[Info]: Socketul a fost creat cu succes" << std::endl;
    }

    // Adaugam detaliile de conectare pentru socketul de client
    socket_details.sin_family = AF_INET;
    socket_details.sin_port = htons(PORT);

    // Adaugam IP.c_str() in socket_details.sin_addr.s_addr
    if (inet_pton(AF_INET, IP.c_str(), &socket_details.sin_addr.s_addr) == -1)
    {
        std::cout << "[Error]: Nu pot converti acest IP, IP-ul nu este valid!" << std::endl;
        close(sockFD);
        exit(1);
    }

    // Inceram o connectare la server
    if (connect(sockFD, (sockaddr*)&socket_details, socket_details_len) == -1)
    {
        std::cout << "[Error]: Nu ma pot connecta la server! Inchid aplicatia!" << std::endl;
        close(sockFD);
        exit(1);
    }
    else
    {
        std::cout << "[Info]: Sunteti connectat cu serverul, folositi prompt-ul de mai jos pentru a interactiona cu serverul, \":help\" pentru ajutor!" << std::endl << std::endl;
    }
}

// Socket simplu care nu primeste niciun parametru
// Folosit pentru a crea un socket de tip client/acceptare
Socket::Socket(socklen_t serverFD)
{
    // Initializam structurile de date
    bzero(&socket_details, sizeof(socket_details));
    socklen_t socket_details_len = sizeof(socket_details);

    // Rulam functia de accept
    this->sockFD = accept(serverFD, (sockaddr*)&socket_details, &socket_details_len);

    // Verificam daca exista o eroare
    if (sockFD < 0)
    {
        std::cout << "[Error]: Nu am putut accepta o noua conexiune!" << std::endl;
        close(serverFD);
        exit(1);
    }
    else
    {
        std::cout << "[Info]: O noua conexiune a fost stabilita pentru socket-ul [" << this->sockFD << "]. " << std::endl;
    }
}

// Functie pentru a returna FD
socklen_t Socket::getFD()
{
    return sockFD; //
}

// Functie pentru a trimite mesaj pe sockedFD intern
void Socket::sendMessage(std::string message)
{
    // Trimitem folosind comanda write pe sockFD
    write(this->sockFD, message.c_str(), message.size());
}

// Functie oentru a trimite mesaj pe sockedFD extern
void Socket::sendMessage(socklen_t externFD, std::string message)
{
    // Trimitem folosind comanda write pe un socked extern
    write(externFD, message.c_str(), message.size());
}

// Functie pentru a citi un mesaj de pe socket intern
std::string Socket::receiveMessage()
{
    // Declaram o structura pentru mesaj
    char buffer[MSG_SIZE];
    int nr_bytes;

    // Citim cu read
    nr_bytes = read(sockFD, buffer, MSG_SIZE);

    // Verificam raspunsul
    if (nr_bytes == 0) {
        std::cout << "[Error]: Connection lost on [" << sockFD << "]!" << std::endl;
        close(sockFD);
        return "";
    }
    else if (nr_bytes < 0) {
        std::cout << "[Error]: There is a problem if the socket or with the code! I got on socket [" << sockFD << "]!" << std::endl;
        close(sockFD);
        exit(1);
    }

    // Formatam raspunsul
    buffer[nr_bytes] = 0;

    // Convertim raspunsul la string
    return std::string(buffer, 0, nr_bytes);

}
// Functie pentru a citi un mesaj de pe socket extern
std::string Socket::receiveMessage(socklen_t externFD)
{
    // Declaram o structura pentru mesaj
    char buffer[MSG_SIZE];
    int nr_bytes;

    // Citim cu read
    nr_bytes = read(externFD, buffer, MSG_SIZE);

    // Verificam raspunsul
    if (nr_bytes == 0) {
        std::cout << "[Error]: Connection lost!" << externFD << std::endl;
        close(externFD);
    }
    else if (nr_bytes < 0) {
        std::cout << "[Error]: There is a problem if the socket or with the code! " << std::endl;
        close(externFD);
        close(sockFD);
        exit(1);
    }

    // Formatam raspunsul
    buffer[nr_bytes] = 0;

    // Convertim raspunsul la string
    return std::string(buffer, 0, nr_bytes);
}

// Deconstructorul clasei
Socket::~Socket()
{
    close(sockFD);
}

// Functie care inchide conexiunea
void Socket::closeConnection()
{
    close(sockFD);
}
