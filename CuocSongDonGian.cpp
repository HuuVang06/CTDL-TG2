#include <iostream>
#include <ctime>
#include <cmath>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>
#include <algorithm> // BẮT BUỘC CHO std::sort
using namespace std;

// Hằng số cho độ trễ in ra
const auto DELAY_TIME = chrono::milliseconds(500);
const int NUM_BUCKETS = 10;

// =========================================================
//                  KHAI BÁO CẤU TRÚC DỮ LIỆU
// =========================================================

struct SinhVien {
    string hoTen;
    long mssv;
};

// Cấu trúc cho thuật toán Đồ thị
struct Edge {
    int src, dest;
    int weight;
};

struct Graph {
    int V, E;
    Edge* edges;
};

// Cấu trúc cho Disjoint Set Union (Dành cho Kruskal)
struct Subset {
    int parent;
    int rank;
};

// =========================================================
//                  CÁC HÀM HỖ TRỢ CHUNG
// =========================================================

char myToUpper(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }
    return c;
}

void clearScreen() {
#ifdef _WIN32
    cout << "\033[2J\033[1;1H";
#else
    cout << "\033[2J\033[1;1H";
#endif
}

string extractLastName(const string& hoTen) {
    size_t lastSpace = hoTen.find_last_of(' ');
    if (lastSpace != string::npos) {
        return hoTen.substr(lastSpace + 1);
    }
    return hoTen;
}

// =========================================================
//                  CÁC HÀM SO SÁNH
// =========================================================

bool compareGivenName(const SinhVien& a, const SinhVien& b) {
    string name_a = extractLastName(a.hoTen);
    string name_b = extractLastName(b.hoTen);
    if (name_a != name_b) {
        return name_a < name_b;
    }
    return a.hoTen < b.hoTen;
}

bool compareEdges(const Edge& a, const Edge& b) {
    return a.weight < b.weight;
}

bool compareFamilyName(const SinhVien& a, const SinhVien& b) {
    return a.hoTen < b.hoTen;
}

// =========================================================
//          HÀM HỖ TRỢ DISJOINT SET (CHO KRUSKAL)
//          CHỈ ĐỊNH NGHĨA MỘT LẦN TẠI ĐÂY
// =========================================================

// Tim tap hop cha cua mot phan tu
int find(Subset subsets[], int i) {
    if (subsets[i].parent != i)
        subsets[i].parent = find(subsets, subsets[i].parent);
    return subsets[i].parent;
}

// Ket hop hai tap hop
void Union(Subset subsets[], int x, int y) {
    int rootX = find(subsets, x);
    int rootY = find(subsets, y);

    if (subsets[rootX].rank < subsets[rootY].rank) {
        subsets[rootX].parent = rootY;
    }
    else if (subsets[rootX].rank > subsets[rootY].rank) {
        subsets[rootY].parent = rootX;
    }
    else {
        subsets[rootY].parent = rootX;
        subsets[rootX].rank++;
    }
}

// =========================================================
//                  CÁC HÀM XỬ LÝ FILE & MENU
// =========================================================

SinhVien* readStudentsFromFile(const string& filename, int& n) {
    ifstream file(filename);
    string line;
    n = 0;

    if (!file.is_open()) {
        cerr << " LOI: Khong the mo file " << filename << endl;
        return nullptr;
    }

    int count = 0;
    while (getline(file, line)) {
        if (!line.empty() && line.find_first_not_of(" \t\n\r") != string::npos) {
            count++;
        }
    }

    if (count == 0) {
        return nullptr;
    }

    SinhVien* students = new SinhVien[count];
    n = count;

    file.clear();
    file.seekg(0, ios::beg);

    int index = 0;
    while (getline(file, line) && index < n) {
        if (line.empty() || line.find_first_not_of(" \t\n\r") == string::npos) {
            continue;
        }

        stringstream ss(line);
        string hoTenStr, mssvStr;

        if (getline(ss, hoTenStr, ',')) {

            size_t first = hoTenStr.find_first_not_of(" \t\n\r");
            if (string::npos == first) {
                hoTenStr = "";
            }
            else {
                size_t last = hoTenStr.find_last_not_of(" \t\n\r");
                hoTenStr = hoTenStr.substr(first, (last - first + 1));
            }

            if (getline(ss, mssvStr)) {
                try {
                    students[index].hoTen = hoTenStr;
                    students[index].mssv = stol(mssvStr);
                    index++;
                }
                catch (const std::exception& e) {
                    cerr << "Loi chuyen doi du lieu MSSV tren dong: " << line << endl;
                }
            }
        }
    }
    if (index < n) {
        n = index;
    }
    return students;
}

