#include "App.hpp"
#include <iostream>
#include <exception>

int main() {
    try {
        App app;
        app.run();
    } catch(const std::exception& error) {
        std::cerr << "An error occured: " << error.what() << std::endl;
    }
    catch(...) {
        std::cerr << "An unknown error occured: " << std::endl;
    }
}
