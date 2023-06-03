#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <ctime>
// Grupo C - Computación Paralela.
// Clase Producto
using namespace std;

class Producto {
private:
    int codigo;
    int precios[12]{};
public:
    explicit Producto(int codigo);
    int getCodigo() const;
    void setPrecio(int mes, int precio);
    int getPrecio(int mes) const;

};
// Constructor de Producto
Producto::Producto(int codigo) {
    this->codigo = codigo;
    for (int & precio : precios) {
        precio = 0;
    }
}
// Métodos de Producto
int Producto::getCodigo() const {
    return codigo;
}

void Producto::setPrecio(int mes, int precio) {
    precios[mes] = precio;
}

int Producto::getPrecio(int mes) const {
    return precios[mes];
}

// Declaraciones
// Función para remover las comillas
string removeQuotes(const string& str);
// Función para crear lista de productos desde archivo CSV
unordered_map<int, Producto> crearTablaProductos();

// Procedimiento para Actualizar los precios posteriores a "2022-01" Se omiten descuentos en este caso
void agregarPreciosDescuentos(unordered_map<int, Producto>& tablaProductos);

// Procedimiento que elimina todos los productos de la tabla hash que en alguno de sus posiciones de precios[x] sea = a 0 (Es decir no se actualizó en algún mes)
void eliminarProductosSinPrecio(unordered_map<int, Producto>& tablaProductos);

// Cálculo de la inflación
void calcularInflacion(const unordered_map<int, Producto>& tablaProductos);

// Formula más representativa para la inflación acumulada de un promedio mensual
double calcularInflacionInteresCompuesto(double, int);

// Función principal
int main() {
    clock_t inicio = clock();
    unordered_map<int, Producto> tablaProductos = crearTablaProductos();

    cout<<"Se termino la tabla del primer mes"<<endl;
    agregarPreciosDescuentos(tablaProductos);// Procesar meses 2 a 12 mes = 2 febrero se supone
    cout<<"Se terminó de actualizar precios"<<endl;
    eliminarProductosSinPrecio(tablaProductos);
    calcularInflacion(tablaProductos);
    clock_t fin = clock();
    double tiempoTotal = double(fin - inicio) / CLOCKS_PER_SEC;
    cout << "Tiempo total: " << tiempoTotal << " segundos" << endl;
    return 0;
}
string removeQuotes(const string& str) {
    string result = str;
    result.erase(remove_if(result.begin(), result.end(), [](char c) {
        return c == '"';
    }), result.end());
    return result;
}

unordered_map<int, Producto> crearTablaProductos() {
    ifstream archivo("/srv/utem/supermercado.csv");
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return {};
    }
    string linea;
    getline(archivo, linea); // Omitir encabezado

    unordered_map<int, Producto> tablaProductos;

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string codigo, nombre, precio, descuento, fecha, estado;
        // Leer el campo del código y eliminar comillas dobles
        getline(ss, codigo, ';'); // Código del producto
        codigo = removeQuotes(codigo);
        getline(ss, nombre, ';'); // Nombre del producto
        getline(ss, precio, ';'); // Precio
        precio = removeQuotes(precio);
        getline(ss, descuento, ';'); // Descuento
        getline(ss, fecha, ';'); // Fecha
        fecha = removeQuotes(fecha);
        fecha = fecha.substr(0,7);
        getline(ss, estado, '\n'); // Estado
        estado.erase(remove(estado.begin(), estado.end(), '\"'), estado.end());

        // Verificar si el estado es FINALIZED o AUTHORIZED
        if ((estado == "FINALIZED" || estado == "AUTHORIZED") && fecha == "2022-01") {//Se agregan solo los del primer mes para tener como filtro
            int codigoInt;
            istringstream(codigo) >> codigoInt;
            // Verificar si el producto ya existe en la tabla
            auto iter = tablaProductos.find(codigoInt);
            if (iter != tablaProductos.end()) {
                // Producto encontrado
            } else {
                // Producto no encontrado, crear un nuevo objeto Producto y agregarlo a la tabla
                int precioInt;
                istringstream(precio) >> precioInt;
                tablaProductos.emplace(codigoInt, Producto(codigoInt));
                Producto& nuevoProducto = tablaProductos.at(codigoInt);
                nuevoProducto.setPrecio(0, precioInt);
            }

        }
    }
    archivo.close();
    return tablaProductos;
}

