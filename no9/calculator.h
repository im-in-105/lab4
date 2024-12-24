#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>

class QLineEdit;
class QLabel;

class Calculator : public QWidget {
    Q_OBJECT

private:
    QLineEdit *input1, *input2;
    QLabel *resultLabel;

public:
    Calculator(QWidget *parent = nullptr);

private slots:
    void add();
    void subtract();
    void multiply();
    void divide();
};

#endif // CALCULATOR_H