Graph* readGraphFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << " LOI: Khong the mo file " << filename << " de doc du lieu do thi." << endl;
        return nullptr;
    }

    int V, E;
    if (!(file >> V >> E)) {
        cerr << " LOI: Khong the doc so luong dinh (V) va canh (E)." << endl;
        return nullptr;
    }

    Graph* graph = new Graph;
    graph->V = V;
    graph->E = E;
    graph->edges = new Edge[E];

    cout << "Da doc do thi: V = " << V << ", E = " << E << endl;

    for (int i = 0; i < E; i++) {
        if (!(file >> graph->edges[i].src >> graph->edges[i].dest >> graph->edges[i].weight)) {
            cerr << " LOI: Khong the doc du lieu canh thu " << i + 1 << endl;
            delete[] graph->edges;
            delete graph;
            return nullptr;
        }
    }
    return graph;
}

int menu()
{
    int chon;
    cout << "==============MENU============== \n";
    cout << "--- SAP XEP (Chon thuat toan) ---\n";
    cout << "1. Shell sort (Chon MSSV/Ho Ten).\n";
    cout << "2. Counting sort (Chon MSSV/Ho Ten).\n";
    cout << "3. Radix sort (Chon MSSV/Ho Ten).\n";
    cout << "4. Bucket sort (Chon MSSV/Ho Ten).\n";
    cout << "--------------------------------\n";
    cout << "--- SAP XEP NHANH (Bang Shell Sort) ---\n";
    cout << "6. Sap xep theo MSSV.\n";
    cout << "7. Sap xep theo Ho Ten.\n";
    cout << "--------------------------------\n";
    cout << "--- THUAT TOAN DO THI ---\n";
    cout << "8. Kruskal (Cay bao trum nho nhat MST).\n";
    cout << "9. Prim (Cay bao trum nho nhat MST).\n";
    cout << "--------------------------------\n";
    cout << "5. Dung lai. \n";
    cout << "Nhap vao lua chon (1-9, 5 de dung):";

    if (!(cin >> chon) || (chon < 1 || chon > 9)) {
        if (cin.fail()) {
            cout << "\nLoi nhap lieu! Vui long nhap so nguyen.\n";
        }
        else {
            cout << "\nLua chon khong hop le! Vui long nhap tu 1-9.\n";
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return 0;
    }
    return chon;
}

// =========================================================
//                  CÁC HÀM VẼ ĐỒ THỊ & IN ẤN
// =========================================================

void veDothitheoMSSV(const SinhVien* a, int n, int soCot = 10) {
    if (n <= 0) {
        cout << "Khong co so luong de ve do thi!\n";
        return;
    }
    // 



    long Min = a[0].mssv;
    long Max = a[0].mssv;

    for (int i = 1; i < n; i++) {
        if (a[i].mssv < Min)
            Min = a[i].mssv;
        if (a[i].mssv > Max)
            Max = a[i].mssv;
    }

    long khoang = (Max - Min + 1) / soCot + 1;
    if (khoang == 0)
    {
        khoang = 1;
    }

    //Mang dem
    int* dem = new int[soCot];
    for (int i = 0; i < soCot; i++) {
        dem[i] = 0;
    }

    //dem xem MSSV thuoc cot nao
    for (int i = 0; i < n; i++) {
        int idx = (a[i].mssv - Min) / khoang;
        if (idx >= soCot)
            idx = soCot - 1;
        if (idx < 0)
            idx = 0;
        dem[idx]++;
    }

    // Vẽ
    cout << "\n===== ĐỒ THỊ PHÂN BỐ MSSV =====\n";

    for (int i = 0; i < soCot; i++) {
        long left = Min + i * khoang;
        long right = left + khoang - 1;

        cout << "[" << left << " - " << right << "] : ";
        for (int j = 0; j < dem[i]; j++) cout << "*";
        cout << " (" << dem[i] << ")\n";
    }

    cout << "================================\n";

    delete[] dem;
}

void veDoThiDangThanh(const SinhVien* a, int n) {
    if (n <= 0) return;
    cout << "\n===== ĐỒ THỊ MINH HỌA GIÁ TRỊ MSSV (BAR CHART) =====\n";
    // 



    long Min = a[0].mssv;
    long Max = a[0].mssv;
    for (int i = 1; i < n; i++) {
        if (a[i].mssv < Min) Min = a[i].mssv;
        if (a[i].mssv > Max) Max = a[i].mssv;
    }

    const int MAX_BAR_LENGTH = 50;
    int limit = (n > 30) ? 30 : n;

    for (int i = 0; i < limit; ++i) {
        int len = 0;
        if (Max > 0) {
            len = (int)((double)a[i].mssv / Max * MAX_BAR_LENGTH);
        }

        cout << setw(3) << i + 1 << ". " << setw(8) << a[i].mssv << " |";
        cout << "\033[1;36m"; // Màu Cyan
        for (int k = 0; k < len; ++k) cout << "=";
        cout << "\033[0m" << endl; // Reset màu và xuống dòng
    }

    if (n > 30) {
        cout << "... (Con " << (n - 30) << " sinh vien nua) ...\n";
    }
    cout << "====================================================\n";
}

void printArr(const SinhVien* a, int n)
{
    cout << "  --------------------------------------------------\n";
    for (int i = 0; i < n; ++i)
    {
        cout << "  | Ho Ten: " << a[i].hoTen;
        size_t padding = 20; // Tang padding
        if (a[i].hoTen.length() < padding) {
            for (size_t k = a[i].hoTen.length(); k < padding; ++k) cout << " ";
        }

        cout << "| MSSV: " << a[i].mssv << " |" << endl;
    }
    cout << "  --------------------------------------------------\n";
}

void delayPrint(const SinhVien* a, int n) {
    printArr(a, n);
    this_thread::sleep_for(DELAY_TIME);
}

// =========================================================
//                  THUẬT TOÁN ĐỒ THỊ (MST)
// =========================================================

void kruskalMST(Graph* graph) {
    if (graph == nullptr) return;

    int V = graph->V;
    Edge* result = new Edge[V];
    int e = 0; // Chi so cho mang ket qua (result[])
    int i = 0; // Chi so cho mang canh da sap xep (graph->edges[])

    cout << "\n--- THỰC HIỆN THUẬT TOÁN KRUSKAL ---\n";
    // 

    // 1. Sắp xếp các cạnh theo trọng số tăng dần
    cout << "1. Sap xep cac canh (Edge) theo trong so: ";
    // Sử dụng std::sort (cần #include <algorithm>)
    sort(graph->edges, graph->edges + graph->E, compareEdges);
    cout << "Hoan thanh.\n";

    // 2. Khởi tạo Disjoint Set
    Subset* subsets = new Subset[V];
    for (int v = 0; v < V; v++) {
        subsets[v].parent = v;
        subsets[v].rank = 0;
    }

    // 3. Lặp qua các cạnh đã sắp xếp
    cout << "2. Bat dau chon canh de xay dung MST:\n";
    while (e < V - 1 && i < graph->E) {
        Edge next_edge = graph->edges[i++];

        int x = find(subsets, next_edge.src);
        int y = find(subsets, next_edge.dest);

        // Kiểm tra xem có tạo chu trình không
        if (x != y) {
            result[e++] = next_edge;
            Union(subsets, x, y);
            cout << "   -> Chon canh (" << next_edge.src << " - " << next_edge.dest << ") | Trong so: " << next_edge.weight << endl;
        }
        else {
            cout << "   -> Bo qua canh (" << next_edge.src << " - " << next_edge.dest << ") vi tao chu trinh." << endl;
        }
        this_thread::sleep_for(DELAY_TIME);
    }

    // 4. In kết quả
    cout << "\n--- KET QUA KRUSKAL MST ---\n";
    int minimumCost = 0;
    for (i = 0; i < e; ++i) {
        cout << "(" << result[i].src << " - " << result[i].dest << ") == " << result[i].weight << endl;
        minimumCost += result[i].weight;
    }
    cout << "Tong trong so nho nhat: " << minimumCost << endl;
    cout << "---------------------------------\n";

    delete[] result;
    delete[] subsets;
}

// Hàm tìm đỉnh có key (trọng số) nhỏ nhất trong các đỉnh chưa được chọn
int minKey(int V, const int key[], const bool mstSet[]) {
    int min = numeric_limits<int>::max(), min_index;

    for (int v = 0; v < V; v++)
        if (mstSet[v] == false && key[v] < min)
            min = key[v], min_index = v;

    return min_index;
}

// AdjMatrix: Ma trận kề (AdjMatrix[i][j] là trọng số cạnh i-j)
void primMST(int V, int** AdjMatrix) {
    if (V <= 0) return;

    int* parent = new int[V];
    int* key = new int[V];
    bool* mstSet = new bool[V];

    cout << "\n--- THỰC HIỆN THUẬT TOÁN PRIM ---\n";
    // 

    // Khởi tạo
    for (int i = 0; i < V; i++) {
        key[i] = numeric_limits<int>::max();
        mstSet[i] = false;
    }

    // Bắt đầu từ đỉnh 0
    key[0] = 0;
    parent[0] = -1;

    // Lặp để tìm V-1 cạnh
    for (int count = 0; count < V - 1; count++) {
        // 1. Chọn đỉnh có key nhỏ nhất
        int u = minKey(V, key, mstSet);
        mstSet[u] = true;

        cout << "-> Chon dinh " << u << " vao MST. Key: " << key[u] << endl;
        this_thread::sleep_for(DELAY_TIME);

        // 2. Cập nhật key và parent của các đỉnh kề
        for (int v = 0; v < V; v++) {
            // AdjMatrix[u][v] > 0: Có cạnh (u, v)
            // mstSet[v] == false: v chưa nằm trong MST
            // AdjMatrix[u][v] < key[v]: Cạnh mới tìm được (u, v) tốt hơn cạnh hiện tại
            if (AdjMatrix[u][v] && mstSet[v] == false && AdjMatrix[u][v] < key[v]) {
                parent[v] = u;
                key[v] = AdjMatrix[u][v];
                cout << "   Cap nhat: key[" << v << "] = " << key[v] << " (parent: " << u << ")" << endl;
            }
        }
    }

    // 3. In kết quả
    cout << "\n--- KẾT QUẢ PRIM MST ---\n";
    int minimumCost = 0;
    for (int i = 1; i < V; i++) {
        cout << "(" << parent[i] << " - " << i << ") == " << AdjMatrix[i][parent[i]] << endl;
        minimumCost += AdjMatrix[i][parent[i]];
    }
    cout << "Tong trong so nho nhat: " << minimumCost << endl;
    cout << "--------------------------------\n";

    delete[] parent;
    delete[] key;
    delete[] mstSet;
}

// =========================================================
//                  CÁC THUẬT TOÁN SẮP XẾP KHÁC
// =========================================================
// (Giữ nguyên các hàm ShellSort, CountingSort, RadixSort, BucketSort)
// ...

void shellSort(SinhVien* a, int n, bool sortByMSSV, int sortByNameChoice = 1) {
    if (n <= 1) return;

    string sort_key_desc = sortByMSSV ? "MSSV" : (sortByNameChoice == 2 ? "TÊN" : "HỌ");

    cout << "--- THUC HIEN SHELL SORT (Sap xep theo " << sort_key_desc << ") ---\n";

    for (int gap = n / 2; gap > 0; gap /= 2) {
        cout << "\n--- Bat dau pass voi gap = " << gap << " ---" << endl;
        for (int i = gap; i < n; i++) {
            SinhVien temp = a[i];
            int j = i;

            if (sortByMSSV) {
                while (j >= gap && a[j - gap].mssv > temp.mssv) {
                    a[j] = a[j - gap];
                    j -= gap;
                }
            }
            else {
                bool (*compareFunc)(const SinhVien&, const SinhVien&);
                if (sortByNameChoice == 2) {
                    compareFunc = compareGivenName;
                }
                else {
                    compareFunc = compareFamilyName;
                }

                while (j >= gap && compareFunc(temp, a[j - gap])) {
                    a[j] = a[j - gap];
                    j -= gap;
                }
            }

            if (j != i) {
                a[j] = temp;
                cout << "   Chen SV co " << (sortByMSSV ? "MSSV " + to_string(temp.mssv) : "Ten '" + temp.hoTen + "'") << " vao vi tri " << j << ": ";
                delayPrint(a, n);
            }
        }
        cout << "==> Ket thuc pass gap = " << gap << ". Mang hien tai: ";
        delayPrint(a, n);
    }
}


long getMax(const SinhVien* a, int n)
{
    if (n == 0) return 0;
    long maxVal = a[0].mssv;
    for (int i = 1; i < n; i++)
    {
        if (a[i].mssv > maxVal)
        {
            maxVal = a[i].mssv;
        }
    }
    return maxVal;
}

int getCharKey(const SinhVien& sv, int sortByNameChoice) {
    string key;
    if (sortByNameChoice == 2) { // Sap xep theo TEN
        key = extractLastName(sv.hoTen);
    }
    else { // Sap xep theo HO
        key = sv.hoTen;
    }

    if (key.empty()) return 0; // Neu ten rong
    int c = myToUpper(key[0]) - 'A'; // Dung ham tu viet
    if (c < 0 || c > 25) return 0; // Neu la ky tu dac biet, cho vao bucket 'A'
    return c;
}


void countingSort(SinhVien* a, int n, bool sortByMSSV, int sortByNameChoice = 1) {
    if (n == 0) return;

    if (!sortByMSSV) {
        string sort_key_desc = (sortByNameChoice == 2 ? "TÊN" : "HỌ");
        cout << "--- THUC HIEN COUNTING SORT (Sap xep theo KY TU DAU cua " << sort_key_desc << ") ---\n";
        cout << "[!] LUU Y: Thuat toan nay chi sap xep theo chu cai dau (A-Z), khong sap xep ben trong cung mot chu cai." << endl;

        const int RANGE = 26; // A-Z
        int* count = new int[RANGE]();
        SinhVien* output = new SinhVien[n];

        cout << "1. Dem so lan xuat hien (A=0, B=1, ...):\n";
        for (int i = 0; i < n; i++) {
            count[getCharKey(a[i], sortByNameChoice)]++;
        }

        cout << "2. Tinh tong tich luy:\n";
        for (int i = 1; i < RANGE; i++) {
            count[i] += count[i - 1];
        }

        cout << "3. Xay dung mang Output (Di nguoc de dam bao Tinh ON DINH):\n";
        for (int i = n - 1; i >= 0; i--) {
            int key = getCharKey(a[i], sortByNameChoice);
            int final_index = count[key] - 1;

            output[final_index] = a[i];
            count[key]--;
        }

        for (int i = 0; i < n; i++) {
            a[i] = output[i];
        }

        delete[] output;
        delete[] count;

        cout << "   Ket qua Counting Sort (theo ky tu dau): " << endl;
        delayPrint(a, n);

    }
    else {
        cout << "--- THUC HIEN COUNTING SORT (Sap xep MSSV) ---\n";

        if (n == 0) return;
        long maxVal = getMax(a, n);

        long minVal = a[0].mssv;
        for (int i = 1; i < n; i++) {
            if (a[i].mssv < minVal) minVal = a[i].mssv;
        }
        if (minVal < 0) {
            cout << "\n[!] LOI: COUNTING SORT khong ho tro MSSV am." << endl;
            return;
        }

        long range = maxVal - minVal + 1;
        if (range > 1000001) { // Dat gioi han an toan
            cout << "\n[!] CANH BAO: Pham vi MSSV (" << range << ") qua lon, Counting Sort khong hieu qua." << endl; return;
        }

        int* count = new int[range]();
        SinhVien* output = new SinhVien[n];

        cout << "1. Dem so lan xuat hien (offset = " << minVal << "):\n";
        for (int i = 0; i < n; i++) { count[a[i].mssv - minVal]++; }

        cout << "2. Tinh tong tich luy:\n";
        for (long i = 1; i < range; i++) { count[i] += count[i - 1]; }

        cout << "3. Xay dung mang Output (Di nguoc de dam bao Tinh ON DINH):\n";
        for (int i = n - 1; i >= 0; i--) {
            long val_offset = a[i].mssv - minVal;
            int final_index = count[val_offset] - 1;
            output[final_index] = a[i];
            count[val_offset]--;
        }

        for (int i = 0; i < n; i++) {
            a[i] = output[i];
        }

        delete[] output;
        delete[] count;

        cout << "   Ket qua Counting Sort (theo MSSV): " << endl;
        delayPrint(a, n);
    }
}


void countingSortByDigit(SinhVien* a, int n, long exp) {
    if (n == 0) return;

    SinhVien* output = new SinhVien[n];
    int count[10] = { 0 };

    for (int i = 0; i < n; i++) { count[(a[i].mssv / exp) % 10]++; }
    for (int i = 1; i < 10; i++) { count[i] += count[i - 1]; }

    for (int i = n - 1; i >= 0; i--) {
        int digit = (a[i].mssv / exp) % 10;
        int final_index = count[digit] - 1;
        output[final_index] = a[i];
        count[digit]--;
    }

    for (int i = 0; i < n; i++) {
        a[i] = output[i];
    }

    delete[] output;
}

void countingSortByLastNameCharLSD(SinhVien* a, int n, int d_from_right) {
    SinhVien* output = new SinhVien[n];
    int count[257] = { 0 }; // 256 ky tu ASCII + 1 (ky tu 0) cho padding

    for (int i = 0; i < n; i++) {
        string lastName = extractLastName(a[i].hoTen);
        int charIndex = lastName.length() - 1 - d_from_right;
        int c = (charIndex >= 0) ? (int)(unsigned char)lastName[charIndex] + 1 : 0;
        count[c]++;
    }

    for (int i = 1; i < 257; i++) {
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; i--) {
        string lastName = extractLastName(a[i].hoTen);
        int charIndex = lastName.length() - 1 - d_from_right;
        int c = (charIndex >= 0) ? (int)(unsigned char)lastName[charIndex] + 1 : 0;

        output[count[c] - 1] = a[i];
        count[c]--;
    }

    for (int i = 0; i < n; i++) {
        a[i] = output[i];
    }

    delete[] output;
}

void countingSortByCharLSD(SinhVien* a, int n, int d_from_right) {
    SinhVien* output = new SinhVien[n];
    int count[257] = { 0 };

    for (int i = 0; i < n; i++) {
        int charIndex = a[i].hoTen.length() - 1 - d_from_right;
        int c = (charIndex >= 0) ? (int)(unsigned char)a[i].hoTen[charIndex] + 1 : 0;
        count[c]++;
    }

    for (int i = 1; i < 257; i++) {
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; i--) {
        int charIndex = a[i].hoTen.length() - 1 - d_from_right;
        int c = (charIndex >= 0) ? (int)(unsigned char)a[i].hoTen[charIndex] + 1 : 0;

        output[count[c] - 1] = a[i];
        count[c]--;
    }

    for (int i = 0; i < n; i++) {
        a[i] = output[i];
    }

    delete[] output;
}


void radixSort(SinhVien* a, int n, bool sortByMSSV, int sortByNameChoice = 1) {
    if (n == 0) return;

    if (sortByMSSV) {
        cout << "--- THUC HIEN RADIX SORT (Sap xep MSSV) ---\n";
        long maxVal = getMax(a, n);
        for (long exp = 1; maxVal / exp > 0; exp *= 10) {
            cout << "-> Sap xep theo vi tri co trong so " << exp << endl;
            countingSortByDigit(a, n, exp);
            cout << "   Mang sau khi sort theo vi tri " << exp << ": ";
            delayPrint(a, n);
        }
    }
    else {

        if (sortByNameChoice == 2) {
            cout << "--- THUC HIEN RADIX SORT (Sap xep Ho Ten - Theo TÊN) ---\n";

            size_t maxLen = 0;
            for (int i = 0; i < n; i++) {
                size_t len = extractLastName(a[i].hoTen).length();
                if (len > maxLen) maxLen = len;
            }
            if (maxLen == 0) return;

            for (size_t d = 0; d < maxLen; d++) {
                cout << "-> Sap xep theo ky tu TEN vi tri " << d << " (tu phai sang):" << endl;
                countingSortByLastNameCharLSD(a, n, d); // Goi ham moi
                cout << "  Mang sau khi sort: ";
                delayPrint(a, n);
            }
        }
        else {
            cout << "--- THUC HIEN RADIX SORT (Sap xep Ho Ten - Theo HỌ) ---\n";

            size_t maxLen = 0;
            for (int i = 0; i < n; i++) {
                if (a[i].hoTen.length() > maxLen) maxLen = a[i].hoTen.length();
            }
            if (maxLen == 0) return;

            for (size_t d = 0; d < maxLen; d++) {
                cout << "-> Sap xep theo ky tu vi tri " << d << " (tu phai sang):" << endl;
                countingSortByCharLSD(a, n, d); // Goi ham moi
                cout << "  Mang sau khi sort: ";
                delayPrint(a, n);
            }
        }
    }
}


void bucketSort(SinhVien* a, int n, bool sortByMSSV, int sortByNameChoice = 1) {
    if (n == 0) return;

    if (sortByMSSV) {
        cout << "--- THUC HIEN BUCKET SORT (Sap xep MSSV) ---\n";

        SinhVien** buckets = new SinhVien * [NUM_BUCKETS];
        int* bucket_sizes = new int[NUM_BUCKETS]();

        long maxVal = getMax(a, n);
        if (maxVal == 0) {
            bool all_zero = true;
            for (int i = 0; i < n; i++) if (a[i].mssv != 0) all_zero = false;
            if (all_zero) {
                cout << "MSSV chi chua so 0, da sap xep.\n";
                delete[] buckets;
                delete[] bucket_sizes;
                return;
            }
        }

        long divisor = (maxVal + 1);

        for (int i = 0; i < n; ++i) {
            int bucket_index = (a[i].mssv * NUM_BUCKETS) / divisor;
            if (bucket_index >= NUM_BUCKETS) bucket_index = NUM_BUCKETS - 1;
            if (bucket_index < 0) bucket_index = 0;
            bucket_sizes[bucket_index]++;
        }

        for (int i = 0; i < NUM_BUCKETS; ++i) {
            buckets[i] = new SinhVien[bucket_sizes[i]];
            bucket_sizes[i] = 0;
        }

        cout << "1. Phan phoi phan tu vao " << NUM_BUCKETS << " Bucket: \n";

        for (int i = 0; i < n; ++i) {
            int bucket_index = (a[i].mssv * NUM_BUCKETS) / divisor;
            if (bucket_index >= NUM_BUCKETS) bucket_index = NUM_BUCKETS - 1;
            if (bucket_index < 0) bucket_index = 0;

            buckets[bucket_index][bucket_sizes[bucket_index]] = a[i];
            bucket_sizes[bucket_index]++;
        }
        cout << "   Da phan phoi xong." << endl;

        cout << "2. Sap xep tung Bucket (dung Insertion Sort): \n";
        int index = 0;
        for (int i = 0; i < NUM_BUCKETS; ++i) {
            int b_size = bucket_sizes[i];

            for (int j = 1; j < b_size; j++) {
                SinhVien key = buckets[i][j];
                int k = j - 1;
                while (k >= 0 && buckets[i][k].mssv > key.mssv) {
                    buckets[i][k + 1] = buckets[i][k];
                    k = k - 1;
                }
                buckets[i][k + 1] = key;
            }

            if (b_size > 0) { cout << "   Bucket " << i << " sau sort: " << b_size << " SV." << endl; }

            for (int j = 0; j < b_size; ++j) {
                a[index++] = buckets[i][j];
            }

            delete[] buckets[i];
        }

        delete[] buckets;
        delete[] bucket_sizes;

        delayPrint(a, n);
    }
    else {
        const int NAME_BUCKETS = 26;
        string sort_key_desc = (sortByNameChoice == 2 ? "TÊN" : "HỌ");
        cout << "--- THUC HIEN BUCKET SORT (Sap xep theo " << sort_key_desc << ") ---\n";

        SinhVien** buckets = new SinhVien * [NAME_BUCKETS];
        int* bucket_sizes = new int[NAME_BUCKETS](); // 0: A, 1: B, ...

        cout << "1. Phan phoi phan tu vao " << NAME_BUCKETS << " Bucket (A-Z): \n";

        for (int i = 0; i < n; i++) {
            int bucket_index = getCharKey(a[i], sortByNameChoice); // Dung ham ho tro
            bucket_sizes[bucket_index]++;
        }

        for (int i = 0; i < NAME_BUCKETS; ++i) {
            buckets[i] = new SinhVien[bucket_sizes[i]];
        }


        int* bucket_counts_temp = new int[NAME_BUCKETS]();


        for (int i = 0; i < n; i++) {
            int bucket_index = getCharKey(a[i], sortByNameChoice);

            buckets[bucket_index][bucket_counts_temp[bucket_index]] = a[i];
            bucket_counts_temp[bucket_index]++;
        }

        cout << "   Da phan phoi xong." << endl;

        bool (*compareFunc)(const SinhVien&, const SinhVien&);
        if (sortByNameChoice == 2) {
            compareFunc = compareGivenName;
        }
        else {
            compareFunc = compareFamilyName;
        }

        cout << "2. Sap xep tung Bucket (dung Insertion Sort): \n";
        int index = 0;
        for (int i = 0; i < NAME_BUCKETS; i++) {
            int b_size = bucket_sizes[i]; // Lay kich thuoc tu Pass 1

            // Insertion Sort cho bucket[i]
            for (int j = 1; j < b_size; j++) {
                SinhVien key = buckets[i][j];
                int k = j - 1;

                while (k >= 0 && compareFunc(key, buckets[i][k])) {
                    buckets[i][k + 1] = buckets[i][k];
                    k = k - 1;
                }
                buckets[i][k + 1] = key;
            }

            if (b_size > 0) {
                cout << "   Bucket '" << (char)('A' + i) << "' sau sort: " << b_size << " SV." << endl;
            }

            // Gop lai
            for (int j = 0; j < b_size; j++) {
                a[index++] = buckets[i][j];
            }

            delete[] buckets[i];
        }

        delete[] buckets;
        delete[] bucket_sizes;
        delete[] bucket_counts_temp;

        delayPrint(a, n);
    }
}


// =========================================================
//                          HÀM MAIN
// =========================================================

int main()
{
    srand(time(0));

    SinhVien* original_students = nullptr;
    int n_students = 0;


    original_students = readStudentsFromFile("SinhVien.txt", n_students);

    if (original_students == nullptr || n_students == 0) {
        cerr << "\n[!] CHUONG TRINH THOAT VI KHONG CO DU LIEU DE SAP XEP. Vui long kiem tra file co ton tai va chua du lieu hop le hay khong." << endl;
        cout << "Nhan Enter de thoat...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (cin.peek() == '\n') cin.get();
        return 1;
    }


    SinhVien* current_students = new SinhVien[n_students];
    for (int i = 0; i < n_students; ++i) {
        current_students[i] = original_students[i];
    }

    int chon;

    do
    {
        clearScreen();

        cout << "--- DANH SACH SINH VIEN HIỆN TẠI (" << n_students << " SV) ---\n";
        printArr(current_students, n_students);
        cout << "==========================================================\n";

        chon = menu();
        string sort_key_name = "";

        if (chon != 0) {
            // Sao chép lại mảng gốc trước khi thực hiện thuật toán (trừ Kruskal/Prim)
            if (chon != 5 && chon != 8 && chon != 9) {
                for (int i = 0; i < n_students; ++i) {
                    current_students[i] = original_students[i];
                }
            }

            switch (chon)
            {
            case 1: {
                int sort_type = 0;
                cout << "Chon kieu sap xep: 1. MSSV | 2. Ho Ten: ";
                if (!(cin >> sort_type) || (sort_type != 1 && sort_type != 2)) {
                    cout << "Lua chon khong hop le!" << endl;
                    break;
                }
                if (sort_type == 2) {
                    int name_type;
                    cout << "Sap xep Ho Ten theo: 1. Ho | 2. Ten: ";
                    if (!(cin >> name_type) || (name_type != 1 && name_type != 2)) {
                        cout << "Lua chon khong hop le!" << endl;
                        break;
                    }
                    shellSort(current_students, n_students, false, name_type);
                    sort_key_name = "Shell Sort (Ho Ten)";
                }
                else {
                    shellSort(current_students, n_students, true);
                    sort_key_name = "Shell Sort (MSSV)";
                }
                break;
            }

            case 2: {
                int sort_type = 0;
                cout << "Chon kieu sap xep: 1. MSSV | 2. Ho Ten: ";
                if (!(cin >> sort_type) || (sort_type != 1 && sort_type != 2)) {
                    cout << "Lua chon khong hop le!" << endl;
                    break;
                }
                if (sort_type == 2) {
                    int name_type;
                    cout << "Sap xep Ho Ten theo: 1. Ho | 2. Ten: ";
                    if (!(cin >> name_type) || (name_type != 1 && name_type != 2)) {
                        cout << "Lua chon khong hop le!" << endl;
                        break;
                    }
                    countingSort(current_students, n_students, false, name_type);
                    sort_key_name = "Counting Sort (Ho Ten)";
                }
                else {
                    countingSort(current_students, n_students, true);
                    sort_key_name = "Counting Sort (MSSV)";
                }
                break;
            }

            case 3: {
                int sort_type = 0;
                cout << "Chon kieu sap xep: 1. MSSV | 2. Ho Ten: ";
                if (!(cin >> sort_type) || (sort_type != 1 && sort_type != 2)) {
                    cout << "Lua chon khong hop le!" << endl;
                    break;
                }
                if (sort_type == 2) {
                    int name_type;
                    cout << "Sap xep Ho Ten theo: 1. Ho | 2. Ten: ";
                    if (!(cin >> name_type) || (name_type != 1 && name_type != 2)) {
                        cout << "Lua chon khong hop le!" << endl;
                        break;
                    }
                    radixSort(current_students, n_students, false, name_type);
                    sort_key_name = "Radix Sort (Ho Ten)";
                }
                else {
                    radixSort(current_students, n_students, true);
                    sort_key_name = "Radix Sort (MSSV)";
                }
                break;
            }

            case 4: {
                int sort_type = 0;
                cout << "Chon kieu sap xep: 1. MSSV | 2. Ho Ten: ";
                if (!(cin >> sort_type) || (sort_type != 1 && sort_type != 2)) {
                    cout << "Lua chon khong hop le!" << endl;
                    break;
                }
                if (sort_type == 2) {
                    int name_type;
                    cout << "Sap xep Ho Ten theo: 1. Ho | 2. Ten: ";
                    if (!(cin >> name_type) || (name_type != 1 && name_type != 2)) {
                        cout << "Lua chon khong hop le!" << endl;
                        break;
                    }
                    bucketSort(current_students, n_students, false, name_type);
                    sort_key_name = "Bucket Sort (Ho Ten)";
                }
                else {
                    bucketSort(current_students, n_students, true);
                    sort_key_name = "Bucket Sort (MSSV)";
                }
                break;
            }

            case 6: {
                shellSort(current_students, n_students, true);
                sort_key_name = "Shell Sort (MSSV)";
                break;
            }

            case 7: {
                int name_type;
                cout << "Sap xep Ho Ten theo: 1. Ho | 2. Ten: ";
                if (!(cin >> name_type) || (name_type != 1 && name_type != 2)) {
                    cout << "Lua chon khong hop le!" << endl;
                    break;
                }
                shellSort(current_students, n_students, false, name_type);
                sort_key_name = "Shell Sort (Ho Ten)";
                break;
            }

                  // THUẬT TOÁN KRUSKAL (MENU 8)
            case 8: {
                // Đọc đồ thị từ file GraphData.txt
                Graph* graph = readGraphFromFile("GraphData.txt");
                if (graph != nullptr) {
                    kruskalMST(graph);
                    delete[] graph->edges;
                    delete graph;
                }
                sort_key_name = "KRUSKAL MST";
                break;
            }

                  // THUẬT TOÁN PRIM (MENU 9)
            case 9: {
                // Đọc đồ thị từ file GraphData.txt
                Graph* graph = readGraphFromFile("GraphData.txt");
                if (graph != nullptr) {
                    int V = graph->V;

                    // Chuyển danh sách cạnh thành ma trận kề (Adjacency Matrix) cho Prim
                    int** AdjMatrix = new int* [V];
                    for (int i = 0; i < V; ++i) {
                        AdjMatrix[i] = new int[V];
                        for (int j = 0; j < V; ++j) {
                            AdjMatrix[i][j] = 0; // 0 nghĩa là không có cạnh
                        }
                    }

                    for (int i = 0; i < graph->E; ++i) {
                        int u = graph->edges[i].src;
                        int v = graph->edges[i].dest;
                        int w = graph->edges[i].weight;
                        if (u < V && v < V) {
                            AdjMatrix[u][v] = w;
                            AdjMatrix[v][u] = w; // Đồ thị vô hướng
                        }
                    }

                    primMST(V, AdjMatrix);

                    // Giải phóng bộ nhớ của ma trận kề và graph
                    for (int i = 0; i < V; ++i) delete[] AdjMatrix[i];
                    delete[] AdjMatrix;
                    delete[] graph->edges;
                    delete graph;
                }
                sort_key_name = "PRIM MST";
                break;
            }

            case 5:
                break;

            default:
                cout << "Lua chon khong hop le! Vui long nhap lai.\n";
            }

            if (chon != 5 && chon != 8 && chon != 9) {
                cout << "\n==========================================================\n";
                cout << "=== KET QUA " << sort_key_name << " CUOI CUNG ===\n";
                printArr(current_students, n_students);
                veDothitheoMSSV(current_students, n_students);
                veDoThiDangThanh(current_students, n_students);
            }
            else if (chon == 8 || chon == 9) {
                // Đã in kết quả MST trong hàm Kruskal/Prim
            }

            if (chon != 5) {
                cout << "Nhan Enter de tiep tuc...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (cin.peek() == '\n') cin.get();
            }
        }

    } while (chon != 5);

    cout << "\nChuong trinh ket thuc.\n";
    delete[] original_students;
    delete[] current_students;

    return 0;
}