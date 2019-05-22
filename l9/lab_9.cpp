#include <iostream>
#include <iomanip>
#include <vector>
#include "fibonacci_heap.h"

using namespace std;

// структура ребра
struct Edge {
	int a, b, w;
	Edge(int a, int b, int w) : a(a), b(b), w(w) {
	}
};

const int INF = (int)1e9;

typedef vector<int> vi;
typedef vector<vector<int>> vvi;
typedef vector<Edge> ve;
 
// алгоритм Белмана-Форда
// повертає пару:
// перший елемент true, якщо знайшовся цикл від'ємної довжини,
// другий - найкоротші відстані від вершини st до
// всіх інших вершин графу 
pair<bool, vi>  bellman_ford(const ve& e, int n, int st) {
	// спочатку відстань до початкової вершини нуль,
	// а до всіх інших нескінченність
	vi d(n);
	for(int i = 0; i < n; i++) {
		d[i] = INF;
	}
	d[st] = 0;
	// на кожній ітерації проводиться релаксація всіх ребер
	for (int k = 0; k < n - 1; k++) {
		for(int i = 0; i < e.size(); i++) {
	   		if (d[e[i].a] < INF) {
	      		d[e[i].b] = min(d[e[i].b], d[e[i].a] + e[i].w); 
			}
		}
	}
	// якщо після останньої ітерації релаксація деякого ребра 
	// ще можлива, то існує цикл від'ємної довжини 
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

// алгоритм Дейкстри
// повертає найкоротші відстані від вершини st
// до всіх інших вершин графу 
vi dijkstra(const vvi& g, const vvi& w, int st) {
	// спочатку відстань до початкової вершини нуль,
	// а до всіх інших нескінченність
	vi d(g.size());
	for (int i = 0; i < g.size(); i++) {
		 d[i] = INF;
	}
	d[st] = 0;
	vector<bool> used(g.size(), false);
	FibonacciHeap<pair<int, int>> h;
	h.insert(make_pair(0, st)); 
	// на кожній ітерації вибирається вершина з мінімальним
	// значенням відстані, а далі проводиться релаксація ребер,
	// що виходять з даної вершини
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

// алгоритм Джонсона
// функція повертає пару, перший елемент, якої є
// true, якщо в графі немає циклу від'ємної довжини,
// а другий - це матриця найкоротших відстаней
pair<bool, vvi> johnson(const vvi& g, const vvi& w) {
	// модифіковані довжини
	vvi wp(g.size());
	// ребра графа з включенням ребер нульової довжини,
	// що ведуть з додаткової вершини s з індексом g.size()
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
	// знаходимо найкоротші шляхи від доданої вершини до
	// всіх інших вершин
	auto [has_cycle, ds] = bellman_ford(es, g.size() + 1, g.size());
	// якщо в ході виконання виявили цикл від'ємної довжини
	// припиняємо виконання алгоритму
	if (has_cycle) return {false, {}};
	// потенціальна функція
	vi p(g.size());
	for (int i = 0; i < g.size(); i++) {
		p[i] = ds[i];
	}
	// розраховуємо нові довжини ребер за допомогою потенціальної функції
	for (int i = 0; i < g.size(); i++) {
		for (int j = 0; j < g[i].size(); j++) {
			wp[i][j] += p[i] - p[g[i][j]];
		}
	}
	// матриця найкоротших відстаней
	vvi d(g.size());
	for (int i = 0; i < g.size(); i++) {
		// знаходимо найкоротші відстані від вершини i
		// до всіх інших вершин в графі з невід'ємними довжинами ребер
		d[i] = dijkstra(g, wp, i);
		// знаходимо найкоротші відстані з
		// початковою функцією довжини 
		for (int j = 0; j < g.size(); j++) {
			if (d[i][j] != INF) {
				d[i][j] += p[j] - p[i];
			}
		}
	}
	return {true, d};
}

// зчитування графу, поданого у вигляді списку суміжності
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

// тестування
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
