// Compilar: c++ -std=c++17 -O2 -pthread squares.cpp -o squares
// Ejecutar: ./squares n L rotados [--todas]
// La matriz contiene [id, x del centro, y del centro, rotación en radianes].
// Se dibuja la primera solución en cuadrados_cpp.svg.
// Exactamente "rotados" cuadrados tienen 45 grados; los demás tienen 0.
// La búsqueda usa centros enteros; no es continua.

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

constexpr double SEMILADO = 5.0;
const double PI = std::acos(-1.0);
constexpr int HILOS = 6;

struct Cuadrado {
    int x;
    int y;
    int angulo;
    double coseno;
    double seno;
    double radio;
};

struct Bordes {
    double min_x;
    double max_x;
    double min_y;
    double max_y;
};

Cuadrado crear_cuadrado(int x, int y, int angulo) {
    const double radianes = angulo * PI / 180.0;
    const double c = std::cos(radianes);
    const double s = std::sin(radianes);
    return {x, y, angulo, c, s, SEMILADO * (std::abs(c) + std::abs(s))};
}

bool se_sobreponen(const Cuadrado& a, const Cuadrado& b) {
    const double dx = static_cast<double>(b.x) - a.x;
    const double dy = static_cast<double>(b.y) - a.y;
    for (const Cuadrado* cuadrado : {&a, &b}) {
        const std::array<std::pair<double, double>, 2> ejes{{
            {cuadrado->coseno, cuadrado->seno},
            {-cuadrado->seno, cuadrado->coseno},
        }};
        for (const auto& eje : ejes) {
            const double ex = eje.first;
            const double ey = eje.second;
            const auto radio = [ex, ey](const Cuadrado& c) {
                return SEMILADO * (std::abs(ex * c.coseno + ey * c.seno) +
                                   std::abs(-ex * c.seno + ey * c.coseno));
            };
            if (std::abs(dx * ex + dy * ey) >= radio(a) + radio(b)) {
                return false;
            }
        }
    }
    return true;
}

std::vector<Cuadrado> crear_candidatos(int limite) {
    std::vector<Cuadrado> candidatos;
    const int maximo = limite - 10;
    const int paso = static_cast<int>(std::ceil(10 + 10 / std::sqrt(2.0)));
    const auto agregar = [&](int x, int y) {
        for (int angulo : {0, 45}) {
            candidatos.push_back(crear_cuadrado(x, y, angulo));
        }
    };
    for (int desplazamiento : {0, paso / 2}) {
        for (long long y = desplazamiento; y < maximo; y += paso) {
            for (long long x = desplazamiento; x < maximo; x += paso) {
                agregar(static_cast<int>(x), static_cast<int>(y));
            }
        }
    }
    for (int y = 0; y < maximo; ++y) {
        for (int x = 0; x < maximo; ++x) {
            if ((x % paso == 0 && y % paso == 0) ||
                (x % paso == paso / 2 && y % paso == paso / 2)) {
                continue;
            }
            agregar(x, y);
        }
    }
    return candidatos;
}

Bordes limites(const Cuadrado& c) {
    return {c.x - c.radio, c.x + c.radio, c.y - c.radio, c.y + c.radio};
}

Bordes combinar(const Bordes& a, const Bordes& b) {
    return {std::min(a.min_x, b.min_x), std::max(a.max_x, b.max_x),
            std::min(a.min_y, b.min_y), std::max(a.max_y, b.max_y)};
}

double lado_contenedor(const Bordes& b) {
    return std::max(b.max_x - b.min_x, b.max_y - b.min_y);
}

void guardar_grafica(const std::vector<Cuadrado>& cuadrados, const Bordes& bordes) {
    std::ofstream archivo("cuadrados_cpp.svg");
    archivo.exceptions(std::ios::failbit | std::ios::badbit);
    archivo << std::setprecision(17);
    const double lado = lado_contenedor(bordes);
    const double margen = std::max(lado * 0.08, 1.0);
    const double dimension = lado + 2 * margen;
    archivo << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"600\" height=\"600\""
            << " viewBox=\"" << bordes.min_x - margen << ' '
            << -bordes.min_y - lado - margen << ' ' << dimension << ' ' << dimension
            << "\">\n<title>Cuadrados y contenedor</title>\n"
            << "<rect x=\"" << bordes.min_x << "\" y=\"" << -bordes.min_y - lado
            << "\" width=\"" << lado << "\" height=\"" << lado
            << "\" fill=\"white\" stroke=\"black\" stroke-width=\"0.15\"/>\n";
    for (std::size_t i = 0; i < cuadrados.size(); ++i) {
        const auto& c = cuadrados[i];
        archivo << "<polygon points=\"";
        const std::array<std::pair<double, double>, 4> vertices{{
            {-SEMILADO, -SEMILADO}, {SEMILADO, -SEMILADO},
            {SEMILADO, SEMILADO}, {-SEMILADO, SEMILADO},
        }};
        for (const auto& [dx, dy] : vertices) {
            archivo << c.x + dx * c.coseno - dy * c.seno << ','
                    << -(c.y + dx * c.seno + dy * c.coseno) << ' ';
        }
        archivo << "\" fill=\"#b9ddf4\" stroke=\"#17628e\" stroke-width=\"0.12\"/>\n"
                << "<text x=\"" << c.x << "\" y=\"" << -c.y
                << "\" text-anchor=\"middle\" dominant-baseline=\"central\""
                << " font-family=\"sans-serif\" font-size=\"2\" fill=\"#123\">"
                << i + 1 << "</text>\n";
    }
    archivo << "</svg>\n";
    archivo.close();
}

class Busqueda {
public:
    Busqueda(int n, int limite, int rotados, bool todas)
        : n_(n), limite_(limite), rotados_(rotados), todas_(todas),
          candidatos_(crear_candidatos(limite)) {}

