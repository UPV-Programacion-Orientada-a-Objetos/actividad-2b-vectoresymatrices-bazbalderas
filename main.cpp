#include <iostream>
#include <string>
#include <cstring>
#include <limits>

using namespace std;



/*======================================================================================
FALTA, PARA MAÑANA PORQUE TENGO SUEÑO
======================================================================================
/*Validación robusta de entradas (funciones como validarEntero())
Función para eliminar/remover lotes del almacén
Función para mostrar estadísticas del almacén (ocupación, totales)
Función para exportar/importar datos desde archivos
Función para mostrar todo el historial de inspecciones
Sistema de alertas (stock bajo, lotes rechazados)
Función para reorganizar/mover lotes entre posiciones
Búsqueda por ID de lote
Reporte completo del almacén (todas las posiciones)
Sistema de backup/restauración de datos*/

/*======================================================================================
SISTEMA DE GESTIÓN DE ALMACÉN - FÁBRICA ALPHATECH
======================================================================================

//Definimos un struct que contendra los datos del Lote
typedef struct{
    int idLote;
    char nombreComponente[50];
    float pesoUnitario;
    int cantidadTotal;
} LoteProduccion; //Definimos un nombre para el struct

/*======================================================================================
ESTRUCTURA MAESTRO - GESTIÓN DINÁMICA DE LOTES
======================================================================================*/
struct Maestro {
    LoteProduccion* data;  // Arreglo dinámico de lotes
    bool* used;            // Marcadores de slots usados
    int size;              // Cantidad de lotes activos
    int cap;               // Capacidad total del arreglo
};

// Inicializa el sistema maestro
void maestroInit(Maestro& m, int capIni = 8) {
    m.data = new LoteProduccion[capIni];
    m.used = new bool[capIni];
    for (int i = 0; i < capIni; ++i) m.used[i] = false;
    m.size = 0;
    m.cap = capIni;
}

// Libera la memoria del sistema maestro
void maestroFree(Maestro& m) {
    delete[] m.data;
    delete[] m.used;
    m.data = nullptr; 
    m.used = nullptr;
    m.size = 0; 
    m.cap = 0;
}

// Expande la capacidad del sistema maestro cuando se llena
void maestroGrow(Maestro& m) {
    int newCap = (m.cap == 0 ? 8 : m.cap * 2);
    LoteProduccion* nd = new LoteProduccion[newCap];
    bool* nu = new bool[newCap];
    
    for (int i = 0; i < m.cap; ++i) { 
        nd[i] = m.data[i]; 
        nu[i] = m.used[i]; 
    }
    for (int i = m.cap; i < newCap; ++i) nu[i] = false;
    
    delete[] m.data; 
    delete[] m.used;
    m.data = nd; 
    m.used = nu; 
    m.cap = newCap;
}

// Reserva un slot en el sistema maestro
int maestroReservar(Maestro& m) {
    for (int i = 0; i < m.cap; ++i) {
        if (!m.used[i]) { 
            m.used[i] = true; 
            m.size++; 
            return i; 
        }
    }
    int old = m.cap; 
    maestroGrow(m);
    m.used[old] = true; 
    m.size++; 
    return old;
}

// Crea un nuevo lote en el sistema maestro
LoteProduccion* maestroCrear(Maestro& m, int id, const char* nombre, float peso, int cant) {
    int idx = maestroReservar(m);
    m.data[idx].idLote = id;
    strncpy(m.data[idx].nombreComponente, nombre, sizeof(m.data[idx].nombreComponente)-1);
    m.data[idx].nombreComponente[sizeof(m.data[idx].nombreComponente)-1] = '\0';
    m.data[idx].pesoUnitario = peso;
    m.data[idx].cantidadTotal = cant;
    return &m.data[idx];
}

// Busca un lote por ID en el sistema maestro
int maestroBuscarID(const Maestro& m, int id) {
    for (int i = 0; i < m.cap; ++i) {
        if (m.used[i] && m.data[i].idLote == id) return i;
    }
    return -1;
}

/*======================================================================================
SISTEMA DE ALMACÉN - MATRIZ 2D COMO ARREGLO 1D
======================================================================================*/

// Crea "matriz" 1D de punteros (todas las celdas a nullptr)
LoteProduccion** crearAlmacen(int filas, int columnas) {
    int N = filas * columnas;
    LoteProduccion** A = new LoteProduccion*[N];
    for (int i = 0; i < N; ++i) A[i] = nullptr;
    return A;
}

// Libera la "matriz" (NO borra los lotes, solo el arreglo de punteros)
void liberarAlmacen(LoteProduccion** A) {
    delete[] A;
}

