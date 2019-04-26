#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_load_grammar_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,"Путь к грамматике");
    if(file_name != NULL){
        QFile file(file_name);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QString text = file.readAll();
        grammar.load(text.toLocal8Bit());
        ui->save_way->setText(file_name);
        print_json(text);
        file.close();
    }
}

void MainWindow::print_json(QString original_text){
    if(ui->is_original->isChecked())
        ui->text_result->setText(original_text);
    else if(!grammar.getIsValid())
        ui->text_result->setText("Ошибка задания грамматики.");
    else
        ui->text_result->setText(grammar.get_Json().toJson(QJsonDocument::Indented));
}

void MainWindow::print_json(QString current_tabulation, QJsonObject object){
    current_tabulation.append("\t");
    for(QString key : object.keys()){
        ui->text_result->append("\n"+current_tabulation+key+":"+grammar.get_Json().object().value(key).toString());
        print_json(current_tabulation,grammar.get_Json().object().value(key).toObject());
    }
}


void MainWindow::on_save_grammar_clicked()
{
    QFile file(ui->save_way->text());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    file.write(ui->text_result->document()->toPlainText().toLocal8Bit());
    file.close();
}
