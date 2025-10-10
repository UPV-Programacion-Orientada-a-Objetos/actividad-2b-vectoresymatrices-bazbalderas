#include <iostream>    // Para entrada/salida estándar (cout, cin)
#include <string>      // Para manejo de strings y operaciones de texto
#include <cstring>     // Para funciones de C strings (strcmp, strcpy, strncpy)
#include <limits>      // Para constantes de límites numéricos (numeric_limits)
#include <climits>     // Para constantes de enteros (INT_MIN, INT_MAX)
#include <cfloat>      // Para constantes de flotantes (FLT_MAX)
#include <fstream>     // Para manejo de archivos (ifstream, ofstream)
#include <iomanip>     // Para manipulación de formato de salida (fixed, setprecision)
#include <vector>      // Para el uso de vectores dinámicos en importación

using namespace std;

/*======================================================================================
SISTEMA DE GESTIÓN DE ALMACÉN FÁBRICA ALPHATECH - V2.0
======================================================================================

DESCRIPCIÓN GENERAL:
Este sistema implementa un almacén inteligente para gestión de componentes industriales
utilizando estructuras de datos avanzadas y técnicas de programación en C++.

CARACTERÍSTICAS PRINCIPALES:
• Almacén como matriz 2D implementada como arreglo 1D para eficiencia de memoria
• Sistema maestro dinámico para gestión automática de memoria de lotes
• Pila LIFO para historial de inspecciones con funcionalidad de deshacer
• Validación robusta de todas las entradas del usuario
• Sistema completo de backup y restauración de datos
• Alertas automáticas para gestión proactiva del inventario
• Exportación/importación de datos en formato CSV

ARQUITECTURA DEL SISTEMA:
1. VALIDACIÓN: Funciones robustas para validar entradas numéricas y de texto
2. ALMACÉN: Matriz bidimensional optimizada como arreglo unidimensional
3. MAESTRO: Sistema dinámico de gestión de memoria para lotes de producción
4. PILA: Estructura LIFO para control de inspecciones con historial
5. PERSISTENCIA: Sistema de archivos para backup y restauración
6. INTERFAZ: Menú interactivo con 19 opciones completas

COMPLEJIDAD ALGORÍTMICA:
• Búsqueda por ID: O(n) donde n = capacidad del maestro
• Búsqueda por nombre: O(f*c) donde f=filas, c=columnas
• Inserción de lote: O(1) amortizado con redimensionamiento automático
• Exportación/Importación: O(f*c) para recorrer todas las posiciones

======================================================================================*/



/*======================================================================================
FUNCIONES DE VALIDACIÓN DE ENTRADA
======================================================================================*/

