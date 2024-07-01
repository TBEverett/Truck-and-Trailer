#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <algorithm>
#include <queue>
#include <ctime>
#include <filesystem>
#include <chrono>
#include <unordered_map>
using namespace std;
namespace fs = std::filesystem;

/*
utils.cpp contiene todas las clases y funciones necesarias para trabajar el problema
independiente del método de resolución. Ej: Client, euclidian_dist, etc.
*/

auto seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 generator(seed);



//Objeto que almacena data de cliente
//Pendiente:
//Destructor
//Implementar lista de adyacencia (quiza no sea necesario siendo que el grafo es todos a todos)
//Método para obtener cliente más cercano, o quizas lista de clientes en orden de cercanía
class Client{
    public:
        int id;
        int x;
        int y;
        int demand;
        bool truck_customer;
    void set(int id, int x, int y, int demand, bool truck_customer){
        this->id = id;
        this->x = x; 
        this->y = y;
        this->demand = demand;
        this->truck_customer = truck_customer;
    }
    bool operator==(const Client& other) const {
        return x == other.x && y == other.y;
    }
};

//Distancia euclidiana
float dist(Client a, Client b){
    //sqrt retorna double asi que casteamos a float
    return (float) sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2)); 
}

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
        if (feasible == 0){
            return 999999;
        }
        float cost = 0;
        int i;
        int j;
        for(i = 0; i < routes.size(); i++){
            for (j = 0; j < routes[i].size() - 1; j++){
                cost += dist(routes[i][j],routes[i][j+1]);
            }
            cost += dist(routes[i][j],routes[i][0]); // Agrega el costo de volver del ultimo nodo de cada camion al depot
        }
        return cost;
    }
    void print(){
        cout << "-------------------" << endl;
        cout << "Solution: " << endl;
        for(int i = 0; i < routes.size(); i++){
            cout << "Truck " << i << ": " << endl; 
            for (int j = 0; j < routes[i].size(); j++){
                if (routes[i][j].truck_customer == 1){
                    cout << "T";
                }
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
    void to_file(string file_name){
        cout << file_name << endl;
        ofstream out(file_name);
        for(int i = 0; i < routes.size(); i++){
            for (int j = 0; j < routes[i].size(); j++){
                out << routes[i][j].id << " ";
            }
            out << endl;
        }
        out << "Evaluacion: " << this->eval() << endl;
        out.close();
    }
};

//Funcion para debugear
void print_clients(vector<Client> clients){
    for(int i = 0; i < clients.size(); i++){
        cout << clients[i].x << " " << clients[i].y << endl;
    }
}


//Como instancias son pequeñas, podemos usar búsqueda lineal simplemente
//Para instancias grandes probablemente sea mejor un min heap
//Obtenemos el cliente factible más cercano
//GRASP: Hacemos un ranking de clientes por cercanía y escogemos al azar ponderado
Client nearestNeighbour(vector<Client> clients, Client client, float truck_capacity, float trailer_capacity){

    //Obtenemos las distancias del punto a todo el resto
    vector<float> distances;
    for(int i = 0; i < clients.size(); i++){
        distances.push_back(dist(client,clients[i]));
    }
    
    //Vamos encontrando y extrayendo el min_element del vector (O(n^2) horrible, en implementación final usar un algoritmo de ordenamiento con indices del otro arreglo)
    vector<Client> sorted_clients;
    vector<Client> aux_clients = clients;
    while(!distances.empty()){
        int min_index = distance(distances.begin(), min_element(distances.begin(), distances.end()));
        sorted_clients.push_back(aux_clients[min_index]);
        distances.erase(distances.begin() + min_index);
    }

    //Filtramos vecinos incluyendo solo factibles
    vector<Client> filtered_neighbours;
    for(int i = 0; i < sorted_clients.size(); i++){
        if (sorted_clients[i].truck_customer == 0){ //Si es nodo normal
            if (sorted_clients[i].demand <= truck_capacity + trailer_capacity){
                filtered_neighbours.push_back(sorted_clients[i]);
            }
        }
        else{ //Si es nodo de camión
            if (sorted_clients[i].demand <= truck_capacity){
                filtered_neighbours.push_back(sorted_clients[i]);
            }
        }
    }
    if (filtered_neighbours.empty()){
        //Si no se encontró factible, retornamos un cliente especial nulo con demanda -1
        Client null_client;
        null_client.x = -1;
        null_client.y = -1;
        null_client.truck_customer = -1;
        null_client.demand = -1;
        return null_client;
    }

    //Escogemos un proximo nodo al azar entre el top 3 (o 5 quizas?)
    int top_n = 3;
    std::uniform_int_distribution<int> distribution(0, min(top_n,(int) filtered_neighbours.size()) - 1);
    float random_number = distribution(generator);
    return filtered_neighbours[random_number];
}

//Objeto que almacena data de instancia
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
            vector<float> trailer_capacities;
            for(int i = 0; i < N_trucks; i++){
                truck_capacities.push_back(max_truck_capacity);
                trailer_capacities.push_back(0);
            }
            this->truck_capacities = truck_capacities;
            this->N_trailers = N_trailers;
            this->max_trailer_capacity = trailer_capacity;
            
            for(int i = 0; i < N_trailers; i++){
                trailer_capacities[i] = max_trailer_capacity;
            }
            this->trailer_capacities = trailer_capacities;
            this->N_clients = N_clients;
            this->clients = clients;
            }
        //El metodo solve está implementado en GRASP.cpp
        Solution solve();
};

