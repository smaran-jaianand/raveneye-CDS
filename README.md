# 🕵️‍♂️ RavenEye — Criminal Network Intelligence System

> A ruthless intelligence visualization tool built with **C++ (Qt6)** and **D3.js** to map and analyze criminal or entity relationships.  
> Designed with **graph-based DSA** and rendered with **3D browser visualization**.

---

## 🚀 Overview

**RavenEye** lets you:
- Create and manage **entities** (e.g., suspects, organizations, nodes).
- Define **connections** between them (e.g., relationships, transactions, affiliations).
- Store and export all data into **JSON graph format**.
- Visualize the network in your browser using **D3.js 3D force graphs**.

It’s a hybrid of **Qt GUI + Browser visualization**, engineered for clarity and precision.

---

## 🧠 Core Concept — Graph DSA

RavenEye internally implements a **Graph Data Structure (Adjacency List)** using `std::vector`.

```
Criminals → Nodes
Connections → Edges
records (std::vector<Criminal>) → Adjacency List Graph
```

Each `Criminal` node stores its `Connection` list, allowing quick traversal, expansion, and visualization.

---

## 🧩 Tech Stack

| Component | Technology |
|------------|-------------|
| **Language** | C++20 |
| **Framework** | Qt6 (Widgets, Core, JSON) |
| **Build System** | CMake |
| **Visualization** | D3.js (Force-Directed Graph) |
| **Data Format** | JSON |
| **OS Tested** | Windows 10/11 |

---

## 🧰 Data Structures Used

| DSA | Purpose |
|-----|----------|
| `std::vector` | Stores all records and their connections |
| **Adjacency List** | Represents the relationship graph |
| **JSON Tree** | Used for saving and exporting structured data |
| *(Future)* BFS / DFS | For pathfinding and graph analysis |

---

## 📁 Project Structure

```
RavenEYE-DSA-Project/
│
├── main.cpp                     # GUI and backend logic
├── mainwindow.h                 # Declarations & structures
├── CMakeLists.txt               # Build configuration
│
├── raveneye-graph/              # Visualization Layer
│   ├── index.html               # D3.js graph view
│   ├── script.js                # Force-directed graph logic
│   └── data.json                # Auto-generated graph data
│
└── ravendb/                     # Stored database (saved JSONs)
```

---

## ⚙️ Installation (Windows)

### 1️⃣ Install Qt
Download and install **Qt 6.10.0** (or latest) using Qt Online Installer.  
Select:
```
Qt → Desktop → MinGW 64-bit
```

### 2️⃣ Install an IDE
Use **CLion**, **Qt Creator**, or **VS Code with CMake Tools**.

### 3️⃣ Clone & Build

```bash
git clone https://github.com/<your-username>/RavenEYE-DSA-Project.git
cd RavenEYE-DSA-Project
cmake -S . -B build
cmake --build build
```

Then **Run ▶️** the project inside CLion or Qt Creator.

---

## 🌐 Browser Visualization

After running the app:
1. Add people and their connections.  
2. Click **Save** inside the app.  
3. It generates:  
   ```
   raveneye-graph/data.json
   ```
4. Open `raveneye-graph/index.html` in your browser.  
5. View your 3D network visualization.

---

## 📦 Example Data Output

```json
{
  "nodes": [
    {"id": 1, "name": "John", "group": "Gang Leader"},
    {"id": 2, "name": "Mike", "group": "Supplier"}
  ],
  "links": [
    {"source": 1, "target": 2, "weight": 5}
  ]
}
```

---

## 🧭 CMake Configuration

```cmake
cmake_minimum_required(VERSION 3.16)
project(RavenEYE_DSA_Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
```

---

## 🧱 Architecture Summary

| Layer | Description |
|--------|-------------|
| **Frontend (Qt GUI)** | UI for managing entities & relationships |
| **Backend (C++ Core)** | Graph-based DSA engine |
| **Storage (JSON)** | Persistent graph data export |
| **Visualization (D3.js)** | Browser-based 3D force graph |

---

## 🔮 Future Enhancements

| Feature | Description | DSA |
|----------|--------------|------|
| Graph Traversal | BFS / DFS to find links | Queue / Stack |
| Shortest Path | Dijkstra’s Algorithm | Priority Queue |
| Influence Ranking | Central node detection | Degree / Heap |
| Group Detection | Cluster segmentation | Disjoint Set (Union-Find) |

---

## 💬 Credits

Developed by **Smaran** and **Sujay** 

---

## 🕹 Quick Run Summary

| Step | Action |
|------|---------|
| 1️⃣ | Run RavenEYE Qt app |
| 2️⃣ | Add people + connections |
| 3️⃣ | Click **Save** |
| 4️⃣ | Auto-updates `raveneye-graph/data.json` |
| 5️⃣ | Open `index.html` in browser |
| ✅ | View your dynamic 3D network |