// Valida entrada de enteros con rango opcional
int validarEntero(const char* mensaje, int minVal = INT_MIN, int maxVal = INT_MAX) {
    int valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            if (valor >= minVal && valor <= maxVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return valor;
            } else {
                cout << "✗ Error: El valor debe estar entre " << minVal << " y " << maxVal << endl;
            }
        } else {
            cout << "✗ Error: Ingrese un número entero válido." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Valida entrada de números flotantes con rango opcional
float validarFloat(const char* mensaje, float minVal = -FLT_MAX, float maxVal = FLT_MAX) {
    float valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            if (valor >= minVal && valor <= maxVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return valor;
            } else {
                cout << "✗ Error: El valor debe estar entre " << minVal << " y " << maxVal << endl;
            }
        } else {
            cout << "✗ Error: Ingrese un número decimal válido." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Valida entrada de cadenas no vacías
void validarString(const char* mensaje, char* buffer, int tamMax) {
    while (true) {
        cout << mensaje;
        cin.getline(buffer, tamMax);
        
        if (strlen(buffer) > 0) {
            return;
        } else {
            cout << "✗ Error: No puede estar vacío. Intente nuevamente." << endl;
        }
    }
}

// Confirma una acción con S/N
bool confirmarAccion(const char* mensaje) {
    char respuesta;
    cout << mensaje << " (S/N): ";
    cin >> respuesta;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return (respuesta == 'S' || respuesta == 's');
}

/*======================================================================================
SISTEMA DE GESTIÓN DE ALMACÉN - FÁBRICA ALPHATECH
======================================================================================*/

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
// ALGORITMO: Duplica la capacidad actual y copia todos los elementos existentes
// COMPLEJIDAD: O(n) donde n es la capacidad actual
void maestroGrow(Maestro& m) {
    // Paso 1: Calcular nueva capacidad (duplicar o inicializar en 8)
    int newCap = (m.cap == 0 ? 8 : m.cap * 2);
    
    // Paso 2: Crear nuevos arreglos con mayor capacidad
    LoteProduccion* nd = new LoteProduccion[newCap];  // Nuevo arreglo de datos
    bool* nu = new bool[newCap];                      // Nuevo arreglo de flags
    
    // Paso 3: Copiar todos los elementos existentes a los nuevos arreglos
    for (int i = 0; i < m.cap; ++i) { 
        nd[i] = m.data[i]; // Copia el lote completo (struct copy)
        nu[i] = m.used[i]; // Copia el flag de uso
    }
    
    // Paso 4: Inicializar las nuevas posiciones como no utilizadas
    for (int i = m.cap; i < newCap; ++i) nu[i] = false;
    
    // Paso 5: Liberar memoria antigua y actualizar punteros
    delete[] m.data; 
    delete[] m.used;
    m.data = nd;     // Apuntar a los nuevos arreglos
    m.used = nu; 
    m.cap = newCap;  // Actualizar capacidad
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

// Elimina un lote del sistema maestro
bool maestroEliminar(Maestro& m, int id) {
    int idx = maestroBuscarID(m, id);
    if (idx == -1) return false;
    
    m.used[idx] = false;
    m.size--;
    return true;
}

/*======================================================================================
SISTEMA DE ALMACÉN - MATRIZ 2D COMO ARREGLO 1D
======================================================================================
CONCEPTO CLAVE: MAPEO DE COORDENADAS 2D A ÍNDICE 1D
- Una matriz de F filas y C columnas se almacena como un arreglo de tamaño F*C
- Para acceder a la posición (fila, columna), usamos: índice = fila * columnas + columna
- Ejemplo: En una matriz 3x4, la posición (1,2) se mapea al índice: 1*4+2 = 6

VENTAJAS DE ESTA IMPLEMENTACIÓN:
• Mayor eficiencia de memoria (sin punteros intermedios)
• Mejor localidad de referencia (cache-friendly)
• Acceso directo O(1) a cualquier posición
• Fácil implementación de algoritmos de recorrido

IMPORTANTE: Los punteros apuntan a lotes en el sistema maestro, no son copias
======================================================================================*/

// Crea "matriz" 1D de punteros (todas las celdas a nullptr)
// PARÁMETROS: filas y columnas definen las dimensiones del almacén
// RETORNA: Puntero al arreglo de punteros (matriz simulada)
LoteProduccion** crearAlmacen(int filas, int columnas) {
    int N = filas * columnas;  // Calcular tamaño total necesario
    LoteProduccion** A = new LoteProduccion*[N];  // Crear arreglo de punteros
    
    // Inicializar todas las posiciones como vacías (nullptr = posición libre)
    for (int i = 0; i < N; ++i) A[i] = nullptr;
    return A;
}

// Libera la "matriz" (NO borra los lotes, solo el arreglo de punteros)
// IMPORTANTE: Los lotes siguen existiendo en el sistema maestro
void liberarAlmacen(LoteProduccion** A) {
    delete[] A;  // Solo libera el arreglo de punteros, no los lotes referenciados
}

// Coloca puntero en (f,c) si está libre
// ALGORITMO: Convierte coordenadas 2D a índice 1D y verifica disponibilidad
// COMPLEJIDAD: O(1) - acceso directo por índice calculado
bool colocar(LoteProduccion** A, int filas, int columnas, int f, int c, LoteProduccion* ptr) {
    // Paso 1: Validar que las coordenadas estén dentro de los límites
    if (f < 0 || f >= filas || c < 0 || c >= columnas) return false;
    
    // Paso 2: Convertir coordenadas 2D a índice 1D usando la fórmula de mapeo
    int idx = f * columnas + c;  // FÓRMULA CLAVE: fila * total_columnas + columna
    
    // Paso 3: Verificar que la posición esté disponible
    if (A[idx] != nullptr) return false;  // Posición ocupada
    
    // Paso 4: Colocar el puntero al lote en la posición calculada
    A[idx] = ptr;
    return true;  // Colocación exitosa
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

// Buscar por ID en el almacén y devolver posición
bool buscarPorID(LoteProduccion** A, int filas, int columnas, int id, int& fila, int& columna) {
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < columnas; ++c) {
            int idx = f * columnas + c;
            if (A[idx] && A[idx]->idLote == id) {
                fila = f;
                columna = c;
                return true;
            }
        }
    }
    return false;
}

// Remover lote del almacén (libera la posición)
bool removerLote(LoteProduccion** A, Maestro& maestro, int filas, int columnas, int id) {
    int f, c;
    if (buscarPorID(A, filas, columnas, id, f, c)) {
        int idx = f * columnas + c;
        A[idx] = nullptr;  // Libera la posición en el almacén
        maestroEliminar(maestro, id);  // Elimina del sistema maestro
        return true;
    }
    return false;
}

// Mover lote de una posición a otra
bool moverLote(LoteProduccion** A, int filas, int columnas, int filaOrigen, int colOrigen, int filaDestino, int colDestino) {
    // Verificar límites
    if (filaOrigen < 0 || filaOrigen >= filas || colOrigen < 0 || colOrigen >= columnas ||
        filaDestino < 0 || filaDestino >= filas || colDestino < 0 || colDestino >= columnas) {
        return false;
    }
    
    int idxOrigen = filaOrigen * columnas + colOrigen;
    int idxDestino = filaDestino * columnas + colDestino;
    
    // Verificar que origen tenga lote y destino esté vacío
    if (A[idxOrigen] == nullptr || A[idxDestino] != nullptr) {
        return false;
    }
    
    // Mover el lote
    A[idxDestino] = A[idxOrigen];
    A[idxOrigen] = nullptr;
    return true;
}

// Función para mostrar estadísticas completas del almacén
void mostrarEstadisticas(LoteProduccion** A, const Maestro& maestro, int filas, int columnas) {
    int totalPosiciones = filas * columnas;
    int posicionesOcupadas = 0;
    int totalComponentes = 0;
    float pesoTotal = 0.0f;
    
    cout << "\n=== ESTADÍSTICAS DEL ALMACÉN ===" << endl;
    cout << "Dimensiones: " << filas << " x " << columnas << " = " << totalPosiciones << " posiciones" << endl;
    
    // Contar posiciones ocupadas y estadísticas
    for (int i = 0; i < totalPosiciones; ++i) {
        if (A[i] != nullptr) {
            posicionesOcupadas++;
            totalComponentes += A[i]->cantidadTotal;
            pesoTotal += A[i]->pesoUnitario * A[i]->cantidadTotal;
        }
    }
    
    int posicionesLibres = totalPosiciones - posicionesOcupadas;
    float porcentajeOcupacion = (float)posicionesOcupadas / totalPosiciones * 100.0f;
    
    cout << "Posiciones ocupadas: " << posicionesOcupadas << endl;
    cout << "Posiciones libres: " << posicionesLibres << endl;
    cout << "Porcentaje de ocupación: " << fixed << setprecision(1) << porcentajeOcupacion << "%" << endl;
    cout << "Total de lotes activos: " << maestro.size << endl;
    cout << "Total de componentes: " << totalComponentes << " unidades" << endl;
    cout << "Peso total almacenado: " << fixed << setprecision(2) << pesoTotal << " kg" << endl;
    
    // Alertas
    if (porcentajeOcupacion > 90.0f) {
        cout << "ALERTA: Almacén casi lleno (" << porcentajeOcupacion << "%)" << endl;
    } else if (porcentajeOcupacion > 75.0f) {
        cout << "ADVERTENCIA: Almacén con alta ocupación (" << porcentajeOcupacion << "%)" << endl;
    }
}

// Reporte completo del almacén (todas las posiciones)
void reporteCompleto(LoteProduccion** A, int filas, int columnas) {
    cout << "\n=== REPORTE COMPLETO DEL ALMACÉN ===" << endl;
    
    for (int f = 0; f < filas; ++f) {
        cout << "\n--- FILA " << f << " ---" << endl;
        for (int c = 0; c < columnas; ++c) {
            int idx = f * columnas + c;
            cout << "Pos (" << f << "," << c << "): ";
            
            if (A[idx] == nullptr) {
                cout << "VACÍA" << endl;
            } else {
                cout << "ID:" << A[idx]->idLote 
                     << " | " << A[idx]->nombreComponente 
                     << " | " << A[idx]->cantidadTotal << " uds"
                     << " | " << A[idx]->pesoUnitario << " kg/ud" << endl;
            }
        }
    }
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

// Mostrar todo el historial de inspecciones
void mostrarHistorialInspecciones(const Pila& p, const Maestro& maestro) {
    cout << "\n=== HISTORIAL COMPLETO DE INSPECCIONES ===" << endl;
    
    if (pilaVacia(p)) {
        cout << "No hay inspecciones registradas." << endl;
        return;
    }
    
    cout << "Total de inspecciones: " << (p.top + 1) << endl;
    cout << "----------------------------------------" << endl;
    
    // Mostrar desde la más reciente (top) hasta la más antigua (0)
    for (int i = p.top; i >= 0; i--) {
        cout << "Inspección " << (p.top - i + 1) << ":" << endl;
        cout << "  ID Lote: " << p.id[i] << endl;
        
        // Buscar información del lote en el maestro
        int idx = maestroBuscarID(maestro, p.id[i]);
        if (idx != -1) {
            cout << "  Componente: " << maestro.data[idx].nombreComponente << endl;
            cout << "  Cantidad: " << maestro.data[idx].cantidadTotal << " unidades" << endl;
        } else {
            cout << "  Componente: [Lote eliminado]" << endl;
        }
        
        cout << "  Resultado: " << (p.res[i] ? "APROBADO" : "RECHAZADO") << endl;
        cout << "----------------------------------------" << endl;
    }
    
    // Estadísticas del historial
    int aprobados = 0, rechazados = 0;
    for (int i = 0; i <= p.top; i++) {
        if (p.res[i] == 1) aprobados++;
        else rechazados++;
    }
    
    cout << "\nESTADÍSTICAS DEL HISTORIAL:" << endl;
    cout << "Lotes aprobados: " << aprobados << endl;
    cout << "Lotes rechazados: " << rechazados << endl;
    
    if (rechazados > 0) {
        float porcentajeRechazo = (float)rechazados / (aprobados + rechazados) * 100.0f;
        cout << "Tasa de rechazo: " << fixed << setprecision(1) << porcentajeRechazo << "%" << endl;
        
        if (porcentajeRechazo > 20.0f) {
            cout << "ALERTA: Alta tasa de rechazo!" << endl;
        }
    }
}

/*======================================================================================
SISTEMA DE EXPORTACIÓN/IMPORTACIÓN Y BACKUP DE DATOS
======================================================================================
Estas funciones permiten:
- Exportar el estado completo del almacén a un archivo de texto
- Importar datos previamente guardados
- Crear backups automáticos del sistema
- Restaurar desde backups
======================================================================================*/

// Exporta todos los datos del almacén a un archivo de texto
bool exportarDatos(LoteProduccion** A, const Maestro& maestro, int filas, int columnas, const char* nombreArchivo) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "✗ Error: No se pudo crear el archivo " << nombreArchivo << endl;
        return false;
    }
    
    cout << "Exportando datos del almacén..." << endl;
    
    // Escribir encabezado con metadatos
    archivo << "# BACKUP ALMACEN ALPHATECH" << endl;
    archivo << "# Generado automáticamente" << endl;
    archivo << "DIMENSIONES=" << filas << "," << columnas << endl;
    archivo << "TOTAL_LOTES=" << maestro.size << endl;
    archivo << "# Formato: FILA,COLUMNA,ID,NOMBRE,PESO,CANTIDAD" << endl;
    
    // Exportar todos los lotes con sus posiciones
    int lotesExportados = 0;
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < columnas; ++c) {
            int idx = f * columnas + c;
            if (A[idx] != nullptr) {
                archivo << f << "," << c << "," 
                       << A[idx]->idLote << ","
                       << A[idx]->nombreComponente << ","
                       << fixed << setprecision(3) << A[idx]->pesoUnitario << ","
                       << A[idx]->cantidadTotal << endl;
                lotesExportados++;
            }
        }
    }
    
    archivo.close();
    cout << "✓ Datos exportados exitosamente: " << lotesExportados << " lotes guardados en " << nombreArchivo << endl;
    return true;
}

