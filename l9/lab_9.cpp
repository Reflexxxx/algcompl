#include <iostream>
#include <iomanip>
#include <vector>
#include "fibonacci_heap.h"

using namespace std;

// ��������� �����
struct Edge {
	int a, b, w;
	Edge(int a, int b, int w) : a(a), b(b), w(w) {
	}
};

const int INF = (int)1e9;

typedef vector<int> vi;
typedef vector<vector<int>> vvi;
typedef vector<Edge> ve;
 
// �������� �������-�����
// ������� ����:
// ������ ������� true, ���� ��������� ���� ��'���� �������,
// ������ - ���������� ������ �� ������� st ��
// ��� ����� ������ ����� 
pair<bool, vi>  bellman_ford(const ve& e, int n, int st) {
	// �������� ������� �� ��������� ������� ����,
	// � �� ��� ����� ������������
	vi d(n);
	for(int i = 0; i < n; i++) {
		d[i] = INF;
	}
	d[st] = 0;
	// �� ����� �������� ����������� ���������� ��� �����
	for (int k = 0; k < n - 1; k++) {
		for(int i = 0; i < e.size(); i++) {
	   		if (d[e[i].a] < INF) {
	      		d[e[i].b] = min(d[e[i].b], d[e[i].a] + e[i].w); 
			}
		}
	}
	// ���� ���� �������� �������� ���������� ������� ����� 
	// �� �������, �� ���� ���� ��'���� ������� 
	bool has_cycle = false;
	for(int i = 0; i < e.size(); i++) {
		if (d[e[i].b] < INF && d[e[i].a] + e[i].w < d[e[i].b]) {
			d[e[i].b] = d[e[i].a] + e[i].w;
			has_cycle = true;
			break;
		}
	}
	if (has_cycle) return {true, {}};
	return {false, d};
}

// �������� ��������
// ������� ���������� ������ �� ������� st
// �� ��� ����� ������ ����� 
vi dijkstra(const vvi& g, const vvi& w, int st) {
	// �������� ������� �� ��������� ������� ����,
	// � �� ��� ����� ������������
	vi d(g.size());
	for (int i = 0; i < g.size(); i++) {
		 d[i] = INF;
	}
	d[st] = 0;
	vector<bool> used(g.size(), false);
	FibonacciHeap<pair<int, int>> h;
	h.insert(make_pair(0, st)); 
	// �� ����� �������� ���������� ������� � ���������
	// ��������� ������, � ��� ����������� ���������� �����,
	// �� �������� � ���� �������
	for (int i = 0; i < g.size() - 1; i++) {
		 int mi = h.getMinimum().second;
		 h.extractMinimum(); 
		 for (int j = 0; j < g[mi].size(); j++) {
		 	  if (!used[g[mi][j]] && d[g[mi][j]] > d[mi] + w[mi][j]) {
		 	  	  d[g[mi][j]] = d[mi] + w[mi][j];  
		 	  	  h.insert(make_pair(d[g[mi][j]], g[mi][j])); 
			  }
		 }
		 used[mi] = true;
	}
	return d;
}

// �������� ��������
// ������� ������� ����, ������ �������, ��� �
// true, ���� � ����� ���� ����� ��'���� �������,
// � ������ - �� ������� ����������� ��������
pair<bool, vvi> johnson(const vvi& g, const vvi& w) {
	// ����������� �������
	vvi wp(g.size());
	// ����� ����� � ���������� ����� ������� �������,
	// �� ������ � ��������� ������� s � �������� g.size()
	ve es;
	for (int i = 0; i < g.size(); i++) {
		for (int j = 0; j < g[i].size(); j++) {
			es.push_back(Edge(i, g[i][j], w[i][j]));
			wp[i].push_back(w[i][j]);
		}
	} 
	for (int i = 0; i < g.size(); i++) {
		es.push_back(Edge(g.size(), i, 0));
	}
	// ��������� ���������� ����� �� ������ ������� ��
	// ��� ����� ������
	auto [has_cycle, ds] = bellman_ford(es, g.size() + 1, g.size());
	// ���� � ��� ��������� ������� ���� ��'���� �������
	// ���������� ��������� ���������
	if (has_cycle) return {false, {}};
	// ������������ �������
	vi p(g.size());
	for (int i = 0; i < g.size(); i++) {
		p[i] = ds[i];
	}
	// ����������� ��� ������� ����� �� ��������� ������������ �������
	for (int i = 0; i < g.size(); i++) {
		for (int j = 0; j < g[i].size(); j++) {
			wp[i][j] += p[i] - p[g[i][j]];
		}
	}
	// ������� ����������� ��������
	vvi d(g.size());
	for (int i = 0; i < g.size(); i++) {
		// ��������� ���������� ������ �� ������� i
		// �� ��� ����� ������ � ����� � ����'������ ��������� �����
		d[i] = dijkstra(g, wp, i);
		// ��������� ���������� ������ �
		// ���������� �������� ������� 
		for (int j = 0; j < g.size(); j++) {
			if (d[i][j] != INF) {
				d[i][j] += p[j] - p[i];
			}
		}
	}
	return {true, d};
}

// ���������� �����, �������� � ������ ������ ��������
pair<vvi, vvi> read_graph(istream& cin = cin) {
	int n, m;
	cin >> n >> m;
	vvi g(n), w(n);
	for (int i = 0; i < m; i++) {
		int u, v, cost;
		cin >> u >> v >> cost;
		g[u].push_back(v);
		w[u].push_back(cost);
	}
	return {g, w};
}

// ����������
int main() {
	auto graph = read_graph(); 
	auto [possible, distances] = johnson(graph.first, graph.second);
	if (!possible) {
		cout << "The graph has a cycle with negative length" << endl;
		return 0;
	}
	for (auto row: distances) {
		for (auto d: row) {
			cout << setw(4) << (d == INF ? "inf" : to_string(d)) << " ";
		}
		cout << endl;
	}
}
