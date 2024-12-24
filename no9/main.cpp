#include <QApplication>
#include "calculator.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Calculator calc;
    calc.setWindowTitle("Calculator");
    calc.resize(300, 200);
    calc.show();
    return app.exec();
}
