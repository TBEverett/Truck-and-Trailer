// Leer instancias
// Lista de adyacencia, no matriz de distancias 

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
using namespace std;
namespace fs = std::filesystem;

auto seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 generator(seed);


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
        null_client.demand = -1;
        return null_client;
    }

    //Escogemos un proximo nodo al azar entre el top 3 (o 5 quizas?)
    int top_n = 5;
    std::uniform_int_distribution<int> distribution(0, min(top_n,(int) filtered_neighbours.size()) - 1);
    float random_number = distribution(generator);
    return filtered_neighbours[random_number];
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
    float eval(){
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
        cout << "Evaluacion: " << this->eval() << endl;
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
    void to_file(){
        ofstream out("solution.txt");
        for(int i = 0; i < routes.size(); i++){
            for (int j = 0; j < routes[i].size(); j++){
                if (routes[i][j].truck_customer == 1){
                    out << "T";
                }
                out << routes[i][j].x << "," << routes[i][j].y  << " ";
            }
            out << endl;
        }
        out << "Evaluacion: " << this->eval() << endl;
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
        //Metodo para resolver la instancia al azar
        Solution solve(){
            // Iteramos camión por camión (random despues maybe?)
            vector<vector<Client>> routes(N_trucks);
            vector<Client> aux_clients = clients;
            vector<float> aux_truck_capacities = truck_capacities;
            vector<float> aux_trailer_capacities = trailer_capacities;
            Client depot = aux_clients[0];
            Client subtour_start_client;
            aux_clients.erase(aux_clients.begin()); //Sacamos depot de lista de clientes
            for(int i = 0; i < N_trucks; i++){
                routes[i].push_back(depot); //Agregamos depot al comienzo de cada ruta
                Client aux_client = depot;
                Client prev_client;
                while(!aux_clients.empty()){ //Iteramos hasta que se acaben los clientes
                    //We find closest neighbour
                    prev_client = aux_client;
                    aux_client = nearestNeighbour(aux_clients,aux_client,aux_truck_capacities[i],aux_trailer_capacities[i]);
                    if (aux_client.demand == -1){ //No se encontró ningun cliente factible, pasamos al siguiente camión
                        //Aqui revisamos si es necesario volver a buscar el trailer a alguna parte
                        
                        if (prev_client.truck_customer == 1){
                            routes[i].push_back(subtour_start_client);
                        }
                        break;
                    }
                    //Eliminamos cliente encontrado de la lista
                    vector<Client>::iterator position = find(aux_clients.begin(), aux_clients.end(), aux_client);
                    if (position != aux_clients.end()) //Si no encuentra el cliente en la lista, hay algun error
                        aux_clients.erase(position);
                    else {
                        cout << "Se intento de eliminar un cliente que no está en aux_clients" << endl;
                    }
                    //Si pasamos a nodo de camion anotamos el comienzo del subtour para poder volver
                    if (prev_client.truck_customer == 0 && aux_client.truck_customer == 1){
                        subtour_start_client = prev_client;
                    }
                    //Si el nodo anterior era de camion y el nuevo es completo, hay que agregar el primer nodo del subtour denuevo para mostrar que el camion recoje el trailer
                    if (prev_client.truck_customer == 1 && aux_client.truck_customer == 0){
                        routes[i].push_back(subtour_start_client);
                    }
                    //Add node to truck route, descontando la demanda correspondiente (TaT sin transferencia)
                    routes[i].push_back(aux_client);
                    float total_capacity = aux_truck_capacities[i] + aux_trailer_capacities[i];
                    if (aux_client.truck_customer == 0){ //Si es cliente normal, se atiende desde el truck + trailer y descuenta demanda correspondiente
                        total_capacity -= aux_client.demand; //Este caso contempla relleno de truck despues de cada parada
                        if (total_capacity - max_truck_capacity < 0){
                            aux_truck_capacities[i] = total_capacity;
                            aux_trailer_capacities[i] = 0;
                        }
                        else { 
                            aux_truck_capacities[i] = max_truck_capacity;
                            aux_trailer_capacities[i] = total_capacity - max_truck_capacity;
                        }
                    }
                    else { //Si es cliente de trailer, se descuenta solo de trailer
                        aux_trailer_capacities[i] -= aux_client.demand;
                    }
                    //Condicion boba para caso borde donde el ultimo cliente de todo el recorrido de un camion es nodo camion y no termina volviendo a buscar el trailer
                    if (aux_clients.empty()){
                        if (aux_client.truck_customer == 1){
                                routes[i].push_back(subtour_start_client);
                        }
                    }
                }   
            }
            //Revisamos si se encontró una solución factible, si no, retornamos una solucion infactible
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

// Ejemplo: 
// g++ GRASP.cpp -o GRASP && ./GRASP instances/small2-10C.txt
// g++ GRASP.cpp -o GRASP && ./GRASP all
int main(int argc, char* argv[]){
    string instance_name = argv[1];
    if (instance_name != "all"){
        Instance instance = read_instance(instance_name);
        int restarts = stoi(argv[2]);
        Solution best_solution;
        float best_eval = 999999;
        for(int i = 0; i < restarts; i++){
            Solution solution = instance.solve();
            if (solution.eval() < best_eval){
                best_solution = solution;
                best_eval = solution.eval();
            }
        }
        best_solution.print();
        best_solution.to_file();
    }
    else{
        string path = "andres";
        ofstream output_file("resultados.csv");
        float total_eval = 0;
        int instance_amount = 0;
        //Ejecutamos 'restarts' veces por cada instancia en el directorio, almacenando la mejor solucion para cada una.
        for (const auto & entry : fs::directory_iterator(path)){
            output_file << entry.path() << ": ";
            cout << "Working on instance: " << instance_amount << " - " << entry.path() << endl;
            string instance_name = entry.path();
            Instance instance = read_instance(instance_name);
            
            int restarts = stoi(argv[2]);
            Solution best_solution;
            float best_eval = 999999;
            for(int i = 0; i < restarts; i++){
                Solution solution = instance.solve();
                if (solution.eval() < best_eval){
                    best_solution = solution;
                    best_eval = solution.eval();
                }
            }
            output_file << best_solution.eval() << endl;
            total_eval += best_solution.eval();
            instance_amount++;
        }
        total_eval /= instance_amount;
        output_file << "Evaluacion promedio: " << total_eval << endl;   
    }
}
