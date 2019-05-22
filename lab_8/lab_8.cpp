#include <iostream>
#include <vector>

using namespace std;

// купа Фібоначчі
template <class T, typename Comp = less<T>> 
class FibonacciHeap {
	
	// вузол купи Фібоначчі
	struct Node {
		T key; // ключ
		// вказіники на правий і лівий елементи двозв'язного циклічного списку
		Node* right; 
		Node* left;
		Node* child; // вказівник на один із синів зі списку синів даного вузла
		Node* parent; // вказівник на батька
		int degree; // степінь вершини
		bool mark; // чи був видалений в процесі зміни ключа син цієї вершини
		
		// конструктор вузла
		Node(T key) {
			this->key = key;
			this->right = this->left = this;
			this->child = NULL;
			this->parent = NULL;
			this->degree = 0;
			this->mark = false;
		}
	};
	
	// вказівник на вузол з мінімальним ключем
	Node* h;
	
	// кількість вузлів в купі
	int __size;
	
	// компаратор для порівняння об'єктів
	Comp comp;
	
	// злиття двох куп
	// аналогічно злиттю двох двозв'язних циклічних списків
	Node* __merge(Node* a, Node* b) {
		if (a == NULL) {
			return b;
		}
		if (b == NULL) {
			return a;
		}
		if (comp(b->key, a->key)) {
			Node* temp = a;
			a = b;
			b = temp;
		}
		Node* al = a->left;
		Node* br = b->right;
		a->left = b;
		b->right = a;
		al->right = br;
		br->left = al;
		return a;
	}
	
	// функція аналогічна об'єднанню двох 
	// біноміальних дерев
	void addChild(Node* parent, Node* child) {
		child->right = child->left = child;
		child->parent = parent;
		parent->degree++;
		parent->child = __merge(parent->child, child);
	}

	// всім вузлам-сусідам даного вузла
	// змінити вказівники на батька на NULL,
    // а відмітки на false
	void normalize(Node* node) {
		if (node == NULL) {
			return;
		}
		Node* cur = node;
		do {
			cur->mark = false;
			cur->parent = NULL;
			cur = cur->left;
		} while (cur != node);
	}
	
	// проріджування дерев 
	Node* consolidate(Node* node) {
		Node* ts[1 << 6] = {NULL};
		while (true) {
			if (ts[node->degree] != NULL) {
				// підвісимо до меньшого вузла із поточного корня і того,
				// що лежить в комірці інший
				Node* t = ts[node->degree];
				if (t == node) break; // елемент масиву не змінився
				ts[node->degree] = NULL;
				if (comp(node->key, t->key)) {
					t->right->left = t->left;
					t->left->right = t->right;
					addChild(node, t);
				} else {
					t->right->left = t->left;
					t->left->right = t->right;
					if (node->left == node) {
						t->left = t->right = t;
						addChild(t, node);
						node = t;
					} else {
						node->right->left = t;
						node->left->right = t;
						t->left = node->left;
						t->right = node->right;
						addChild(t, node);
						node = t;
					}
				}
				continue;
			} else { // якщо комірка пуста, то покладемо сюди даний вузол
				ts[node->degree] = node;
			}
			node = node->left;
		}
		// оновлюємо мінімум, якщо необхідно
		Node* mn = node;
		Node* start = node;
		do {
			if (comp(node->key, mn->key)) {
				mn = node;
			}
			node = node->left;
		} while (node != start);
		return mn;
	}

	// видалення мінімального елемента з купи (рекурсивна частина)
	Node* __extractMinimum(Node* node) {
		// об'єднуємо список дітей мінімального вузла з кореневим 
		// та вилучаємо даний вузол зі списку
		normalize(node->child);
		if (node->left == node) {
			node = node->child;
		} else {
			node->left->right = node->right;
			node->right->left = node->left;
			node = __merge(node->left, node->child);
		}
		if (node == NULL) {
			return node;
		}
		__size--;
		return consolidate(node);
	}

