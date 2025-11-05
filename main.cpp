#include "mainwindow.h"
#include <QApplication>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>

#include <vector>
#include <unordered_map>

// ----------------------------------------------------------------------
// AddPersonDialog
// ----------------------------------------------------------------------
class AddPersonDialog : public QDialog {
public:
    AddPersonDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Add Person");
        auto *form = new QFormLayout(this);
        nameEdit = new QLineEdit(this);
        tagEdit = new QLineEdit(this);
        ageSpin = new QSpinBox(this);
        ageSpin->setRange(0,150);
        locationEdit = new QLineEdit(this);
        form->addRow("Name:", nameEdit);
        form->addRow("Tag:", tagEdit);
        form->addRow("Age:", ageSpin);
        form->addRow("Location:", locationEdit);

        auto *btnBox = new QHBoxLayout();
        auto *ok = new QPushButton("OK", this);
        auto *cancel = new QPushButton("Cancel", this);
        btnBox->addWidget(ok);
        btnBox->addWidget(cancel);
        form->addRow(btnBox);

        connect(ok, &QPushButton::clicked, this, &AddPersonDialog::accept);
        connect(cancel, &QPushButton::clicked, this, &AddPersonDialog::reject);
    }

    QString name() const { return nameEdit->text(); }
    QString tag() const { return tagEdit->text(); }
    int age() const { return ageSpin->value(); }
    QString location() const { return locationEdit->text(); }

private:
    QLineEdit *nameEdit;
    QLineEdit *tagEdit;
    QSpinBox *ageSpin;
    QLineEdit *locationEdit;
};

// ----------------------------------------------------------------------
// AddConnectionDialog
// ----------------------------------------------------------------------
class AddConnectionDialog : public QDialog {
public:
    AddConnectionDialog(const QStringList &names, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Add Connection");
        auto *form = new QFormLayout(this);
        comboA = new QComboBox(this);
        comboB = new QComboBox(this);
        comboA->addItems(names);
        comboB->addItems(names);
        typeEdit = new QLineEdit(this);
        weightSpin = new QSpinBox(this);
        weightSpin->setRange(0, 100);
        form->addRow("Person A:", comboA);
        form->addRow("Person B:", comboB);
        form->addRow("Type:", typeEdit);
        form->addRow("Weight:", weightSpin);

        auto *btnBox = new QHBoxLayout();
        auto *ok = new QPushButton("OK", this);
        auto *cancel = new QPushButton("Cancel", this);
        btnBox->addWidget(ok);
        btnBox->addWidget(cancel);
        form->addRow(btnBox);

        connect(ok, &QPushButton::clicked, this, &AddConnectionDialog::onOk);
        connect(cancel, &QPushButton::clicked, this, &AddConnectionDialog::reject);
    }

    int indexA() const { return comboA->currentIndex(); }
    int indexB() const { return comboB->currentIndex(); }
    QString type() const { return typeEdit->text(); }
    int weight() const { return weightSpin->value(); }

private:
    void onOk() {
        if (comboA->currentIndex() == comboB->currentIndex()) {
            QMessageBox::warning(this, "Invalid", "Cannot connect a person to themselves.");
            return;
        }
        if (typeEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Invalid", "Enter connection type.");
            return;
        }
        accept();
    }

    QComboBox *comboA;
    QComboBox *comboB;
    QLineEdit *typeEdit;
    QSpinBox *weightSpin;
};

// ----------------------------------------------------------------------
// MainWindow Implementation (uses types from mainwindow.h)
// ----------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("RavenEye - GUI");
    auto *central = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(central);

    listWidget = new QListWidget(this);
    mainLayout->addWidget(listWidget, 2);

    auto *right = new QWidget(this);
    auto *rv = new QVBoxLayout(right);
    btnAdd = new QPushButton("Add Person", this);
    btnAddConnection = new QPushButton("Add Connection", this);
    btnSave = new QPushButton("Save", this);
    btnLoad = new QPushButton("Load", this);
    detailLabel = new QLabel("Select a person to see details", this);
    detailLabel->setWordWrap(true);

    rv->addWidget(btnAdd);
    rv->addWidget(btnAddConnection);
    rv->addWidget(btnSave);
    rv->addWidget(btnLoad);
    rv->addWidget(detailLabel);
    rv->addStretch();

    mainLayout->addWidget(right, 3);
    setCentralWidget(central);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddPerson);
    connect(btnAddConnection, &QPushButton::clicked, this, &MainWindow::onAddConnection);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoad);
    connect(listWidget, &QListWidget::currentRowChanged, this, &MainWindow::onSelectionChanged);
}

