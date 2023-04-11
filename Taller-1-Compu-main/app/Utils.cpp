#include "Utils.h"
#include <iostream>
#include <math.h>


using namespace std;


std::string Utils::acomodar(std::string texto) {
    /*
     * Pasamos el texto a minusculas (en caso de que se use una variable en mayúscula)
     */
    std::transform(texto.begin(), texto.end(), texto.begin(), [](unsigned char c) {
        return std::tolower(c); });

    /*
     * Truco para tokenizar negativos.
     *
     * Reemplazar el menos por un más y un menos.
     */
    std::string polinomio = std::regex_replace(texto, std::regex("-"), "+-");

    /*
     * Se eliminan los espacios al polinomio
     */
    std::string::iterator it = std::remove(polinomio.begin(), polinomio.end(), ' ');
    polinomio.erase(it, polinomio.end());

    return polinomio;
}

vector<std::string> Utils::obtener_terminos(std::string polinomioStr) {
    std::vector<std::string> elementos;
    if (!polinomioStr.empty()) {

        char* str = new char[polinomioStr.size() + 1];
        strncpy(str, polinomioStr.c_str(), polinomioStr.size());

        char* token = strtok(str, "+");
        while (token != NULL) {
            std::string elemento = token;
            elementos.push_back(elemento);
            token = strtok(NULL, "+");
        }
        delete[] str;
    }
    return elementos;
}

Monomio Utils::str2Monomio(std::string texto) {
    Monomio mono;

    if (!texto.empty()) {
        std::string str = texto;
        std::size_t pos = str.find("x");

        //        std::cout << "Texto: " << texto << " Pos: " << pos << std::endl;
        if (pos != std::string::npos) {
            /// Coeficiente
            std::string coefStr = str.substr(0, pos);
            if (coefStr.empty()) {
                mono.SetCoeficiente(1);
            } else {
                mono.SetCoeficiente(atof(coefStr.c_str()));
            }

            /// Grado
            unsigned int resto = pos + 3;
            if (str.size() >= resto) {
                std::string gradoStr = str.substr(resto, str.size() - 1);
                mono.SetGrado(atof(gradoStr.c_str()));
            } else if (str.size() > pos) {
                mono.SetGrado(1);
            } else {
                mono.SetGrado(0);
            }
        } else {
            mono.SetCoeficiente(atof(str.c_str()));
            mono.SetGrado(0);
        }
    }

    return mono;
}

vector<Monomio> Utils::convertir(std::string polinomioStr) {
    vector<Monomio> listado;
    if (!polinomioStr.empty()) {
        std::string resultado = acomodar(polinomioStr);
        vector<std::string> terminos = obtener_terminos(resultado);

        vector<int>::size_type i = 0;
        vector<Monomio> polinomio;

        for (i = 0; i < terminos.size(); i++) {
            std::string valor = terminos[i];
            Monomio monomio = str2Monomio(valor);
            polinomio.push_back(monomio);
        }

        listado = polinomio;


    }
    return listado;
}

double Utils::evaluar(vector<Monomio> polinomio, double valor) {
    double resultado;
    vector<Monomio>::size_type i;
    double suma = (double) 0;
    for (i = 0; i < polinomio.size(); i++) {
        Monomio monomio = polinomio[i];
        suma += (monomio.GetCoeficiente() * pow(valor, monomio.GetGrado()));
    }
    resultado = suma;
    return resultado;
}


//Funcion que calcula y retorna la derivada en un polinomio
vector<Monomio> Utils::derivar(vector<Monomio> polinomio)
{
       vector<Monomio> deriv;
       cout<<endl;
       cout<<"-Derivada (f(x)'): ";
       vector<Monomio>::size_type i;
       for(i=0; i < polinomio.size(); i++){
           Monomio mon;
           mon = polinomio[i];

           Monomio derivMon;    //Creamos un objeto de la clase Monomio para guardar la derivada del monomio


          /*Realizamos el calculo de la derivada del monomio
           * -Para el nuevo  coeficiente: Multiplicamos el exponente (o grado) del monomio por su coeficiente
           * -Para el nuevo exponente (o grado): Le restamos 1 al exponente actual
           */
          derivMon.SetCoeficiente(mon.GetGrado() * mon.GetCoeficiente());
          derivMon.SetGrado(mon.GetGrado()-1);
          deriv.push_back(derivMon);  //agregamos la derivada calculada al vector "deriv"

          cout<<derivMon.GetCoeficiente() << "x**" << derivMon.GetGrado()<<" ";

    }
    cout<<endl;
    return deriv;
}


double Utils::newthonRaphson(std::vector<Monomio> polinomio)
{
    vector<Monomio> poliDeriv = derivar(polinomio); //llamamos a la funcion "derivar" y asignamos el valor que retorna a la variable "poliDeriv"
    int i = 0;
    double n = 1;
    double resulPoli, resulDeriv, aprox;
    resulPoli = evaluar(polinomio, n);
    resulDeriv = evaluar(poliDeriv, n);
    aprox = resulPoli/resulDeriv;   //Dividimos entre si los valores conseguidos con ayuda de la funcion "evaluar"
    //Realizamos iteraciones 60 veces, ya que mientras mas hagamos mas preciso sera
    while(i<=60){
        resulPoli = evaluar(polinomio, n);
        resulDeriv = evaluar (poliDeriv, n);
        aprox = resulPoli/resulDeriv;
        n = n - aprox;
        i = i+1;
    }
    return n;
}