	// при вирізанні вершини ми
	// видаляємо її зі списка дітей свого батька,
	// зменшуємо степінь її батька і знімаємо мітку з даної вершини
	Node* cut(Node* h, Node* node) {
		if (node->left == node) {
			node->parent->child = NULL;
		} else {
			node->left->right = node->right;
			node->right->left = node->left;
			node->parent->child = node->left;
		}
		node->left = node->right = node;
		node->mark = false;
		return __merge(h, node);
	}

	// зменшення значення елемента (рекурсивна частина)
	Node* __decreaseKey(Node* h, Node* node, T key) {
		if (comp(node->key, key)) {
			return h;
			// ключ насправді більший
		}
		node->key = key;
		if (node->parent) {
			if (comp(node->key, node->parent->key)) {
				// вирізання
				h = cut(h, node);
				Node* parent = node->parent;
				node->parent = NULL;
				// каскадне вирізання
				while (parent != NULL && parent->mark) {
					h = cut(h, parent);
					node = parent;
					parent = node->parent;
					node->parent = NULL;
				}
				if (parent != NULL && parent->parent != NULL) {
					parent->mark = true;
				}
			}
		} else {
			if (comp(node->key, h->key)) {
				h = node;
			}
		}
		return h;
	}
	
	// виведення купи Фібоначчі (рекурсивна частина)
	void __print(Node* node, int depth) {
		if (node == NULL) {
			return;
		}
		Node* cur = node;
		do {
			auto makeSpaces = [&]() {
				for (int i = 0; i < depth; i++) {
					cout << " ";
				}	
			};
			makeSpaces();
			cout << "(" << (string)cur->key << ", " << cur->degree << ") ->" << endl;
			makeSpaces();
			cout << "|" << endl;
			makeSpaces();
			cout << "V" << endl;
			__print(cur->child, depth + 4);
			cur = cur->right;
		} while (cur != node);
	}

public:

	// конструктор купи
	FibonacciHeap(Comp comp = Comp()) : h(NULL), __size(0), comp(comp) {
	}

	// вставка елемента
	void insert(T key) {
		Node* nh = new Node(key);
		h = __merge(h, nh);
		__size++;
	}
	
	// знайти мінімум
	T getMinimum() {
		return h->key;
	}
	
	// злиття двох куп Фібоначчі
	void merge(FibonacciHeap other) {
		this->h = __merge(this->h, other.h);
	}

	// видалити мінімум
	void extractMinimum() {
		Node* oh = h;
		h = __extractMinimum(h);
		T nh = oh->key;
	}
	
	// перевірка купи на порожність
	bool isEmpty() {
		return h == NULL;
	}
	
	// повернення розміру купи
	int size() {
		return __size;
	}
	
	// виведення купи
	void print() {
		if (isEmpty()) {
			cout << "Heap is empty" << endl;
			return;
		}
		cout << "Size of heap = " << __size << endl;
		__print(h, 0);
		cout << endl;
	}

};

// підрозділ компанії
class CompanyDivision {
	// назва
	string name;
	
public:
	
	// конструктор 1
	CompanyDivision() : name("") {
	}
	
	// конструктор 2
	CompanyDivision(string name) : name(name) {
	}
	
	// рядкове представлення компанії
	operator string() const {
		return name;
	}
};

// оператор порівняння двох підрозділів
bool operator <(const CompanyDivision& a, const CompanyDivision& b) {
	return (string)a < (string)b;
}

// тестування
int main() {
	vector <string> names = {"divD", "divE", "divB", "divH", "divF", "divC"};
	FibonacciHeap <CompanyDivision> fh;
	for (auto name : names) {
		fh.insert(CompanyDivision(name));
	}
	cout << "Heap after inserting ";
	cout << names[0];
	for (int i = 1; i < names.size(); i++) {
		cout << ", " << names[i]; 
	}
	cout << endl;
	fh.print();
	cout << "Minimum is " << (string)fh.getMinimum() << endl;
	fh.extractMinimum();
	string minName = "divA";
	fh.insert(CompanyDivision(minName));
	cout << "Heap after extracting minumum and inserting " << minName << ": " << endl;
	fh.print();
	cout << "Minimum is " << (string)fh.getMinimum() << endl;
}