void MainWindow::onAddPerson() {
    AddPersonDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Criminal c;
        c.id = nextId++;
        c.name = dlg.name();
        c.tag = dlg.tag();
        c.age = dlg.age();
        c.location = dlg.location();
        records.push_back(c);
        refreshList();
    }
}

void MainWindow::onAddConnection() {
    if (records.size() < 2) {
        QMessageBox::warning(this, "Need 2+ people", "Add at least two people first.");
        return;
    }
    QStringList names;
    for (auto &r : records) names << r.name;
    AddConnectionDialog dlg(names, this);
    if (dlg.exec() == QDialog::Accepted) {
        int a = dlg.indexA();
        int b = dlg.indexB();
        Connection c1{records[b].id, dlg.type(), dlg.weight()};
        Connection c2{records[a].id, dlg.type(), dlg.weight()};
        records[a].connections.push_back(c1);
        records[b].connections.push_back(c2);
        refreshList();
    }
}

void MainWindow::onSave() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Database", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) return;
    QJsonArray arr;
    for (auto &r : records) {
        QJsonObject obj;
        obj["id"] = r.id;
        obj["name"] = r.name;
        obj["tag"] = r.tag;
        obj["age"] = r.age;
        obj["location"] = r.location;
        QJsonArray conArr;
        for (auto &c : r.connections) {
            QJsonObject co;
            co["targetId"] = c.targetId;
            co["type"] = c.type;
            co["weight"] = c.weight;
            conArr.append(co);
        }
        obj["connections"] = conArr;
        arr.append(obj);
    }
    QJsonDocument doc(arr);
    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly)) { QMessageBox::warning(this, "Error", "Can't open file for writing"); return; }
    f.write(doc.toJson());
    f.close();
}

void MainWindow::onLoad() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Database", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) return;
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) { QMessageBox::warning(this, "Error", "Can't open file"); return; }
    QByteArray data = f.readAll();
    f.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) { QMessageBox::warning(this, "Error", "Invalid file format"); return; }
    records.clear();
    nextId = 1;
    QJsonArray arr = doc.array();
    for (auto v : arr) {
        QJsonObject obj = v.toObject();
        Criminal r;
        r.id = obj["id"].toInt();
        r.name = obj["name"].toString();
        r.tag = obj["tag"].toString();
        r.age = obj["age"].toInt();
        r.location = obj["location"].toString();
        nextId = std::max(nextId, r.id + 1);
        QJsonArray conArr = obj["connections"].toArray();
        for (auto cval : conArr) {
            QJsonObject co = cval.toObject();
            Connection c{co["targetId"].toInt(), co["type"].toString(), co["weight"].toInt()};
            r.connections.push_back(c);
        }
        records.push_back(r);
    }
    refreshList();
}

void MainWindow::onSelectionChanged(int row) {
    if (row < 0 || row >= (int)records.size()) {
        detailLabel->setText("Select a person to see details");
        return;
    }
    auto &r = records[row];
    QString details = QString("ID: %1\nName: %2\nTag: %3\nAge: %4\nLocation: %5\nConnections:\n")
                      .arg(r.id).arg(r.name).arg(r.tag).arg(r.age).arg(r.location);
    for (auto &c : r.connections) {
        QString targetName = nameById(c.targetId);
        details += QString("  - %1 (Type: %2, W:%3)\n").arg(targetName).arg(c.type).arg(c.weight);
    }
    detailLabel->setText(details);
}

QString MainWindow::nameById(int id) const {
    for (auto &r : records) if (r.id == id) return r.name;
    return "<unknown>";
}

void MainWindow::refreshList() {
    listWidget->clear();
    for (auto &r : records)
        listWidget->addItem(QString("%1: %2 (%3)").arg(r.id).arg(r.name).arg(r.tag));
}

// ----------------------------------------------------------------------
// main()
// ----------------------------------------------------------------------
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.resize(800, 480);
    w.show();
    return app.exec();
}
