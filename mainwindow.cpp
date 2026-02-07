#include "mainwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QComboBox>
#include <QAbstractItemView>
#include <QFont>
#include <QGroupBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QDate>
#include <QTime>
#include <QDateTime>

#include <chrono>
#include <stdexcept>
#include <utility>

#include "analyzer.h"
#include "tests.h"
#include "history_reporter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    // ---- директории ----
    auto *rootsLabel = new QLabel(tr("Директории для анализа:"), this);
    mainLayout->addWidget(rootsLabel);

    rootsList_ = new QListWidget(this);
    rootsList_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mainLayout->addWidget(rootsList_);

    auto *rootsButtonsLayout = new QHBoxLayout;
    auto *addRootBtn = new QPushButton(tr("Добавить папку"), this);
    auto *removeRootBtn = new QPushButton(tr("Удалить выбранные"), this);

    connect(addRootBtn, &QPushButton::clicked,
            this, &MainWindow::addRootDirectory);
    connect(removeRootBtn, &QPushButton::clicked,
            this, &MainWindow::removeSelectedRoots);

    rootsButtonsLayout->addWidget(addRootBtn);
    rootsButtonsLayout->addWidget(removeRootBtn);
    rootsButtonsLayout->addStretch();
    mainLayout->addLayout(rootsButtonsLayout);

    // ---- рекурсивный обход ----
    recursiveCheck_ = new QCheckBox(
        tr("Сканировать рекурсивно (включая вложенные папки)"), this);
    recursiveCheck_->setChecked(true);
    mainLayout->addWidget(recursiveCheck_);

    // ---- история ----
    saveHistoryCheck_ = new QCheckBox(
        tr("Сохранять результаты в файл истории"), this);
    mainLayout->addWidget(saveHistoryCheck_);

    auto *historyLayout = new QHBoxLayout;
    auto *historyLabel = new QLabel(tr("Файл истории:"), this);
    historyPathEdit_ = new QLineEdit(this);
    historyPathEdit_->setText(QStringLiteral("history.txt"));
    auto *historyButton = new QPushButton(tr("Обзор"), this);

    connect(historyButton, &QPushButton::clicked,
            this, &MainWindow::chooseHistoryFile);

    historyLayout->addWidget(historyLabel);
    historyLayout->addWidget(historyPathEdit_);
    historyLayout->addWidget(historyButton);
    mainLayout->addLayout(historyLayout);

    // ---- группа фильтров ----
    auto *filtersGroup = new QGroupBox(tr("Настройки фильтров"), this);
    auto *filtersLayout = new QVBoxLayout(filtersGroup);

    // Top-K
    {
        auto *row = new QHBoxLayout;
        topKCheck_ = new QCheckBox(
            tr("Ограничить количество результатов (Top K)"), this);
        topKSpin_ = new QSpinBox(this);
        topKSpin_->setRange(1, 1000);
        topKSpin_->setValue(10);
        row->addWidget(topKCheck_);
        row->addStretch();
        row->addWidget(new QLabel(tr("K:"), this));
        row->addWidget(topKSpin_);
        filtersLayout->addLayout(row);
    }

    // Фильтр по размеру
    {
        auto *row = new QHBoxLayout;
        sizeCheck_ = new QCheckBox(
            tr("Фильтровать по минимальному размеру (в байтах)"), this);
        sizeEdit_ = new QLineEdit(this);
        sizeEdit_->setPlaceholderText(tr("например, 1048576 (1 MB)"));
        row->addWidget(sizeCheck_);
        row->addStretch();
        row->addWidget(sizeEdit_, 1);
        filtersLayout->addLayout(row);
    }

    // Фильтр по дате
    {
        auto *row = new QHBoxLayout;
        dateCheck_ = new QCheckBox(
            tr("Показывать только файлы новее даты"), this);
        dateEdit_ = new QDateEdit(this);
        dateEdit_->setDisplayFormat("dd.MM.yyyy");
        dateEdit_->setCalendarPopup(true);
        dateEdit_->setDate(QDate::currentDate());
        row->addWidget(dateCheck_);
        row->addStretch();
        row->addWidget(dateEdit_);
        filtersLayout->addLayout(row);
    }

    // Режим поиска дубликатов
    {
        auto *row = new QHBoxLayout;
        auto *label = new QLabel(tr("Режим поиска дубликатов:"), this);
        dupModeCombo_ = new QComboBox(this);
        dupModeCombo_->addItem(tr("По имени"));             // 0
        dupModeCombo_->addItem(tr("По размеру"));           // 1
        dupModeCombo_->addItem(tr("По имени и размеру"));   // 2
        dupModeCombo_->setCurrentIndex(2); // как в дефолтном конфиге

        row->addWidget(label);
        row->addWidget(dupModeCombo_, 1);
        filtersLayout->addLayout(row);
    }

    mainLayout->addWidget(filtersGroup);

    // ---- выбор типа анализа ----
    auto *analysisLayout = new QHBoxLayout;
    auto *analysisLabel = new QLabel(tr("Тип анализа:"), this);
    analysisCombo_ = new QComboBox(this);

    analysisCombo_->addItem(tr("Все анализы"));               // 0
    analysisCombo_->addItem(tr("Общая статистика"));          // 1 -> CountSize
    analysisCombo_->addItem(tr("Самые большие файлы"));       // 2 -> Biggest
    analysisCombo_->addItem(tr("Самые новые файлы"));         // 3 -> Newest
    analysisCombo_->addItem(tr("Группировка по расширениям"));// 4 -> Exts
    analysisCombo_->addItem(tr("Поиск дубликатов"));          // 5 -> Dups
    analysisCombo_->addItem(tr("Категории файлов"));          // 6 -> Category

    analysisLayout->addWidget(analysisLabel);
    analysisLayout->addWidget(analysisCombo_, 1);
    mainLayout->addLayout(analysisLayout);

    // ---- кнопки ----
    auto *buttonsLayout = new QHBoxLayout;

    auto *runButton = new QPushButton(tr("Запустить анализ"), this);
    testsButton_ = new QPushButton(tr("Запустить тесты"), this);

    buttonsLayout->addWidget(runButton);
    buttonsLayout->addWidget(testsButton_);
    buttonsLayout->addStretch();

    mainLayout->addLayout(buttonsLayout);

    connect(runButton, &QPushButton::clicked,
            this, &MainWindow::runAnalysis);

    connect(testsButton_, &QPushButton::clicked,
            this, &MainWindow::runTests);

    // ---- вывод отчёта ----
    outputText_ = new QTextEdit(this);
    outputText_->setReadOnly(true);

    // моноширинный шрифт
    QFont monoFont;
    monoFont.setStyleHint(QFont::Monospace);
