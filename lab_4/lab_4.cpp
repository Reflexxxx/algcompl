#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

const int INF = 1000 * 1000 * 1000;

// дерево бінарного пошуку
template <typename T>
class BinarySearchTree {
	// l, r - вказівники на ліве і праве піддерево 
	// key - значення у вузлі
	struct Node {
		Node *l, *r;
		T key;
		
		Node(T key = T(), Node* l = NULL, Node* r = NULL) : key(key), l(l), r(l) {
		}
	};
	// корінь
	Node* root;
	// сума добутків частот на глибини відповідних значень (вартість)
	int cost;
	
	// побудувати оптимальне дерево на основі частот frs і відповідних ключів keys 
	// для ключів від l до r та/або обчислити його найменшу вартість
	// dp[l][r] = 0, якщо дана функція не викликалась з даними параметрами, інакше - це 
	// найменша вартість бінарного дерева, побудованого з ключів від l до r включно
	// frs, keys - частоти і відповідні ключі
	// функція повертає побудоване оптимальне дерево або нульовий вказівник
	Node* buildOptimalDP(int l, int r, bool create, vector < vector <int> >& dp, vector <int>& frs, vector <T>& keys) {
		// дерево складається з одного вузла
		if (l == r) {
			dp[l][r] = frs[l];
			return create ? new Node(keys[l], NULL, NULL) : NULL;
		}
		// обчислюємо суму частот
		int s = 0;
		for (int i = l; i <= r; i++) {
			s += frs[i];
		}
		// знаходимо оптимальну вартість та вершину, яку найбільш вигідно зробити корнем
		int mn = INF, mni = l;
		for (int i = l; i <= r; i++) {
			if (i != l && !dp[l][i - 1]) {
				buildOptimalDP(l, i - 1, false, dp, frs, keys);
			}
			if (i != r && !dp[i + 1][r]) {
				buildOptimalDP(i + 1, r, false, dp, frs, keys);
			}
			int c = (i == l ? 0 : dp[l][i - 1])  + (i == r ? 0 : dp[i + 1][r]);
			if (c < mn) {
				mn = c;
				mni = i;
			}
		}
		// застосовуємо рекурентну формулу
		dp[l][r] = mn + s;
		if (!create) {
			return NULL;
		}
		// будуємо дерево та вертаємо вказівник на корінь, якщо необхідно
		Node* node = new Node(keys[mni]);
		node->l = l == mni ? NULL : buildOptimalDP(l, mni - 1, true, dp, frs, keys);
		node->r = r == mni ? NULL : buildOptimalDP(mni + 1, r, true, dp, frs, keys);
		return node;
	}
	
	// побудувати оптимальне дерево на основі частот frs і відповідних ключів keys
	// функція повертає мінімальну вартість дерева, яку можна досягти
	int buildOptimal(vector <int>& frs, vector <T>& keys) {
		vector < vector <int> > dp(frs.size());
		for (auto& v : dp) {
			v.assign(frs.size(), 0);
		}
		root = buildOptimalDP(0, frs.size() - 1, true, dp, frs, keys);
		return dp[0][frs.size() - 1];
	}
	
	// виведення дерева (рекурсивна частина)
	void show(Node* node, int depth, char dir) {
		if (!node) return;
		for (int i = 0; i < depth; i++) {
			cout << " ";	
		}
		cout << dir << "[" << ((string)node->key).c_str() << "]" << endl; 
		show(node->l, depth + 2, '-');
		show(node->r, depth + 2, '+');
	}
	
public:
	// конструктор 1
	BinarySearchTree() : root(NULL) {
	}	
	
	// конструктор 2
	BinarySearchTree(vector <int> frs, vector <T> keys) : BinarySearchTree() {
		cost = buildOptimal(frs, keys);
	}
		
	// виведення дерева
	void show() {
		cout << "[tree]" << endl;
		cout << "cost = " << cost << endl;
		show(root, 0, '*');
	}
	
};

// підрозділ компанії
class CompanyDivision {
	// назва
	string name;
	friend void read(); 
	
public:
	// рядкове представлення компанії
	operator string() {
		return name;
	}
};

typedef pair <CompanyDivision, int> CInfo;

// список пар вигляду: частота звертання до підрозділу - сам підрозділ
vector <CInfo> compDivsInfo; 

// зчитування підрозділів компанії та їх апріорних частот
void read() {
	int divN;
	cin >> divN;
	for (int i = 0; i < divN; i++) {
		CompanyDivision compDiv;
		cin >> compDiv.name; 
		int fr;
		cin >> fr;
		compDivsInfo.push_back({compDiv, fr});
	}
}

// тестування
int main() {
	read();
	// сортування підрозділів за збільшенням частоти
	sort(compDivsInfo.begin(), compDivsInfo.end(), 
		[](CInfo a, CInfo b) { return (string)a.first < (string)b.first; }); 
	vector <int> frs;
	vector <CompanyDivision> compDivs;
	for (auto it : compDivsInfo) {
		compDivs.push_back(it.first);
		frs.push_back(it.second);
	}
	// побудова оптимального дереве бінарного пошуку на основі частот і ключів
	BinarySearchTree <CompanyDivision> bst(frs, compDivs);
	bst.show();
}
