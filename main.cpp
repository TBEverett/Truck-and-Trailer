#include "ILS.cpp"

// Ejemplo: 
// g++ -g main.cpp -o main && ./main instances/small2-10C.txt 10000
// g++ main.cpp -o main && ./main all 10000
int main(int argc, char* argv[]){
    if (argc != 3){
        cout << "Cantidad incorrecta de argumentos, deberian ser 2 (instancia restarts)" << endl;
        return 0;
    } 
    string instance_name = argv[1];
    if (instance_name != "all"){
        Instance instance = read_instance(instance_name);
        int restarts = stoi(argv[2]);
        Solution best_solution;
        float best_eval = 999999;
        for(int i = 0; i < restarts; i++){
            Solution solution = GRASP(instance); //Resolvemos con GRASP
            solution = ILS(solution); //Mejoramos con ILS
            if (solution.eval() < best_eval){
                best_solution = solution;
                best_eval = solution.eval();
            }
        }
        best_solution.print();
        best_solution.to_file("solutions/" + instance_name + "_GRASP_sol.txt");
    }
    else{
        string path = "andres";
        ofstream results_file("resultados.csv");
        float total_eval = 0;
        int instance_amount = 0;
        //Ejecutamos 'restarts' veces por cada instancia en el directorio, almacenando la mejor solucion para cada una.
        for (const auto & entry : fs::directory_iterator(path)){
            results_file << entry.path() << ": ";
            cout << "Working on instance: " << instance_amount << " - " << entry.path() << endl;
            string instance_name = entry.path();
            Instance instance = read_instance(instance_name);
            
            int restarts = stoi(argv[2]);
            Solution best_solution;
            float best_eval = 999999;
            for(int i = 0; i < restarts; i++){
                Solution solution = GRASP(instance);
                solution = ILS(solution); //Mejoramos con ILS
                if (solution.eval() < best_eval){
                    best_solution = solution;
                    best_eval = solution.eval();
                }
            }
            best_solution.to_file("solutions/" + instance_name + "_ILS2_sol.txt");
            results_file << best_solution.eval() << endl;
            total_eval += best_solution.eval();
            instance_amount++;
        }
        total_eval /= instance_amount;
        results_file << "Evaluacion promedio: " << total_eval << endl;   
    }
}
