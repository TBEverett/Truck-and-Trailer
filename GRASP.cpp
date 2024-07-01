
//GRASP.cpp incluye los métodos relacionados a la resolucion de instancias utilizando GRASP
#include "utils.cpp"

Solution GRASP(Instance instance){
    //Extraemos data de la instancia
    int N_trucks = instance.N_trucks;
    int N_trailers = instance.N_trailers;
    int max_truck_capacity = instance.max_truck_capacity;

    // Round Robin de Camión
    vector<vector<Client>> routes(N_trucks);
    vector<Client> pending_clients = instance.clients;
    vector<float> aux_truck_capacities = instance.truck_capacities;
    vector<float> aux_trailer_capacities = instance.trailer_capacities;
    Client depot = pending_clients[0];
    vector<Client> subtour_start_client(N_trucks);
    pending_clients.erase(pending_clients.begin()); //Sacamos depot de lista de clientes
    for (int i = 0; i < N_trucks; i++){
        routes[i].push_back(depot); //Agregamos depot al comienzo de cada ruta
    }
    vector<Client> current_client; //Un current client por truck
    for (int i = 0; i < N_trucks; i++){ //Comenzamos con el depot como cliente actual de cada truck
        current_client.push_back(depot);
    }
    vector<Client> prev_client(N_trucks); //Un prev client por truck
    vector<bool> trucks_stuck(N_trucks,false); //Si damos N_trucks vueltas por los camiones y ninguno puede tomar un nodo, la solucion es infactible
    int i = 0;
    int i_truck = 0;
    int CLIENTS_PER_ASSIGNMENT = 3;
    while(!pending_clients.empty()){ //Iteramos hasta que se acaben los clientes
        //Revisamos si todos los camiones ya estan stuck, en cuyo caso la solucion es infactible
        bool all_stuck = true;
        for(bool stuck : trucks_stuck){
            if (!stuck){
                all_stuck = false;
                break;
            } 
        }
        if (all_stuck){
            Solution solution;
            return solution;
        }
        //Aquí seguimos si hay algun camión no stuck
        //Conseguimos vecino más cercano
        //Si camion esta stuck, continuamos con el siguiente
        if (trucks_stuck[i_truck]){
            i_truck = (i_truck + 1) % N_trucks; //Saltamos a prox truck
            i = CLIENTS_PER_ASSIGNMENT * i_truck; //Correjimos i para que proximo truck siga dando CLIENTS_PER_ASSIGNMENT vueltas
            continue;
        }
        prev_client[i_truck] = current_client[i_truck];
        current_client[i_truck] = nearestNeighbour(pending_clients,current_client[i_truck],aux_truck_capacities[i_truck],aux_trailer_capacities[i_truck]);
        if (current_client[i_truck].demand == -1){ //No se encontró ningun cliente factible, pasamos al siguiente camión
            //Aqui revisamos si es necesario volver a buscar el trailer a alguna parte
            if (prev_client[i_truck].truck_customer == 1){
                routes[i_truck].push_back(subtour_start_client[i_truck]);
            }
            trucks_stuck[i_truck] = true;
            i_truck = (i_truck + 1) % N_trucks; //Saltamos a prox truck
            i = CLIENTS_PER_ASSIGNMENT * i_truck; //Correjimos i para que proximo truck siga dando CLIENTS_PER_ASSIGNMENT vueltas
            continue;
        }
        
        //Eliminamos cliente encontrado de la lista
        auto position = find(pending_clients.begin(), pending_clients.end(), current_client[i_truck]);
        if (position != pending_clients.end()) //Si no encuentra el cliente en la lista, hay algun error
            pending_clients.erase(position);
        else {
            cout << "Se intento de eliminar un cliente que no está en pending_clients" << endl;
        }
        //Si pasamos a nodo de camion anotamos el comienzo del subtour para poder volver
        if (prev_client[i_truck].truck_customer == 0 && current_client[i_truck].truck_customer == 1){
            subtour_start_client[i_truck] = prev_client[i_truck];
        }
        //Si el nodo anterior era de camion y el nuevo es completo, hay que agregar el primer nodo del subtour denuevo para mostrar que el camion recoje el trailer
        //Tambien se debe agregar si el anterior era de camión y no hay proximo nodo.
        if (prev_client[i_truck].truck_customer == 1 && current_client[i_truck].truck_customer == 0){
            routes[i_truck].push_back(subtour_start_client[i_truck]);
        }
        //Add node to truck route, descontando la demanda correspondiente
        routes[i_truck].push_back(current_client[i_truck]);
        float total_capacity = aux_truck_capacities[i_truck] + aux_trailer_capacities[i_truck];
        if (current_client[i_truck].truck_customer == 0){ //Si es cliente normal, se atiende desde el truck + trailer y descuenta demanda correspondiente
            total_capacity -= current_client[i_truck].demand; //Este caso contempla relleno de truck despues de cada parada
            if (total_capacity - max_truck_capacity < 0){
                aux_truck_capacities[i_truck] = total_capacity;
                aux_trailer_capacities[i_truck] = 0;
            }
            else { 
                aux_truck_capacities[i_truck] = max_truck_capacity;
                aux_trailer_capacities[i_truck] = total_capacity - max_truck_capacity;
            }
        }
        else { //Si es cliente de trailer, se descuenta solo de trailer
            aux_trailer_capacities[i_truck] -= current_client[i_truck].demand;
        }
        //Condicion boba para caso borde donde el ultimo cliente de todo el recorrido de un camion es nodo camion y no termina volviendo a buscar el trailer
        if (pending_clients.empty()){
            if (current_client[i_truck].truck_customer == 1){
                routes[i_truck].push_back(subtour_start_client[i_truck]);
            }
        }
        i_truck = ((i++ / CLIENTS_PER_ASSIGNMENT) % N_trucks); // Vehiculo seleccionado dará CLIENTS_PER_ASSIGNMENT vueltas, tomando clientes.
        
        }   

        //Caso Borde: Si alguna ruta termina con un nodo camion, append el inicio del subtour
        for (int i_truck = 0; i_truck < N_trucks; i_truck++){
            if (routes[i_truck][routes[i_truck].size() - 1].truck_customer == 1){
                routes[i_truck].push_back(subtour_start_client[i_truck]);
            }
        }

        //Revisamos si se encontró una solución factible, si no, retornamos una solucion infactible
        if (pending_clients.size() != 0){
            Solution solution;
            return solution;
        }
        Solution solution(routes);
        return solution;
    }