#if defined(Q_OS_WIN)
    monoFont.setFamily(QStringLiteral("Consolas"));
#else
    monoFont.setFamily(QStringLiteral("Monospace"));
#endif
    outputText_->setFont(monoFont);

    mainLayout->addWidget(outputText_);

    setCentralWidget(central);
    setWindowTitle(tr("Анализатор файловой системы"));
    resize(950, 700);
}

// ---------------------- слоты -------------------------

void MainWindow::addRootDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Выберите директорию"),
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;

    // не добавляем дубликаты
    for (int i = 0; i < rootsList_->count(); ++i) {
        if (rootsList_->item(i)->text() == dir)
            return;
    }

    rootsList_->addItem(dir);
}

void MainWindow::removeSelectedRoots()
{
    auto selected = rootsList_->selectedItems();
    for (QListWidgetItem *item : selected) {
        delete item; // удаляет элемент из списка
    }
}

void MainWindow::chooseHistoryFile()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Выберите файл истории"),
        historyPathEdit_->text().isEmpty()
            ? QStringLiteral("history.txt")
            : historyPathEdit_->text());

    if (fileName.isEmpty())
        return;

    historyPathEdit_->setText(fileName);
}

// ---------------------- сборка AppConfig -------------------------

AppConfig MainWindow::buildConfigFromUi() const
{
    AppConfig cfg;

    // директории
    std::vector<std::string> roots;
    roots.reserve(static_cast<std::size_t>(rootsList_->count()));
    for (int i = 0; i < rootsList_->count(); ++i) {
        // В Qt 6 это UTF-8 (что нам и нужно для корректной кириллицы)
        roots.push_back(rootsList_->item(i)->text().toStdString());
    }
    cfg.roots = std::move(roots);

    cfg.recursive = recursiveCheck_->isChecked();

    // --- top_k ---
    if (topKCheck_->isChecked()) {
        cfg.has_top_k = true;
        cfg.top_k = static_cast<unsigned int>(topKSpin_->value());
    } else {
        cfg.has_top_k = false;
    }

    // --- size_threshold ---
    if (sizeCheck_->isChecked()) {
        bool ok = false;
        QString text = sizeEdit_->text().trimmed();
        quint64 value = text.toULongLong(&ok);
        if (!ok) {
            throw std::runtime_error(
                "Некорректное значение минимального размера файла.");
        }
        cfg.has_size_threshold = true;
        cfg.size_threshold = static_cast<unsigned long long>(value);
    } else {
        cfg.has_size_threshold = false;
    }

    // --- date_threshold ---
    if (dateCheck_->isChecked()) {
        cfg.has_date_threshold = true;
        QDate date = dateEdit_->date();
        QDateTime dt(date, QTime(0, 0), Qt::LocalTime);
        qint64 secs = dt.toSecsSinceEpoch();

        auto tp = std::chrono::time_point_cast<std::chrono::seconds>(
            std::chrono::system_clock::from_time_t(static_cast<time_t>(secs)));
        cfg.date_threshold = tp;
    } else {
        cfg.has_date_threshold = false;
    }

    // --- dup_mode ---
    switch (dupModeCombo_->currentIndex()) {
    case 0:
        cfg.dup_mode = DupMode::ByName;
        break;
    case 1:
        cfg.dup_mode = DupMode::BySize;
        break;
    case 2:
    default:
        cfg.dup_mode = DupMode::ByNameAndSize;
        break;
    }

    // --- history ---
    if (saveHistoryCheck_->isChecked()) {
        // В Qt 6 это UTF-8
        cfg.history_path = historyPathEdit_->text().trimmed().toStdString();
    } else {
        cfg.history_path.clear();
    }

    return cfg;
}

