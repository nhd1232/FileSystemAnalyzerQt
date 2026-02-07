#pragma once

#include <QMainWindow>
#include <qpushbutton.h>
#include <vector>

#include "config.h"
#include "types.h"

class QListWidget;
class QLineEdit;
class QTextEdit;
class QCheckBox;
class QComboBox;
class QSpinBox;
class QDateEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void addRootDirectory();        // добавить папку
    void removeSelectedRoots();     // удалить выбранные папки
    void chooseHistoryFile();       // выбрать файл истории
    void runAnalysis();             // запустить анализ
    void runTests();                // запустить тесты

private:
    AppConfig buildConfigFromUi() const;
    void showReport(const std::vector<ReportChunk> &report);

    QListWidget *rootsList_;        // список директорий

    QCheckBox   *recursiveCheck_;
    QCheckBox   *saveHistoryCheck_;
    QLineEdit   *historyPathEdit_;

    // фильтры
    QCheckBox   *topKCheck_;
    QSpinBox    *topKSpin_;

    QCheckBox   *sizeCheck_;
    QLineEdit   *sizeEdit_;

    QCheckBox   *dateCheck_;
    QDateEdit   *dateEdit_;

    QComboBox   *dupModeCombo_;     // режим поиска дубликатов

    // выбор анализа
    QComboBox   *analysisCombo_;

    // вывод
    QTextEdit   *outputText_;
    QPushButton *testsButton_;
};