// Importa datos desde un archivo de texto al almacén
bool importarDatos(LoteProduccion**& A, Maestro& maestro, int& filas, int& columnas, const char* nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "✗ Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        return false;
    }
    
    cout << "Importando datos del almacén..." << endl;
    
    string linea;
    int nuevasFilas = 0, nuevasColumnas = 0;
    
    // Leer metadatos del archivo
    while (getline(archivo, linea)) {
        if (linea[0] == '#') continue; // Ignorar comentarios
        
        // Buscar dimensiones
        if (linea.find("DIMENSIONES=") == 0) {
            size_t coma = linea.find(',');
            if (coma != string::npos) {
                nuevasFilas = stoi(linea.substr(12, coma - 12));
                nuevasColumnas = stoi(linea.substr(coma + 1));
            }
        }
        
        // Si encontramos las dimensiones, crear nuevo almacén
        if (nuevasFilas > 0 && nuevasColumnas > 0 && A == nullptr) {
            filas = nuevasFilas;
            columnas = nuevasColumnas;
            A = crearAlmacen(filas, columnas);
            cout << "Almacén recreado: " << filas << "x" << columnas << endl;
        }
        
        // Procesar líneas de datos (que no sean metadatos)
        if (linea.find("=") == string::npos && linea[0] != '#' && !linea.empty()) {
            // Parsear línea de datos: FILA,COLUMNA,ID,NOMBRE,PESO,CANTIDAD
            size_t pos = 0;
            string token;
            vector<string> tokens;
            
            // Separar por comas
            while ((pos = linea.find(',')) != string::npos) {
                token = linea.substr(0, pos);
                tokens.push_back(token);
                linea.erase(0, pos + 1);
            }
            tokens.push_back(linea); // Último token
            
            if (tokens.size() >= 6) {
                int f = stoi(tokens[0]);
                int c = stoi(tokens[1]);
                int id = stoi(tokens[2]);
                string nombre = tokens[3];
                float peso = stof(tokens[4]);
                int cantidad = stoi(tokens[5]);
                
                // Crear lote y colocarlo
                LoteProduccion* ptr = maestroCrear(maestro, id, nombre.c_str(), peso, cantidad);
                if (colocar(A, filas, columnas, f, c, ptr)) {
                    cout << "✓ Lote " << id << " importado en (" << f << "," << c << ")" << endl;
                }
            }
        }
    }
    
    archivo.close();
    cout << "✓ Importación completada. Lotes activos: " << maestro.size << endl;
    return true;
}