// ---------------------- вывод отчёта -------------------------

void MainWindow::showReport(const std::vector<ReportChunk> &report)
{
    QString html;

    for (std::size_t i = 0; i < report.size(); ++i) {
        const auto &chunk = report[i];

        if (!chunk.title.empty()) {
            // FIX: было fromLocal8Bit -> стало fromUtf8
            QString title = QString::fromUtf8(chunk.title.c_str()).toHtmlEscaped();
            html += QString("<h3>%1</h3>\n").arg(title);
        }

        if (!chunk.lines.empty()) {
            html += "<pre>";
            for (const auto &line : chunk.lines) {
                // FIX: было fromLocal8Bit -> стало fromUtf8
                QString text = QString::fromUtf8(line.text.c_str()).toHtmlEscaped();
                html += text;
                html += '\n';
            }
            html += "</pre>";
        }

        if (i + 1 < report.size()) {
            html += "<hr/>";
        }
    }

    outputText_->setHtml(html);
}

// ---------------------- запуск анализа -------------------------

void MainWindow::runAnalysis()
{
    if (rootsList_->count() == 0) {
        QMessageBox::warning(this,
                             tr("Ошибка"),
                             tr("Добавьте хотя бы одну директорию для анализа."));
        return;
    }

    AppConfig cfg;
    try {
        cfg = buildConfigFromUi();
    } catch (const std::exception &ex) {
        QMessageBox::warning(this,
                             tr("Ошибка в настройках"),
                             // FIX: было fromLocal8Bit -> стало fromUtf8
                             tr("%1").arg(QString::fromUtf8(ex.what())));
        return;
    }

    try {
        Analyzer analyzer(cfg);
        std::vector<ReportChunk> report;

        int mode = analysisCombo_->currentIndex();
        if (mode == 0) {
            // все анализы
            report = analyzer.RunAnalysis();
        } else {
            // один конкретный анализ
            int analysisIndex = mode - 1; // 0..5
            report = analyzer.RunSelected({ analysisIndex });
        }

        showReport(report);

        if (!cfg.history_path.empty()) {
            HistoryReporter historyReporter(cfg.history_path);
            historyReporter.Report(report, cfg);
        }
    }
    catch (const std::exception &ex) {
        QMessageBox::critical(this,
                              tr("Фатальная ошибка"),
                              // FIX: было fromLocal8Bit -> стало fromUtf8
                              tr("Во время анализа произошла ошибка:\n%1")
                                  .arg(QString::fromUtf8(ex.what())));
    }
    catch (...) {
        QMessageBox::critical(this,
                              tr("Фатальная ошибка"),
                              tr("Во время анализа произошла неизвестная ошибка."));
    }
}

void MainWindow::runTests()
{
    std::vector<TestCaseResult> results = RunAllTests();

    int passed = 0;
    for (const auto &r : results) {
        if (r.passed) ++passed;
    }

    QString html;
    html += "<h3>Результаты тестов</h3>";

    html += QString("<p>Всего тестов: <b>%1</b>, успешно: "
                    "<span style='color:green;'><b>%2</b></span>, "
                    "провалено: <span style='color:red;'><b>%3</b></span></p>")
                .arg(results.size())
                .arg(passed)
                .arg(results.size() - passed);

    html += "<ul>";
    for (const auto &r : results) {
        QString name = QString::fromUtf8(r.name.c_str()).toHtmlEscaped();
        QString details = QString::fromUtf8(r.details.c_str()).toHtmlEscaped();

        if (r.passed) {
            html += QString("<li><span style='color:green;'>✔ %1</span>: %2</li>")
                        .arg(name, details);
        } else {
            html += QString("<li><span style='color:red;'>✘ %1</span>: %2</li>")
                        .arg(name, details);
        }
    }
    html += "</ul>";

    outputText_->setHtml(html);
}
