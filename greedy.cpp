// Leer instancias
// Lista de adyacencia, no matriz de distancias 

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <algorithm>
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
    bool operator==(const Client& other) const {
        return x == other.x && y == other.y;
    }
};

//Funcion para debugear
void print_clients(vector<Client> clients){
    for(int i = 0; i < clients.size(); i++){
        cout << clients[i].x << " " << clients[i].y << endl;
    }
}
//Distancia euclidiana
float dist(Client a, Client b){
    //sqrt retorna double asi que casteamos a float
    return (float) sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2)); 
}

//Como instancias son pequeñas, podemos usar búsqueda lineal simplemente
//Para instancias grandes probablemente sea mejor un min heap
//Obtenemos el cliente factible más cercano
Client nearestNeighbour(vector<Client> clients, Client client, float truck_capacity, float trailer_capacity){
    int min = dist(client,clients[0]);
    Client nearest = clients[0];
    float new_dist;
    for(int i = 1; i < clients.size(); i++){
        new_dist = dist(client,clients[i]);
        if (new_dist < min){
            if (client.truck_customer == 0){ //Si es nodo normal
                if (client.demand <= truck_capacity + trailer_capacity){
                    min = new_dist;
                    nearest = clients[i];
                }
            }
            else{ //Si es nodo de camión
                if (client.demand <= truck_capacity){
                        min = new_dist;
                        nearest = clients[i];
                    }
            }
            min = new_dist;
            nearest = clients[i];
        }
    }
    //Revisamos factibilidad final por si no encontró en todo el for y se quedó con el basal
    if (nearest.truck_customer == 0){ //Si es nodo normal
        if (nearest.demand <= truck_capacity + trailer_capacity){
            return nearest;
        }
    }
    else{ //Si es nodo de camión
        if (nearest.demand <= truck_capacity){
            return nearest;
        }
    }
    //Si no se encontró factible, retornamos un cliente especial None
    Client null_client;
    null_client.demand = -1;
    return null_client;
}

//Clase que representa la ruta de un vehículo en particular
//Solo es una lista de nodos. Al pasar de un nodo completo a un nodo chico se deja el trailer.
//Eventualmente, aparece nuevamente en la lista el nodo completo, reconectando el trailer.
class Route{
    public:
        vector<Client> clients;
    Route(vector<Client> clients, int n){
        this->clients = clients;
    }
    float getCost(){
        float cost = 0;
        for(int i = 0; i < clients.size() - 1; i++){
            cost += dist(clients[i],clients[i+1]);
        }
        return cost;
    }
    void print(){
        cout << "Solution: " << endl;
        for(int i = 0; i < clients.size(); i++){
            cout << "(" << clients[i].x << "," << clients[i].y << ")" << " ";
        }
        cout << endl;
        cout << "Evaluacion: " << this->getCost() << endl;
    }
};



class Solution{
    public:
        bool feasible;
        vector<vector<Client>> routes;
    Solution(){
        feasible = 0;
    }
    Solution(vector<vector<Client>> routes){
        //Inicialmente n rutas vacías
        feasible = 1;
        this->routes = routes;
    }
    // Evaluacion de una solucion
    float eval(){
        float cost = 0;
        int i;
        int j;
        for(i = 0; i < routes.size(); i++){
            for (j = 0; j < routes[i].size() - 1; j++){
                cost += dist(routes[i][j],routes[i][j+1]);
            }
            cost += dist(routes[i][j+1],routes[i][0]); // Agrega el costo de volver del ultimo nodo de cada camion al depot
        }
        return cost;
    }
    void print(){
        cout << "-------------------" << endl;
        cout << "Solution: " << endl;
        for(int i = 0; i < routes.size(); i++){
            cout << "Truck " << i << ": " << endl; 
            for (int j = 0; j < routes[i].size() - 1; j++){
                cout << "(" << routes[i][j].x << "," << routes[i][j].y << ")" << " ";
            }
            cout << endl;
        }
        cout << endl;
        cout << "Evaluacion: " << this->eval() << endl;
        if (feasible == 0){
            cout << "Unfeasible Solution" << endl;
        }
        else{
            cout << "Feasible Solution" << endl;
        }
    }
};

//Objeto que almacena data de instancia
//Pendiente:
//Metodo para printear la instancia bien bonito
//Destructor
class Instance{
    public:
        int N_trucks;
        float max_truck_capacity;
        vector<float> truck_capacities;
        int N_trailers;
        float max_trailer_capacity;
        vector<float> trailer_capacities;
        int N_clients;
        vector<Client> clients;