    void ejecutar() {
        std::vector<std::thread> hilos;
        try {
            for (int i = 0; i < HILOS; ++i) {
                hilos.emplace_back([this] { trabajar(); });
            }
        } catch (...) {
            detener_ = true;
            for (auto& hilo : hilos) hilo.join();
            throw;
        }
        for (auto& hilo : hilos) hilo.join();
        if (error_) std::rethrow_exception(error_);
        if (cantidad_ == 0) {
            std::cout << "No hay solución en la cuadrícula y los ángulos probados.\n";
        } else if (todas_) {
            std::cout << "Total de soluciones discretas: " << cantidad_ << '\n';
        }
    }

private:
    void trabajar() {
        try {
            while (!detener_) {
                const std::size_t indice = siguiente_++;
                if (indice >= candidatos_.size()) return;
                const Cuadrado& primero = candidatos_[indice];
                std::vector<Cuadrado> elegidos{primero};
                recorrer(indice + 1, elegidos, limites(primero), primero.angulo == 45);
            }
        } catch (...) {
            std::lock_guard<std::mutex> bloqueo(mutex_);
            if (!error_) error_ = std::current_exception();
            detener_ = true;
        }
    }

    void recorrer(std::size_t inicio, std::vector<Cuadrado>& elegidos,
                  const Bordes& bordes, int rotados) {
        const int restantes = n_ - static_cast<int>(elegidos.size());
        if (detener_ || lado_contenedor(bordes) >= limite_ ||
            rotados > rotados_ || rotados + restantes < rotados_) return;
        if (elegidos.size() == static_cast<std::size_t>(n_)) {
            const bool x_cero = std::any_of(elegidos.begin(), elegidos.end(),
                                            [](const Cuadrado& c) { return c.x == 0; });
            const bool y_cero = std::any_of(elegidos.begin(), elegidos.end(),
                                            [](const Cuadrado& c) { return c.y == 0; });
            if (x_cero && y_cero) publicar(elegidos, bordes);
            return;
        }
        for (std::size_t i = inicio; i < candidatos_.size() && !detener_; ++i) {
            const auto& candidato = candidatos_[i];
            const Bordes nuevos = combinar(bordes, limites(candidato));
            if (lado_contenedor(nuevos) >= limite_) continue;
            if (std::any_of(elegidos.begin(), elegidos.end(), [&](const Cuadrado& c) {
                    return se_sobreponen(candidato, c);
                })) continue;
            elegidos.push_back(candidato);
            recorrer(i + 1, elegidos, nuevos, rotados + (candidato.angulo == 45));
            elegidos.pop_back();
        }
    }

    void publicar(const std::vector<Cuadrado>& elegidos, const Bordes& bordes) {
        std::lock_guard<std::mutex> bloqueo(mutex_);
        if (detener_) return;
        if (cantidad_ == 0) guardar_grafica(elegidos, bordes);
        ++cantidad_;
        std::cout << std::setprecision(17)
                  << "Solución " << cantidad_ << ": lado contenedor = "
                  << lado_contenedor(bordes) << " < " << limite_ << '\n';
        if (cantidad_ == 1) std::cout << "Gráfica guardada en: cuadrados_cpp.svg\n";
        std::cout << "Matriz [id, x del centro, y del centro, rotación en radianes]:\n[";
        for (std::size_t i = 0; i < elegidos.size(); ++i) {
            if (i != 0) std::cout << ", ";
            const auto& c = elegidos[i];
            std::cout << '[' << i + 1 << ", " << c.x << ", " << c.y << ", "
                      << c.angulo * PI / 180.0 << ']';
        }
        std::cout << "]\n";
        if (!todas_) detener_ = true;
    }

    int n_;
    int limite_;
    int rotados_;
    bool todas_;
    std::vector<Cuadrado> candidatos_;
    std::atomic<std::size_t> siguiente_{0};
    std::atomic<bool> detener_{false};
    std::mutex mutex_;
    std::size_t cantidad_ = 0;
    std::exception_ptr error_;
};

int leer_entero(const char* texto) {
    std::size_t leidos = 0;
    const std::string entrada(texto);
    const long long valor = std::stoll(entrada, &leidos);
    if (leidos != entrada.size() || valor < 0 || valor > std::numeric_limits<int>::max()) {
        throw std::invalid_argument("se esperaban enteros no negativos");
    }
    return static_cast<int>(valor);
}

int main(int argc, char* argv[]) {
    if (argc < 4 || argc > 5 || (argc == 5 && std::string(argv[4]) != "--todas")) {
        std::cerr << "Uso: " << argv[0] << " n L rotados [--todas]\n";
        return 1;
    }
    try {
        const int n = leer_entero(argv[1]);
        const int limite = leer_entero(argv[2]);
        const int rotados = leer_entero(argv[3]);
        if (limite == 0) throw std::invalid_argument("L debe ser positivo");
        if (rotados > n) throw std::invalid_argument("rotados no puede ser mayor que n");
        const bool todas = argc == 5;
        if (n == 0) {
            guardar_grafica({}, {0, 0, 0, 0});
            std::cout << "Solución 1: lado contenedor = 0 < " << limite
                      << "\nGráfica guardada en: cuadrados_cpp.svg\n"
                      << "Matriz [id, x del centro, y del centro, rotación en radianes]:\n[]\n";
            if (todas) std::cout << "Total de soluciones discretas: 1\n";
        } else if (limite <= 10 || 100.0 * n >= static_cast<double>(limite) * limite) {
            std::cout << "No hay solución en la cuadrícula y los ángulos probados.\n";
        } else {
            Busqueda(n, limite, rotados, todas).ejecutar();
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