// Coloca puntero en (f,c) si está libre
bool colocar(LoteProduccion** A, int filas, int columnas, int f, int c, LoteProduccion* ptr) {
    if (f < 0 || f >= filas || c < 0 || c >= columnas) return false;
    int idx = f * columnas + c;
    if (A[idx] != nullptr) return false;
    A[idx] = ptr;
    return true;
}

// Reporte por fila
void reporteFila(LoteProduccion** A, int filas, int columnas, int f) {
    if (f < 0 || f >= filas) return;
    cout << "=== REPORTE DE FILA " << f << " ===" << endl;
    for (int c = 0; c < columnas; ++c) {
        int idx = f * columnas + c;
        if (A[idx] == nullptr) {
            cout << "Posición (" << f << ", " << c << "): VACÍA" << endl;
        } else {
            cout << "Posición (" << f << ", " << c << "): " << endl;
            cout << "  ID: " << A[idx]->idLote << endl;
            cout << "  Componente: " << A[idx]->nombreComponente << endl;
            cout << "  Peso unitario: " << A[idx]->pesoUnitario << " kg" << endl;
            cout << "  Cantidad: " << A[idx]->cantidadTotal << " unidades" << endl;
        }
    }
}

// Buscar por nombre en todo el almacén
bool buscarPorNombre(LoteProduccion** A, int filas, int columnas, const char* nombre) {
    bool encontrado = false;
    cout << "=== BÚSQUEDA POR COMPONENTE: " << nombre << " ===" << endl;
    
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < columnas; ++c) {
            int idx = f * columnas + c;
            if (A[idx] && strcmp(A[idx]->nombreComponente, nombre) == 0) {
                cout << "Encontrado en posición (" << f << ", " << c << ")" << endl;
                cout << "  ID Lote: " << A[idx]->idLote << endl;
                cout << "  Cantidad: " << A[idx]->cantidadTotal << " unidades" << endl;
                cout << "  Peso unitario: " << A[idx]->pesoUnitario << " kg" << endl;
                encontrado = true;
            }
        }
    }
    return encontrado;
}

/*======================================================================================
SISTEMA DE PILA PARA HISTORIAL DE INSPECCIONES
======================================================================================*/
struct Pila {
    int id[10];     // IDs de lotes inspeccionados
    int res[10];    // Resultados: 1=aprobado, 0=rechazado
    int top;        // Índice del tope (-1 si vacía)
};

// Inicializa la pila vacía
void pilaInit(Pila& p) { 
    p.top = -1; 
}

// Verifica si la pila está vacía
bool pilaVacia(const Pila& p) { 
    return p.top < 0; 
}

// Agrega una inspección a la pila
void pilaPush(Pila& p, int idLote, int resultado) {
    if (p.top < 9) { 
        p.top++;
        p.id[p.top] = idLote;
        p.res[p.top] = resultado;
    } else {         
        // Pila llena: desplaza elementos y agrega al final
        for (int i = 0; i < 9; ++i) { 
            p.id[i] = p.id[i+1]; 
            p.res[i] = p.res[i+1]; 
        }
        p.id[9] = idLote; 
        p.res[9] = resultado;
    }
}

// Remueve la última inspección de la pila
bool pilaPop(Pila& p, int& idLote, int& resultado) {
    if (pilaVacia(p)) return false;
    idLote = p.id[p.top];
    resultado = p.res[p.top];
    p.top--;
    return true;
}

