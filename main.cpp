#include <iostream>
#include <string>

using namespace std;

//Definimos un struct que contendra los datos del Lote
typedef struct{
    int idLote;
    char nombreComponente[50];
    float pesoUnitario;
    int cantidadTotal;
} LoteProducción; //Definimos un nombre para el struct

//Creacion de la Matriz 2D para el almacenamiento de los punteros a una estructura LoteProduccion
LoteProduccion*** crearAlmacen(int filas, int columnas){
    LoteProduccion*** almacen = new LoteProducción**[filas];
    for(int i=0; i<filas; i++){
        almacen[i] = new LoteProduccion*[columnas];
        for(int j=0; j<columnas; i++){
            almacen[i][j] = nullptr;
        }
    }
}

int main(){
    cout << "1. Inicializar almacen" << endl;
    cout << "2. Colocar un lote en el almacén" << endl;
    cout << "3. Registrar inspección" << endl;
    cout << "4. Deshacer ultima inspección" << endl;
    cout << "5. Reporte por fila" << endl;
    cout << "6. Buscar por nombre de componente" << endl;
    cout << "7. Salir\n" << endl;

    cout << "Seleccione una opción: " << endl;
    int opc;
    cin >> opc(1,7);

    switch(opc){
        
        //Caso 1
        case 1:

        break;

                //Caso 2
        case 2:
        break;

                //Caso 3
        case 3:
        break;

                //Caso 4
        case 4:
        break;

                //Caso 5
        case 5:
        break;

                //Caso 6
        case 6:
        break;
        
                //Caso 7
        case 7:
        break;
    }
}