// Crea un backup automático con timestamp
bool crearBackup(LoteProduccion** A, const Maestro& maestro, int filas, int columnas) {
    if (!A) {
        cout << "✗ Error: No hay almacén para respaldar." << endl;
        return false;
    }
    
    // Generar nombre de archivo con timestamp simple
    char nombreBackup[100];
    strcpy(nombreBackup, "backup_almacen.txt");
    
    return exportarDatos(A, maestro, filas, columnas, nombreBackup);
}

// Restaura desde un backup
bool restaurarBackup(LoteProduccion**& A, Maestro& maestro, int& filas, int& columnas) {
    // Limpiar almacén actual si existe
    if (A) {
        liberarAlmacen(A);
        A = nullptr;
    }
    maestroFree(maestro);
    maestroInit(maestro);
    
    return importarDatos(A, maestro, filas, columnas, "backup_almacen.txt");
}

// Función para detectar y alertar sobre stock bajo
void verificarStockBajo(LoteProduccion** A, int filas, int columnas, int umbralMinimo = 10) {
    cout << "\n=== VERIFICACIÓN DE STOCK BAJO ===" << endl;
    bool hayAlertas = false;
    
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < columnas; ++c) {
            int idx = f * columnas + c;
            if (A[idx] != nullptr && A[idx]->cantidadTotal <= umbralMinimo) {
                if (!hayAlertas) {
                    cout << "ALERTAS DE STOCK BAJO (≤" << umbralMinimo << " unidades):" << endl;
                    hayAlertas = true;
                }
                cout << "  - Lote " << A[idx]->idLote 
                     << " (" << A[idx]->nombreComponente << "): " 
                     << A[idx]->cantidadTotal << " unidades restantes" << endl;
            }
        }
    }
    
    if (!hayAlertas) {
        cout << "✓ No hay alertas de stock bajo." << endl;
    }
}

