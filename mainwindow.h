#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "grammar.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_load_grammar_clicked();

    void on_save_grammar_clicked();

private:
    Ui::MainWindow *ui;
    Grammar grammar;
    void print_json(QString original_text);
    void print_json(QString, QJsonObject);
};

#endif // MAINWINDOW_H
