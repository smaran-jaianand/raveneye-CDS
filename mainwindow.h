#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include <QString>

// -------------------------
// Data types — must be complete for moc/autogen
// -------------------------
struct Connection {
    int targetId;
    QString type;
    int weight;
};

struct Criminal {
    int id{};
    QString name;
    QString tag;
    int age{};
    QString location;
    std::vector<Connection> connections;
};

// -------------------------
// MainWindow declaration
// -------------------------
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onAddPerson();
    void onAddConnection();
    void onSave();
    void onLoad();
    void onSelectionChanged(int row);

private:
    QString nameById(int id) const;
    void refreshList();

    QListWidget *listWidget = nullptr;
    QPushButton *btnAdd = nullptr;
    QPushButton *btnAddConnection = nullptr;
    QPushButton *btnSave = nullptr;
    QPushButton *btnLoad = nullptr;
    QLabel *detailLabel = nullptr;

    std::vector<Criminal> records;
    int nextId = 1;
};