/*======================================================================================
FUNCIONES AUXILIARES DE UTILIDAD
======================================================================================
Estas funciones proporcionan utilidades adicionales para el manejo del sistema:
- Limpiar pantalla para mejor presentación
- Pausar ejecución para lectura
- Mostrar ayuda del sistema
======================================================================================*/

// Función para pausar y esperar input del usuario
void pausar() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Mostrar ayuda del sistema
void mostrarAyuda() {
    cout << "\n=== GUÍA RÁPIDA DEL SISTEMA ALPHATECH ===" << endl;
    cout << "\nFUNCIONES PRINCIPALES:" << endl;
    cout << "• Crear almacén: Define las dimensiones de tu almacén (máx 20x20)" << endl;
    cout << "• Colocar lotes: Asigna componentes a posiciones específicas" << endl;
    cout << "• Inspecciones: Lleva control de calidad con historial" << endl;
    cout << "• Búsquedas: Localiza componentes por nombre o ID" << endl;
    cout << "• Reportes: Consulta estadísticas y estados del almacén" << endl;
    
    cout << "\nALERTAS AUTOMÁTICAS:" << endl;
    cout << "• Almacén >75% ocupado: Advertencia de capacidad" << endl;
    cout << "• Almacén >90% ocupado: Alerta crítica" << endl;
    cout << "• Stock bajo: Componentes con ≤10 unidades" << endl;
    cout << "• Alta tasa rechazo: >20% inspecciones rechazadas" << endl;
    
    cout << "\nGESTIÓN DE DATOS:" << endl;
    cout << "• Backup automático: Guarda estado completo" << endl;
    cout << "• Exportar/Importar: Intercambio de datos" << endl;
    cout << "• Restauración: Recupera desde backups" << endl;
    
    cout << "\nCONSEJOS DE USO:" << endl;
    cout << "• IDs únicos: Cada lote debe tener un ID diferente" << endl;
    cout << "• Posiciones: Coordenadas empiezan en (0,0)" << endl;
    cout << "• Historial: Máximo 10 inspecciones en memoria" << endl;
    cout << "• Validación: El sistema verifica todas las entradas" << endl;
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
        cout << "\n--- AlphaTech: Control de Lotes Dinámico ---" << endl;
        cout << "1. Inicializar almacén" << endl;
        cout << "2. Colocar lote" << endl;
        cout << "3. Control de calidad (Inspección)" << endl;
        cout << "4. Deshacer (Pop de Pila)" << endl;
        cout << "5. Reporte por fila" << endl;
        cout << "6. Buscar por componente" << endl;
        cout << "7. Salir" << endl;
        cout << "Opción: ";
        
        opc = validarEntero("", 1, 7);

        switch(opc) {
            case 1: {
                // Inicializar almacén
                if (almacen) {
                    if (confirmarAccion("¿Desea reinicializar el almacén? Se perderán todos los datos")) {
                        liberarAlmacen(almacen);
                        almacen = nullptr;
                    } else {
                        break;
                    }
                }
                
                filas = validarEntero("Ingrese el número de filas (1-20): ", 1, 20);
                columnas = validarEntero("Ingrese el número de columnas (1-20): ", 1, 20);
                
                almacen = crearAlmacen(filas, columnas);
                cout << "✓ Almacén " << filas << "x" << columnas << " creado exitosamente." << endl;
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
                f = validarEntero("", 0, filas-1);
                cout << "Ingrese la columna (0-" << (columnas-1) << "): ";
                c = validarEntero("", 0, columnas-1);
                id = validarEntero("Ingrese el ID del lote (positivo): ", 1, 99999);
                
                // Verificar si ya existe el ID
                if (maestroBuscarID(maestro, id) != -1) {
                    cout << "✗ Error: Ya existe un lote con ID " << id << endl;
                    break;
                }
                
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                validarString("Ingrese el nombre del componente: ", nombre, 50);
                peso = validarFloat("Ingrese el peso unitario (kg): ", 0.001f, 1000.0f);
                cant = validarEntero("Ingrese la cantidad total: ", 1, 100000);

                LoteProduccion* ptr = maestroCrear(maestro, id, nombre, peso, cant);
                if (colocar(almacen, filas, columnas, f, c, ptr)) {
                    cout << "✓ Lote colocado exitosamente en posición (" << f << ", " << c << ")" << endl;
                } else {
                    cout << "✗ Error: No se pudo colocar (posición ocupada)" << endl;
                }
                break;
            }
            
            case 3: {
                // Control de calidad (Inspección)
                int id = validarEntero("Ingrese el ID del lote a inspeccionar: ", 1, 99999);
                
                if (maestroBuscarID(maestro, id) == -1) {
                    cout << "✗ Error: No existe un lote con ID " << id << endl;
                    break;
                }
                
                int resultado = validarEntero("Ingrese el resultado (1=Aprobado, 0=Rechazado): ", 0, 1);
                
                pilaPush(pila, id, resultado);
                cout << "✓ Inspección registrada: Lote " << id << " - " 
                     << (resultado ? "APROBADO" : "RECHAZADO") << endl;
                break;
            }
            
            case 4: {
                // Deshacer (Pop de Pila)
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
                
                cout << "Fila a consultar (0-" << (filas-1) << "): ";
                int f = validarEntero("", 0, filas-1);
                reporteFila(almacen, filas, columnas, f);
                break;
            }
            
            case 6: {
                // Buscar por componente
                if (!almacen) {
                    cout << "✗ Error: No hay almacén inicializado." << endl;
                    break;
                }
                
                char nombre[50];
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                validarString("Ingrese el nombre del componente: ", nombre, 50);
                
                if (!buscarPorNombre(almacen, filas, columnas, nombre)) {
                    cout << "✗ No se encontraron componentes con ese nombre." << endl;
                }
                break;
            }
            
            case 7: {
                // Salir
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