/*======================================================================================
FUNCIÓN PRINCIPAL CON MENÚ INTERACTIVO
======================================================================================*/
int main() {
    cout << "=== SISTEMA DE GESTIÓN DE ALMACÉN ALPHATECH ===" << endl;
    cout << "Inicializando sistemas..." << endl;

    // Variables del almacén
    int filas = 0, columnas = 0;
    LoteProduccion** almacen = nullptr;

    // Inicialización de estructuras
    Maestro maestro; 
    maestroInit(maestro);
    Pila pila; 
    pilaInit(pila);

    int opc;
    do {
        cout << "\n=================== MENÚ PRINCIPAL ===================" << endl;
        cout << "1. Inicializar almacén" << endl;
        cout << "2. Colocar un lote en el almacén" << endl;
        cout << "3. Registrar inspección" << endl;
        cout << "4. Deshacer última inspección" << endl;
        cout << "5. Reporte por fila" << endl;
        cout << "6. Buscar por nombre de componente" << endl;
        cout << "7. Salir" << endl;
        cout << "=======================================================" << endl;
        cout << "Seleccione una opción (1-7): ";
        
        if (!(cin >> opc)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Ingrese un número válido." << endl;
            continue;
        }

        switch(opc) {
            case 1: {
                // Inicializar almacén
                if (almacen) {
                    liberarAlmacen(almacen);
                    almacen = nullptr;
                }
                
                cout << "Ingrese el número de filas: ";
                cin >> filas;
                cout << "Ingrese el número de columnas: ";
                cin >> columnas;
                
                if (filas > 0 && columnas > 0) {
                    almacen = crearAlmacen(filas, columnas);
                    cout << "✓ Almacén " << filas << "x" << columnas << " creado exitosamente." << endl;
                } else {
                    cout << "✗ Error: Las dimensiones deben ser positivas." << endl;
                }
                break;
            }
            
            case 2: {
                // Colocar lote en almacén
                if (!almacen) {
                    cout << "✗ Error: Primero debe inicializar el almacén." << endl;
                    break;
                }
                
                int f, c, id, cant;
                float peso;
                char nombre[50];
                
                cout << "Ingrese la fila (0-" << (filas-1) << "): ";
                cin >> f;
                cout << "Ingrese la columna (0-" << (columnas-1) << "): ";
                cin >> c;
                cout << "Ingrese el ID del lote: ";
                cin >> id;
                cout << "Ingrese el nombre del componente: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.getline(nombre, 50);
                cout << "Ingrese el peso unitario (kg): ";
                cin >> peso;
                cout << "Ingrese la cantidad total: ";
                cin >> cant;

                // Verificar si ya existe el ID
                if (maestroBuscarID(maestro, id) != -1) {
                    cout << "✗ Error: Ya existe un lote con ID " << id << endl;
                    break;
                }

                LoteProduccion* ptr = maestroCrear(maestro, id, nombre, peso, cant);
                if (colocar(almacen, filas, columnas, f, c, ptr)) {
                    cout << "✓ Lote colocado exitosamente en posición (" << f << ", " << c << ")" << endl;
                } else {
                    cout << "✗ Error: No se pudo colocar (posición ocupada o fuera de rango)" << endl;
                }
                break;
            }
            
            case 3: {
                // Registrar inspección
                int id, resultado;
                cout << "Ingrese el ID del lote a inspeccionar: ";
                cin >> id;
                
                if (maestroBuscarID(maestro, id) == -1) {
                    cout << "✗ Error: No existe un lote con ID " << id << endl;
                    break;
                }
                
                cout << "Ingrese el resultado (1=Aprobado, 0=Rechazado): ";
                cin >> resultado;
                
                if (resultado != 0 && resultado != 1) {
                    cout << "✗ Error: Resultado inválido." << endl;
                    break;
                }
                
                pilaPush(pila, id, resultado);
                cout << "✓ Inspección registrada: Lote " << id << " - " 
                     << (resultado ? "APROBADO" : "RECHAZADO") << endl;
                break;
            }
            
            case 4: {
                // Deshacer última inspección
                int id, resultado;
                if (pilaPop(pila, id, resultado)) {
                    cout << "✓ Inspección deshecha: Lote " << id << " - " 
                         << (resultado ? "APROBADO" : "RECHAZADO") << endl;
                } else {
                    cout << "✗ No hay inspecciones para deshacer." << endl;
                }
                break;
            }
            
            case 5: {
                // Reporte por fila
                if (!almacen) {
                    cout << "✗ Error: No hay almacén inicializado." << endl;
                    break;
                }
                
                int f;
                cout << "Ingrese el número de fila (0-" << (filas-1) << "): ";
                cin >> f;
                
                if (f >= 0 && f < filas) {
                    reporteFila(almacen, filas, columnas, f);
                } else {
                    cout << "✗ Error: Fila fuera de rango." << endl;
                }
                break;
            }
            
            case 6: {
                // Buscar por nombre de componente
                if (!almacen) {
                    cout << "✗ Error: No hay almacén inicializado." << endl;
                    break;
                }
                
                char nombre[50];
                cout << "Ingrese el nombre del componente: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.getline(nombre, 50);
                
                if (!buscarPorNombre(almacen, filas, columnas, nombre)) {
                    cout << "✗ No se encontraron componentes con ese nombre." << endl;
                }
                break;
            }
            
            case 7: {
                cout << "Cerrando sistema y liberando memoria..." << endl;
                break;
            }
            
            default: {
                cout << "✗ Opción inválida. Seleccione entre 1-7." << endl;
                break;
            }
        }
        
    } while(opc != 7);

    // Limpieza de memoria
    if (almacen) liberarAlmacen(almacen);
    maestroFree(maestro);
    
    cout << "✓ Sistema cerrado correctamente. ¡Hasta luego!" << endl;
    return 0;
}
