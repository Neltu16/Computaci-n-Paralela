#ifndef UTILS_H
#define UTILS_H

#include "Monomio.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

/**
 * Espacio de nombres para utilidades de manejo de monomios/polinomios
 */
namespace Utils {

/**
 *
 * @param texto cadena de texto
 * @return String sin espacios
 */
std::string acomodar(std::string texto);

/**
 *
 * @param polinomioStr representación de texto del polinomio
 * @return Un arreglo con los terminos separados.
 */
std::vector<std::string> obtener_terminos(std::string polinomioStr);

/**
 *
 * @param texto cadena de texto
 * @return Objeto monomio
 */
Monomio str2Monomio(std::string texto);

/**
 *
 * @param polinomioStr Representación de string del polinomio
 * @return arreglo del polinomio.
 */
std::vector<Monomio> convertir(std::string polinomioStr);

/**
 *
 * @param polinomio Arreglo de polinomios
 * @param value Elemento a evaluar
 * @return el valor resultante
 */
double evaluar(std::vector<Monomio> polinomio, double value);


/**
 * funcion que calcula las derivadas del polinomio ingresado
 * @param polinomio arreglo de polinomios
 * @return polinomio con las derivadas calculadas
 */

std::vector<Monomio>derivar(std::vector<Monomio> polinomio);

/**
 * Funcion que calcula las raices (o soluciones) del polinomio ingresado haciendo uso del metodo Newthon-Raphson
 *@param polinomio arreglo de monomios
 *@return la solucion o raices del Polinomio ingresado
 *
*/
double newthonRaphson(std::vector<Monomio> polinomio);

} // namespace Utils

#endif /* UTILS_H */
