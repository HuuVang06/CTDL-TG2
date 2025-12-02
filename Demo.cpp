#include <iostream>
using namespace std;

//Khai bao
const int MAX_V = 10;
const int MAX_NEI = MAX_V - 1;

//1)Ma tran ke
int adj_matrix[MAX_V + 1][MAX_V + 1];

//2)Danh sach ke
int adj_list[MAX_V + 1][MAX_NEI + 1];

//3)bfs, dfs
bool visited[MAX_V + 1];

//So dinh, canh thuc te
int V, E = 0;

//Khoi tao 
void khoitaoGraph() {
	for (int i = 1; i <= V; i++)
	{
		for (int j = 1; j <= V; j++)
		{
			adj_matrix[i][j] = 0;
		}
		adj_list[i][0] = 0;
	}
}

//Nhap 
void inputGraph(int v, int e, const int edges[][2]) {
	V = v;
	E = e;

	if (V < 1 || V > MAX_V)
	{
		cout << "Nhap khong hop le! ";
		V , E = 0;
		return;
	}

	khoitaoGraph();

	cout << "NHAP DU LIEU TU DONG \n";
	cout << "V=" << V << " ,E=" << E << " \n";
	for (int k = 0; k < E; k++)
	{
		int u = edges[k][0];
		int v = edges[k][1];

		//Cap nhat ma tran ke vo huong
		adj_matrix[u][v] = 1;
		adj_matrix[v][u] = 1;

		//Cap nhat danh sach ke (mo phong)
		//Dinh u
		int count_u = ++adj_list[u][0];
		if (count_u <= MAX_NEI)
		{
			adj_list[u][count_u] = v;
		}

		//Dinh v
		int count_v = ++adj_list[v][0];
		if (count_v <= MAX_NEI)
		{
			adj_list[v][count_v] = u;
		}
	}
	cout << "NHAP DU LIEU HOAN TAT! \n";
}

//Xuat
//Ma tran ke
void outputMatrix() {
	if (V == 0)
	{
		cout << "Do thi hien chua co du lieu.\n";
		return;
	}
	cout << "KET QUA BIEU DIEN MA TRAN KE! \n";
	cout << " ";
	for (int j = 1; j <= V; j++)
	{
		cout << " " << j;
	}
	cout << endl;

	for (int i = 1; i <= V; i++)
	{
		cout << i << " ";
		for (int j = 1; j <= V; j++)
		{
			cout << " " << adj_matrix[i][j];
		}
		cout << endl;
	}
}

//Xuat danh sach ke
void outputList() {
	if (V == 0)
	{
		cout << "Do thi hien chua co du lieu.\n";
		return;
	}
	cout << "KET QUA BIEU DIEN DANH SACH KE! \n";
	
	for (int i = 1; i <= V; i++)
	{
		cout << "Dinh " << i << " (" << adj_list[i][0] << ") ke: ";
		for (int j = 1; j <= adj_list[i][0]; j++)
		{
			cout << "->" << adj_list[i][j];
		}
		cout << endl;
	}
}

//BFS
void BFS_traversal(int start_node) {

}

//DFS
void DFS_traversal(int start_node) {

}

//Menu
void menuDisplay() {
	int chon = 0;
	//int start_node;

	//Du lieu mau
	const int v = 7;
	const int e = 9;
	const int edges[e][2] = { {1, 2}, {1, 3}, {2, 4}, {3, 4}, {3, 6}, {4, 5}, {5, 6}, {5, 7}, {6, 7} };
	
	do
	{
		cout << "CHUONG TRINH BIEU DIEN VA DUYET DO THI \n";
		cout << "1.NHAP DU LIEU(TU DONG). \n";
		cout << "2.XUAT DO THI THEO MA TRAN KE. \n";
		cout << "3.XUAT DO THI THEO DANH SACH KE \n";
		cout << "4.DUYET THEO BFS. \n";
		cout << "5.DUYET THEO DFS. \n";
		cout << "0.THOAT CHUONG TRINH. \n";
		cout << "Nhap vao lua chon cua ban: ";
		if (!(cin >> chon))return;

		switch (chon)
		{
		case 1:
			inputGraph(v, e, edges);
			break;
		case 2:
			outputMatrix();
			break;
		case 3:
			outputList();
			break;
		case 4:
			break;
		case 5:
			break;
		case 0:
			cout << "Cam on ban da su dung chuong trinh! \n";
			break;
		default:
			cout << "Chuc nang chua phat trien! Cam on vi da su dung! \n";
			break;
		}
	} while (chon != 0);
}




int main() {
	menuDisplay();
	return 0;
}