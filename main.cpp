// Leer instancias
// Lista de adyacencia, no matriz de distancias 

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;


//Objeto que almacena data de cliente
//Pendiente:
//Destructor
//Implementar lista de adyacencia (quiza no sea necesario siendo que el grafo es todos a todos)
//Método para obtener cliente más cercano, o quizas lista de clientes en orden de cercanía
class Client{
    public:
        int x;
        int y;
        int demand;
        bool truck_customer;
    void set(int x, int y, int demand, bool truck_customer){
        this->x = x; 
        this->y = y;
        this->demand = demand;
        this->truck_customer = truck_customer;
    }
};

// Ton: Quizás no será necesario un struct de arco, dado que el costo es la distancia, basta 
// con una funcion que calcule la distancia entre dos nodos
struct Arc{
    int node_a;
    int node_b;
    float cost;
};

float dist(Client a, Client b){
    //sqrt retorna double asi que casteamos a float
    return (float) sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2)); 
}

//Objeto que almacena data de instancia
//Pendiente:
//Metodo solve() para resolver la instancia con algun algoritmo en particular
//Metodo para printear la instancia bien bonito
//Destructor
class Instance{
    int N_trucks;
    float truck_capacity;
    int N_trailers;
    float trailer_capacity;
    int N_clients;
    vector<Client> clients;

    Instance(int N_trucks, float truck_capacity, int N_trailers, 
             float trailer_capacity, int N_clients, vector<Client> clients)
        {
        this->N_trucks = N_trucks;
        this->truck_capacity = truck_capacity;
        this->N_trailers = N_trailers;
        this->trailer_capacity = trailer_capacity;
        this->N_clients = N_clients;
        this->clients = clients;
        }
    

};


int main(){
    int N_trucks;
    float truck_capacity;
    int N_trailers;
    float trailer_capacity;
    int N_clients;
    vector<Client> clients;

    string instance_name = "small1-5A.txt";
    ifstream file("instances/"+instance_name);

    file >> N_trucks;
    file >> truck_capacity;
    file >> N_trailers;
    file >> trailer_capacity;
    file >> N_clients;

    // Lectura de datos de depot y clientes
    // por ahora tratamos al depot como un cliente cualquiera, quedando al comienzo de la lista
    int nub;
    float x;
    float y;
    float demand;
    bool truck_customer;

    //Construimos lista de clientes a partir del archivo
    Client aux_client;
    for (int i = 0; i < N_clients; i++){
        file >> nub;
        file >> x;
        file >> y;
        file >> demand;
        file >> truck_customer;
        aux_client.set(x,y,demand,truck_customer);
        clients.push_back(aux_client);
    }
    
    file.close();

}