void agregarPreciosDescuentos(unordered_map<int, Producto>& tablaProductos) {
    int mes;
    string linea, codigo, nombre, precio, descuento, fecha, estado, mes1;
    ifstream archivo("/srv/utem/supermercado.csv");
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return;
    }
    //Actualiza precios y descuentos
    while (getline(archivo, linea)) {
        stringstream ss(linea);

        getline(ss, codigo, ';'); // Código del producto
        codigo = removeQuotes(codigo);
        getline(ss, nombre, ';'); // Nombre del producto
        getline(ss, precio, ';'); // Precio
        precio = removeQuotes(precio);
        getline(ss, descuento, ';'); // Descuento
        getline(ss, fecha, ';'); // Fecha
        fecha = removeQuotes(fecha);
        fecha = fecha.substr(0,7);
        getline(ss, estado, '\n'); // Estado
        estado.erase(remove(estado.begin(), estado.end(), '\"'), estado.end());

        //Se desconoce si el archivo está ordenado por fechas.
        if ( (estado == "AUTHORIZED" || estado == "FINALIZED") && fecha!="2022-01") {//suponemos que es solo 2022
            // Verificar si el producto está presente en la tabla de productos
            int codigoInt;
            istringstream(codigo) >> codigoInt;
            auto iter = tablaProductos.find(codigoInt);
            if (iter != tablaProductos.end() ) {
                // Producto encontrado, actualizar precio y descuento en el mes correspondiente
                int precioInt;
                istringstream(precio) >> precioInt;
                mes1 = fecha.substr(5, 2);
                mes = stoi(mes1);
                Producto& productoExistente = iter->second;
                productoExistente.setPrecio(mes -1, precioInt);
            }
        }
    }
    archivo.close();
}

void eliminarProductosSinPrecio(unordered_map<int, Producto>& tablaProductos) {
    for (auto it = tablaProductos.begin(); it != tablaProductos.end();) {
        bool eliminar = false;
        const Producto& producto = it->second;
        for (int mes = 1; mes <= 11; ++mes) {
            if (producto.getPrecio(mes) == 0) {
                eliminar = true;
                break;
            }
        }
        if (eliminar) {
            it = tablaProductos.erase(it);
        } else {
            ++it;
        }
    }
}
double calcularInflacionInteresCompuesto(double tasa, int periodos) {
    double resultado = 1.0;
    for (int i = 0; i < periodos; ++i) {
        resultado *= (1.0 + tasa);
    }
    resultado -= 1.0;
    return resultado * 100.0;
}
void calcularInflacion(const unordered_map<int, Producto>& tablaProductos) {
    // Obtener precios y descuentos del mes de enero
    streamsize oldPrecision = cout.precision();
    cout.precision(4);
    double sumaPreciosEnero = 0.0 ;
    double inflacionAcumulada = 0.0;
    double inflacionPromedioMensual ;
    for (const auto& pair : tablaProductos) {
        const Producto& producto = pair.second;
        sumaPreciosEnero += producto.getPrecio(0);
    }

    for (int mes = 1; mes <= 11; ++mes) {// Calcular inflación mensual desde febrero hasta diciembre
        double sumaPreciosMesActual = 0.0;
        for (const auto& pair : tablaProductos) {// Obtener suma total de precios descontados en el mes actual
            const Producto& producto = pair.second;
            sumaPreciosMesActual += producto.getPrecio(mes);
        }
        // Calcular inflación mensual respecto al mes de referencia anterior
        double inflacionMensual = 0.0;
        if (sumaPreciosEnero != 0.0) {
            inflacionMensual = ((sumaPreciosMesActual - sumaPreciosEnero) / sumaPreciosEnero) * 100.0;
        }
        cout << "Mes " << mes +1 << ": Inflación = " << inflacionMensual << "%" << endl;
        inflacionAcumulada += inflacionMensual;// Acumular inflación mensual para obtener inflación acumulada
    }
    // Calcular inflación promedio mensual
    inflacionPromedioMensual = inflacionAcumulada / 11.0;

    cout << "Inflación acumulada: " << inflacionAcumulada << "%" << endl;
    cout << "Inflación promedio mensual: " << inflacionPromedioMensual << "%" << endl;

    // Fórmula del interés compuesto
    double inflacionInteresCompuesto = calcularInflacionInteresCompuesto(inflacionPromedioMensual / 100.0, 11);

    cout << "Inflación obtenida con interés compuesto: " << inflacionInteresCompuesto << "%" << endl;
    cout.precision(oldPrecision);

}