        Instance(int N_trucks, float truck_capacity, int N_trailers, 
                float trailer_capacity, int N_clients, vector<Client> clients)
            {
            this->N_trucks = N_trucks;
            this->max_truck_capacity = truck_capacity;
            vector<float> truck_capacities;
            for(int i = 0; i < N_trucks; i++){
                truck_capacities.push_back(max_truck_capacity);
            }
            this->truck_capacities = truck_capacities;
            this->N_trailers = N_trailers;
            this->max_trailer_capacity = trailer_capacity;
            vector<float> trailer_capacities;
            for(int i = 0; i < N_trailers; i++){
                trailer_capacities.push_back(max_trailer_capacity);
            }
            this->trailer_capacities = trailer_capacities;
            this->N_clients = N_clients;
            this->clients = clients;
            }
        //Metodo para resolver la instancia al azar
        Solution solve(){

            

            srand(time(NULL));

            // Primero haremos un greedy, luego lo randomizaremos
            // Iteramos camión por camión (esto despues será random, o sera todo primero uno y luego otro idk)
            vector<vector<Client>> routes(N_trucks);
            vector<Client> aux_clients = clients;
            Client depot = aux_clients[0];
            aux_clients.erase(aux_clients.begin()); //Sacamos depot de lista de clientes

            
            for(int i = 0; i < N_trucks; i++){
                routes[i].push_back(depot); //Agregamos depot al comienzo de cada ruta
                Client aux_client = depot;
                while(!aux_clients.empty()){ //Iteramos hasta que se acaben los clientes
                    //We find closest neighbour
                    aux_client = nearestNeighbour(aux_clients,aux_client,truck_capacities[i],trailer_capacities[i]);
                    if (aux_client.demand == -1){ //No se encontró ningun cliente factible, pasamos al siguiente camión
                        break;
                    }
                    //Eliminamos cliente encontrado de la lista
                    vector<Client>::iterator position = find(aux_clients.begin(), aux_clients.end(), aux_client);
                    if (position != aux_clients.end()) //Si no encuentra el cliente en la lista, hay algun error
                        aux_clients.erase(position);
                    else {
                        cout << "Se intento de eliminar un cliente que no está en aux_clients" << endl;
                    }
                    //Add node to truck route, descontando la demanda correspondiente
                    routes[i].push_back(aux_client);
                    float total_capacity = truck_capacities[i] + trailer_capacities[i];
                    if (aux_client.truck_customer == 0){ //Si es cliente normal, se atiende desde el truck + trailer y descuenta demanda correspondiente
                        total_capacity -= aux_client.demand; //Este caso contempla relleno de truck despues de cada parada
                        if (total_capacity - max_truck_capacity < 0){
                            truck_capacities[i] = total_capacity;
                            trailer_capacities[i] = 0;
                        }
                        else { 
                            truck_capacities[i] = max_truck_capacity;
                            trailer_capacities[i] = total_capacity - max_truck_capacity;
                        }
                    }
                    else { //Si es cliente de trailer, se descuenta solo de trailer
                        trailer_capacities[i] -= aux_client.demand;
                    }

                }   
            }
            //Revisamos si se encontró una solución factible
            if (aux_clients.size() != 0){
                Solution solution;
                return solution;
            }
            Solution solution(routes);
            return solution;
        }
};

Instance read_instance(string instance_name){

    //Lectura de datos de instancia
    int N_trucks;
    float truck_capacity;
    int N_trailers;
    float trailer_capacity;
    int N_clients;
    vector<Client> clients;

    ifstream file("instances/"+instance_name);

    file >> N_trucks;
    file >> truck_capacity;
    file >> N_trailers;
    file >> trailer_capacity;
    file >> N_clients;
    
    //Nota: Por alguna razon, N_trailers puede ser mayor que N_trucks, en cuyo caso se igualan. (preguntar)
    N_trailers = min(N_trailers,N_trucks);

    // Lectura de datos de depot y clientes
    // por ahora tratamos al depot como un cliente cualquiera, quedando al comienzo de la lista
    int nub;
    float x;
    float y;
    float demand;
    bool truck_customer;

    //Construimos lista de clientes a partir del archivo
    Client aux_client;
    for (int i = 0; i < N_clients + 1; i++){
        file >> nub;
        file >> x;
        file >> y;
        file >> demand;
        file >> truck_customer;
        aux_client.set(x,y,demand,truck_customer);
        clients.push_back(aux_client);
    }

    Instance instance(N_trucks,truck_capacity,N_trailers,trailer_capacity,N_clients,clients);
    file.close();
    return instance;
}


int main(){
    
    string instance_name = "small1-5A.txt";
    Instance instance = read_instance(instance_name);
    print_clients(instance.clients);
    Solution solution = instance.solve();
    solution.print();

}