Instance read_instance(string instance_name){

    //Lectura de datos de instancia
    int N_trucks;
    float truck_capacity;
    int N_trailers;
    float trailer_capacity;
    int N_clients;
    vector<Client> clients;

    ifstream file(instance_name);

    file >> N_trucks;
    file >> truck_capacity;
    file >> N_trailers;
    file >> trailer_capacity;
    file >> N_clients;
    
    //Nota: Por alguna razon, N_trailers puede ser mayor que N_trucks, en cuyo caso se igualan.
    N_trailers = min(N_trailers,N_trucks);

    // Lectura de datos de depot y clientes
    // por ahora tratamos al depot como un cliente cualquiera, quedando al comienzo de la lista
    int id;
    float x;
    float y;
    float demand;
    bool truck_customer;

    //Construimos lista de clientes a partir del archivo
    Client aux_client;
    for (int i = 0; i < N_clients + 1; i++){
        file >> id;
        file >> x;
        file >> y;
        file >> demand;
        file >> truck_customer;
        aux_client.set(id,x,y,demand,truck_customer);
        clients.push_back(aux_client);
    }

    Instance instance(N_trucks,truck_capacity,N_trailers,trailer_capacity,N_clients,clients);
    file.close();
    return instance;
}

unordered_map<int,vector<Client>> getSubtours(vector<Client> route){
    unordered_map<int,vector<Client>> subtours;
    auto it = route.begin() + 1; //Nos saltamos depot
    while (it != route.end()){
        if ((*it).truck_customer == 1){ //Empieza subruta
            Client subtour_start_node = *(prev(it));
            // Caso de borde: Si el subtour_start_node es el depot nunca volverá
            // Si ocurre, consideramos su ruta completa como ruta sin subtours y retornamos
            if (subtour_start_node.id == route[0].id){
                //Buscamos si el depot aparece denuevo
                bool depot_repeated = false;
                for (auto iter = route.begin() + 1; iter != route.end(); iter++){
                    if ((*iter).id == route[0].id){
                        depot_repeated = true;
                        break;
                    }
                }
                if (!depot_repeated){
                    return subtours;
                }
            }
            vector<Client> subtour;
            auto sub_iter = it;
            while((*sub_iter).id != subtour_start_node.id){ //Almacenamos subruta 
                subtour.push_back(*sub_iter);
                if (sub_iter + 1 == route.end()){
                    break;
                } 
                sub_iter++;
                
            }
            // Agregamos nodo de devolucion a la subruta
            subtour.push_back(*sub_iter);
            //Agregamos subtour al hashmap de subtours
            subtours[subtour_start_node.id] = subtour;
            it = sub_iter;
        }
        it++;
    }
    return subtours;
}

vector<Client> addSubtours(vector<Client> route, unordered_map<int,vector<Client>> subtours){
    vector<Client> new_route;
    auto iter = route.begin();
    while(iter != route.end()){
        new_route.push_back(*iter);
        if (subtours.find((*iter).id) != subtours.end()){ //Agregamos subtour en caso de existir
            vector<Client> subtour = subtours[(*iter).id];
            for (Client client : subtour){
                new_route.push_back(client);
            }
        }
        iter++;
    }
    return new_route;
}
