// ILS.cpp contiene todo el código relacionado al ILS sobre una instancia ya resuelta
#include "GRASP.cpp"

// fullSwap, adjacentSwap or subtourFullSwap
#define MOVEMENT_TYPE "fullSwap"

// Funcion para construir el vecindario de soluciones utilizando swap adyacente, sin romper subtours
vector<Solution> getAdjacentSwapNeighbours(Solution solution){
    vector<Solution> neighbours;
    //Para cada ruta, aplicamos swap de todos los nodos adyacentes
    for (int route_index = 0; route_index < solution.routes.size(); route_index++){
        //Primero construimos hashmap de subrutas para no construir soluciones infactibles
        //El hashmap es de la forma {idCliente : listaDeClientes}
        //Cuando la ruta final alcanza un nodo presente en el hashmap, se appendea toda la listaDeClientes a la ruta y asi no arruinamos subrutas
        vector<Client> route = solution.routes[route_index];
        unordered_map<int,vector<Client>> subtours = getSubtours(route); 

        //Construimos nueva ruta sin subtours
        vector<Client> filtered_route;
        auto it = route.begin();
        while(it != route.end()){
            filtered_route.push_back(*it);
            if (subtours.find((*it).id) != subtours.end()){ //Revisamos si nodo tiene subtour
                int subtour_length = subtours[(*it).id].size();
                it += subtour_length; //Nos saltamos el subtour por completo
            }
            it++;
        }
        //Aplicamos swap adyacente sobre la lista filtrada, reintegrando los subtours tras haberlo hecho
        for (int i = 1; i < filtered_route.size() - 1; i++){
            Solution neighbourSolution = solution;
            vector<Client> aux_route = filtered_route;
            Client aux_client = aux_route[i]; 
            aux_route[i] = aux_route[i+1]; //Realizamos swap
            aux_route[i+1] = aux_client;
            aux_route = addSubtours(aux_route,subtours); //Agregamos subtours a la ruta modificada
            neighbourSolution.routes[route_index] = aux_route; //Almacenamos ruta modificada en nuevo vecino
            neighbours.push_back(neighbourSolution);
        }
    }
    return neighbours;
}

// Funcion para construir el vecindario de soluciones utilizando swap full (todos con todos), sin romper subtours
vector<Solution> getFullSwapNeighbours(Solution solution){
    vector<Solution> neighbours;
    //Para cada ruta, aplicamos swap de todos los nodos 
    for (int route_index = 0; route_index < solution.routes.size(); route_index++){
        //Primero construimos hashmap de subrutas para no construir soluciones infactibles
        vector<Client> route = solution.routes[route_index];
        unordered_map<int,vector<Client>> subtours = getSubtours(route); 
        //Construimos nueva ruta sin subtours
        vector<Client> filtered_route;
        auto it = route.begin();
        while(it != route.end()){
            filtered_route.push_back(*it);
            if (subtours.find((*it).id) != subtours.end()){ //Revisamos si nodo tiene subtour
                int subtour_length = subtours[(*it).id].size();
                it += subtour_length; //Nos saltamos el subtour por completo
            }
            it++;
        }
        //Aplicamos swap sobre la lista filtrada, reintegrando los subtours tras haberlo hecho
        for (int i = 1; i < filtered_route.size() - 1; i++){
            for (int j = i + 1; j < filtered_route.size() - 1; j++){
                Solution neighbourSolution = solution;
                vector<Client> aux_route = filtered_route;
                Client aux_client = aux_route[i];
                aux_route[i] = aux_route[j]; //Realizamos swap
                aux_route[j] = aux_client;
                aux_route = addSubtours(aux_route,subtours); //Agregamos subtours a la ruta modificada
                neighbourSolution.routes[route_index] = aux_route; //Almacenamos ruta modificada en nuevo vecino
                neighbours.push_back(neighbourSolution);
            }
        }
    }
    return neighbours;
}


//Funcion getNeighbours actua de router a distintos métodos de movimiento
vector<Solution> getNeighbours(Solution solution, string movement_type){  
    if (movement_type == "adjacentSwap") return getAdjacentSwapNeighbours(solution);
    if (movement_type == "fullSwap") return getFullSwapNeighbours(solution);
    else{
        return getFullSwapNeighbours(solution);
    }
}

//Método de hill-climbing para mejorar iterativamente la solución actual
Solution ILS(Solution solution){
    float current_eval = solution.eval();
    while(true){
        vector<Solution> neighbours = getNeighbours(solution, MOVEMENT_TYPE);
        bool improved = false;
        for (Solution neighbour : neighbours){
            float new_eval = neighbour.eval();
            if (new_eval < current_eval){
                solution = neighbour;
                current_eval = new_eval;
                improved = true;
                break;
            }
        }
        if (!improved) return solution;
    }
    return solution;
}