#include <iostream>
#include <ctime>
#include <cmath>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

using namespace std;

const long long INF = LLONG_MAX;
const int MAX_VER = 100005;
const int MAX_ED = 200005;

int num_ver = 0;
long long dist[MAX_VER];
bool mark[MAX_VER];

//Khai bao
struct SinhVien {
    string hoTen;
    long mssv;
};

struct Edge {
    int to;
    int weight;
    int next;
};

Edge edges[MAX_ED];
int head[MAX_VER]; 
int edge_count = 0;    

const auto DELAY_TIME = chrono::milliseconds(500);
const int NUM_BUCKETS = 10;

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


bool compareGivenName(const SinhVien& a, const SinhVien& b) {
    string name_a = extractLastName(a.hoTen);
    string name_b = extractLastName(b.hoTen);
    if (name_a != name_b) {
        return name_a < name_b;
    }
    return a.hoTen < b.hoTen;
}

bool compareFamilyName(const SinhVien& a, const SinhVien& b) {
    return a.hoTen < b.hoTen;
}


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

void add_edge(int u, int v, int w) {
    edges[edge_count].to = v;
    edges[edge_count].weight = w;
    edges[edge_count].next = head[u];
    head[u] = edge_count;
    edge_count++;
}

void InitGraph() {
    num_ver = 5;

    edge_count = 0;
    for (int i = 1; i <= num_ver; ++i) {
        head[i] = -1;
    }

    add_edge(1, 2, 10);
    add_edge(1, 3, 5);
    add_edge(3, 2, 2);
    add_edge(2, 4, 1);
    add_edge(3, 4, 9);
    add_edge(3, 5, 2);
    add_edge(4, 5, 4);

    cout << "[INFO] Do thi Dijkstra da duoc khoi tao voi " << num_ver << " dinh (1->5) su dung Danh sach ke bang Mang tinh.\n";
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
    cout << "8. Chay thuat toan Dijkstra.\n";
    cout << "--------------------------------\n";
    cout << "5. Dung lai. \n";
    cout << "Nhap vao lua chon (1-8, 5 de dung):";

    if (!(cin >> chon) || (chon < 1 || chon > 8)) {
        if (cin.fail()) {
            cout << "\nLoi nhap lieu! Vui long nhap so nguyen.\n";
        }
        else {
            cout << "\nLua chon khong hop le! Vui long nhap tu 1-8.\n";
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return 0;
    }
    return chon;
}

void printArr(const SinhVien* a, int n)
{
    cout << "  --------------------------------------------------\n";
    for (int i = 0; i < n; ++i)
    {
        cout << "  | Ho Ten: " << a[i].hoTen;
        size_t padding = 20;
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
                cout << "    Chen SV co " << (sortByMSSV ? "MSSV " + to_string(temp.mssv) : "Ten '" + temp.hoTen + "'") << " vao vi tri " << j << ": ";
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
    if (sortByNameChoice == 2) { 
        key = extractLastName(sv.hoTen);
    }
    else { 
        key = sv.hoTen;
    }

    if (key.empty()) return 0; 
    int c = myToUpper(key[0]) - 'A'; 
    if (c < 0 || c > 25) return 0;
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

        cout << "    Ket qua Counting Sort (theo ky tu dau): " << endl;
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
        if (range > 1000001) { 
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

        cout << "    Ket qua Counting Sort (theo MSSV): " << endl;
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
    int count[257] = { 0 }; 

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
            cout << "    Mang sau khi sort theo vi tri " << exp << ": ";
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
                countingSortByLastNameCharLSD(a, n, d); 
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
        cout << "    Da phan phoi xong." << endl;

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

            if (b_size > 0) { cout << "    Bucket " << i << " sau sort: " << b_size << " SV." << endl; }

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
        int* bucket_sizes = new int[NAME_BUCKETS]();

        cout << "1. Phan phoi phan tu vao " << NAME_BUCKETS << " Bucket (A-Z): \n";

        for (int i = 0; i < n; i++) {
            int bucket_index = getCharKey(a[i], sortByNameChoice); 
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

        cout << "    Da phan phoi xong." << endl;

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
            int b_size = bucket_sizes[i]; 

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
                cout << "    Bucket '" << (char)('A' + i) << "' sau sort: " << b_size << " SV." << endl;
            }

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

void Dijkstra(int s) {
    if (num_ver == 0) {
        cout << "[!] LOI: Do thi chua duoc khoi tao (num_vertices = 0)." << endl;
        return;
    }

    cout << "--- THUC HIEN THUAT TOAN DIJKSTRA O(V^2) (Tu dinh nguon " << s << ") ---" << endl;

    for (int i = 1; i <= num_ver; ++i) {
        dist[i] = INF;
        mark[i] = false;
    }
    dist[s] = 0;

    for (int i = 1; i <= num_ver; ++i) {

        int v = -1;
        for (int j = 1; j <= num_ver; ++j) {
            if (!mark[j] && (v == -1 || dist[j] < dist[v])) {
                v = j;
            }
        }

        if (v == -1) break;

        mark[v] = true;

    
        for (int current_edge_index = head[v]; current_edge_index != -1; current_edge_index = edges[current_edge_index].next) {

            int u = edges[current_edge_index].to;
            int weight = edges[current_edge_index].weight;

            if (dist[v] != INF && dist[v] + weight < dist[u]) {
                dist[u] = dist[v] + weight;
            }
        }

        cout << "   -> Buoc " << i << ": Chon dinh " << v << ". Khoang cach cua no la " << dist[v] << "." << endl;
    }

    cout << "\n=== KET QUA DIJKSTRA ===" << endl;
    for (int i = 1; i <= num_ver; ++i) {
        cout << "Khoang cach tu " << s << " den " << i << ": ";
        if (dist[i] == INF) {
            cout << "INF (Khong the toi duoc)" << endl;
        }
        else {
            cout << dist[i] << endl;
        }
    }
    cout << "========================" << endl;
}


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

        cout << "--- DANH SACH SINH VIEN HIEN TAI (" << n_students << " SV) ---\n";
        printArr(current_students, n_students);
        cout << "==========================================================\n";

        chon = menu();

        if (chon == 0) continue;
        if (chon == 5) {
            cout << "\n Cam on da su dung chuong trinh! \n";
            break;
        }


        SinhVien* temp_students = new SinhVien[n_students];
        for (int i = 0; i < n_students; ++i) {
            temp_students[i] = original_students[i];
        }

        string sort_key_name = "MSSV";
        int name_choice = 1; 
        bool sortByMSSV = true; 

        switch (chon)
        {
        case 1: 
        case 2:
        case 3: 
        case 4:
        {
            int sort_key = 1;
            cout << "\nBan muon sap xep theo khoa nao?\n";
            cout << "1. Ma so sinh vien (MSSV)\n";
            cout << "2. Ho Ten\n";
            cout << "Nhap lua chon khoa (1 hoac 2): ";

            if (!(cin >> sort_key) || (sort_key != 1 && sort_key != 2)) {
                cout << "Lua chon khoa khong hop le! Mac dinh sap xep theo MSSV (1)." << endl;
                sort_key = 1;
            }

            sortByMSSV = (sort_key == 1);

            if (sort_key == 2) { 
                cout << "\nBan muon sap xep theo:\n";
                cout << "1. HO (Toan bo chuoi Ho Ten)\n";
                cout << "2. TEN (Phan ten cuoi)\n";
                cout << "Nhap lua chon (1 hoac 2): ";
                if (!(cin >> name_choice) || (name_choice != 1 && name_choice != 2)) {
                    cout << "Lua chon khong hop le! Mac dinh sap xep theo HO (1)." << endl;
                    name_choice = 1;
                }
            }

            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (sortByMSSV) {
                sort_key_name = "MSSV";
            }
            else {
                sort_key_name = (name_choice == 2) ? "HO TEN (Theo TEN)" : "HO TEN (Theo HO)";
                
                if (chon == 2) {
                    sort_key_name += " - THEO KY TU DAU";
                }
            }

            
            if (chon == 1) shellSort(temp_students, n_students, sortByMSSV, name_choice);
            if (chon == 2) countingSort(temp_students, n_students, sortByMSSV, name_choice);
            if (chon == 3) radixSort(temp_students, n_students, sortByMSSV, name_choice);
            if (chon == 4) bucketSort(temp_students, n_students, sortByMSSV, name_choice);

            break;
        }
      

        case 6: 
            shellSort(temp_students, n_students, true);
            sort_key_name = "MSSV";
            break;

        case 7: {
            cout << "\n Ban muon sap xep nhanh theo: \n";
            cout << "1. HO (Toan bo chuoi Ho Ten)\n";
            cout << "2. TEN (Phan ten cuoi)\n";
            cout << "Nhap lua chon (1 hoac 2): ";
            if (!(cin >> name_choice) || (name_choice != 1 && name_choice != 2)) {
                cout << "Lua chon khong hop le! Mac dinh sap xep theo HO (1)." << endl;
                name_choice = 1;
            }
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');

            shellSort(temp_students, n_students, false, name_choice);
            sort_key_name = (name_choice == 2) ? "HO TEN (Theo TEN)" : "HO TEN (Theo HO)";
            break;
        }
        case 8:
        {
            InitGraph();

            int start_node = 1;
            cout << "\nChay Dijkstra tren do thi co " << num_ver << " dinh.\n";
            cout << "Nhap dinh nguon (1-" << num_ver << "): ";

            if (!(cin >> start_node) || start_node < 1 || start_node > num_ver) {
                cout << "Lua chon khong hop le! Mac dinh chon dinh 1." << endl;
                start_node = 1;
            }

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            Dijkstra(start_node);

            cout << "\nNhan Enter de tiep tuc...";
            if (cin.peek() == '\n') cin.get();

            delete[] temp_students;
            temp_students = nullptr; 

            break;
        }

        default:
            if (chon != 5 && chon != 0) {
                cout << "\nLua chon khong duoc xu ly hoac khong hop le.\n";
            }
            delete[] temp_students;
            temp_students = nullptr;
            break;
        }

        if (chon >= 1 && chon <= 4 || chon == 6 || chon == 7) {
            delete[] current_students;
            current_students = temp_students;

            cout << "\n============================================\n";
            cout << "DANH SACH DA SAP XEP CUOI CUNG THEO " << sort_key_name << ": ";
            printArr(current_students, n_students);
            cout << "\n============================================\n";

            cout << "\nNhan Enter de tiep tuc...";

            if (cin.peek() == '\n') cin.get();
        }
       

    } while (true);

    delete[] original_students;
    delete[] current_students;

    return 0;
}