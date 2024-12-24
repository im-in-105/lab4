#include "calculator.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

Calculator::Calculator(QWidget *parent) : QWidget(parent) {
    input1 = new QLineEdit(this);
    input2 = new QLineEdit(this);
    resultLabel = new QLabel("Result: ", this);

    QPushButton *addButton = new QPushButton("Add", this);
    QPushButton *subtractButton = new QPushButton("Subtract", this);
    QPushButton *multiplyButton = new QPushButton("Multiply", this);
    QPushButton *divideButton = new QPushButton("Divide", this);

    connect(addButton, &QPushButton::clicked, this, &Calculator::add);
    connect(subtractButton, &QPushButton::clicked, this, &Calculator::subtract);
    connect(multiplyButton, &QPushButton::clicked, this, &Calculator::multiply);
    connect(divideButton, &QPushButton::clicked, this, &Calculator::divide);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(input1);
    layout->addWidget(input2);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(subtractButton);
    buttonLayout->addWidget(multiplyButton);
    buttonLayout->addWidget(divideButton);
    layout->addLayout(buttonLayout);

    layout->addWidget(resultLabel);
}

void Calculator::add() {
    double num1 = input1->text().toDouble();
    double num2 = input2->text().toDouble();
    resultLabel->setText("Result: " + QString::number(num1 + num2));
}

void Calculator::subtract() {
    double num1 = input1->text().toDouble();
    double num2 = input2->text().toDouble();
    resultLabel->setText("Result: " + QString::number(num1 - num2));
}

void Calculator::multiply() {
    double num1 = input1->text().toDouble();
    double num2 = input2->text().toDouble();
    resultLabel->setText("Result: " + QString::number(num1 * num2));
}

void Calculator::divide() {
    double num1 = input1->text().toDouble();
    double num2 = input2->text().toDouble();
    if (num2 != 0)
        resultLabel->setText("Result: " + QString::number(num1 / num2));
    else
        resultLabel->setText("Error: Division by zero");
}
