#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// B+ дерево
template <typename T>
class BPlusTree {
	
	// максимальна степінь дерева
	static const int B = 31; 
	
	// вузол B+ дерева
	struct Node {
	    int keyNum; // кількість ключів вузла
	    int key[B]; // ключі вузла
	    void* pointers[B + 1]; // вказівники на дітей або, 
							   // якщо вузол є листом, на значення
	    Node* parent; // вказівник на батька
	    bool isLeaf; // чи є вузол листом
	    
	    // конструктор вузла
	    Node() {
	    	isLeaf = false;
	    	keyNum = 0;
	    	pointers[0] = NULL;
	    	parent = NULL;
		}
	};
	
	Node* root; // корінь дерева
	int __size; // кількість вузлів в дереві
	
	// виведення (рекурсивна частина)
	void __print(Node* node) {
		for (int i = 0; i <= node->keyNum; i++) {
            if (node->isLeaf) {
            	if (i == 0) continue;
            	cout << node->key[i - 1] << ":" << (string)*(T*)(node->pointers[i]) << "  ";
			} else {
                __print((Node*)node->pointers[i]);
            } 
        }
	}
	
public:
	
	// конструктор дерева
	BPlusTree() {
	    root = new Node();
	    root->isLeaf = true;
	    __size = 0;
	}
	
	// вставка елемента в дерево
	bool insert(int key, void* value) {
	    Node* leaf = findLeaf(key);
	    for (int i = 0; i < leaf->keyNum; i++) {
	    	if (key == leaf->key[i]) {
	    		return false;
			}
		}
	    insertInLeaf(leaf, key, value);
	    __size++;
	    return true;
	}
	
	// знаходження листа в якому знаходиться даний ключ
	// якщо елемент з таким ключем вже є в дереві, повертається false
	Node* findLeaf(int key) {
	    Node* cur = root;
	    while (!cur->isLeaf) {
	        for (int i = 0; i <= cur->keyNum; i++) {
	            if (i == cur->keyNum || key < cur->key[i]) {
	                cur = (Node*)cur->pointers[i];
	                break;
	            }
	        }
	    }
	    return cur;
	}
	
	// вставка елемента в лист
	void insertInLeaf(Node* node, int key, void* value) {
	    int k = 0;
	    while (k < node->keyNum && node->key[k] < key) {
	    	k++;
		}
	    for (int i = node->keyNum; i > k; i--) {
	    	node->key[i] = node->key[i - 1];
		}
	        
	    for (int i = node->keyNum + 1; i > k + 1; i--) {
	    	node->pointers[i] = node->pointers[i - 1];
		}
	    node->key[k] = key;
	    node->pointers[k + 1] = value;
	    node->keyNum++;
		// якщо після вставки лист виявився заповненим
		// необхідно його розбити
	    if (node->keyNum == B) {
	    	split(node);
		}
	}
	
	// розбиття дерева
	void split(Node* node) {
	    Node* nodeN = new Node();
	    int mid = node->key[B / 2];
		// переносимо праву частину елементів node в вузол nodeN
	    nodeN->keyNum = (B - 1) / 2;
	    for (int i = 0; i < nodeN->keyNum; i++) {
	        nodeN->key[i] = node->key[i + (B / 2 + 1)];
	        nodeN->pointers[i] = node->pointers[i + (B / 2 + 1)];
	    }
	    nodeN->pointers[nodeN->keyNum] = node->pointers[B];
	    node->keyNum = B / 2;
		// якщо вузол лист, то ключ, що залишився також
		// додається в праве піддерево, а його копія
		// відправляється в батьківський вузол і стає ключем розподілу
	    if (node->isLeaf) {
	        node->keyNum++;
	        nodeN->pointers[0] = node->pointers[0];
	        node->pointers[0] = nodeN;
	        nodeN->isLeaf = true;
	        mid = node->key[B / 2 + 1];
	    }
		// якщо вузол корінь, створюється новий корінь,
		// що розбиває ключі на дві групи і
		// висота дерева збільшується
	    if (!node->parent) {
	        root = new Node();
	        root->key[0] = mid;
	        root->pointers[0] = node;
	        root->pointers[1] = nodeN;
	        root->keyNum = 1;
	        node->parent = nodeN->parent = root;
	    } else {
	        nodeN->parent = node->parent;
	        insertInLeaf((Node*)node->parent, mid, (void*)nodeN);
	    }
	}
	
	// видалення елемента за ключем
	// якщо такого елемента не має, повертається false
	bool erase(int key) {
	    Node* leaf = findLeaf(key);
	    for (int i = 0; i < leaf->keyNum; i++) {
	    	if (key == leaf->key[i]) {
	            eraseFromLeaf(leaf, key);
	            __size--;
	            return true;
	        }
		}
	    return false;
	}
	
	// видалення елемента з листа за ключем
	void eraseFromLeaf(Node* node, int key) {
	    int k = 0;
	    while (key != node->key[k]) {
	    	k++;
		}
	    for (int i = k; i < node->keyNum - 1; i++) {
	    	node->key[i] = node->key[i + 1];
		}
	    for (int i = k + 1; i < node->keyNum; i++) {
	    	node->pointers[i] = node->pointers[i + 1];
		}
	    node->keyNum--;
	}
	
	// пошук елемента за ключем
	// якщо він невдалий, то повертається NULL
	T* search(int key) {
	    Node* leaf = findLeaf(key);
	    for (int i = 0; i < leaf->keyNum; i++) {
	    	if (key == leaf->key[i]) {
	    		return (T*)leaf->pointers[i + 1];
			}
		}
	    return NULL;
	}
	
	// повертає розмір дерева
	int size() {
		return __size;
	}
	
	// виведення дерева
	void print() {
		cout << "B+-tree (size = " << __size << "):" << endl;
		__print(root);
		cout << endl;
	}
};

// підрозділ компанії
class CompanyDivision {
	// назва
	string name;
	friend void read();
	
public:
	
	// конструктор 1
	CompanyDivision() : name("") {
	}
	
	// конструктор 2
	CompanyDivision(const string name) : name(name) {
	}
	
	// рядкове представлення компанії
	operator string() const {
		return name;
	}
};

// вхідні підрозділи
vector <CompanyDivision> compDivs;
vector <int> keys;

// зчитування підрозділів компанії та відповідних ключів
// з файлу test.txt
void read() {
	ifstream fin("test.txt");
	int divN;
	fin >> divN;
	for (int i = 0; i < divN; i++) {
		int key;
		CompanyDivision compDiv;
		fin >> key >> compDiv.name;  
		compDivs.push_back(compDiv);
		keys.push_back(key);
	}
}

// тестування
int main() {
	read();
	BPlusTree <CompanyDivision> bpt;
	for (int i = 0; i < compDivs.size(); i++) {
		bpt.insert(keys[i], &compDivs[i]);
	}
	cout << "Operations: insert - 1, delete - 2, search - 3, exit - 4" << endl << endl;
	bpt.print();
    while (true) {
    	int op;
    	cout << "operation: ";
    	cin >> op;
    	switch (op) {
    		case 1: {
    			cout << "--insertion--" << endl;
    			int key;
    			string name;
    			cout << "key: ";
				cin >> key;
				cout << "company division name: ";
				cin >>  name;
				if (!bpt.insert(key, new CompanyDivision(name))) {
					cout << "The key can not repeat!" << endl;
				} else {
					bpt.print();
				}
    			break;
    		}
    		case 2: {
    			cout << "--deleting--" << endl;
    			int key;
    			cout << "key: ";
				cin >> key;
    			if (!bpt.erase(key)) {
					cout << "There isn't company division with such key!" << endl;
				} else {
					bpt.print();
				}
    			break;
    		}
    		case 3: {
    			cout << "--searching--" << endl;
    			int key;
    			cout << "key: ";
				cin >> key;
				CompanyDivision* compDiv = bpt.search(key);
				if (compDiv == NULL) {
					cout << "Not found" << endl;
				} else {
					cout << "Company division with key = " << key << " is " << (string)*compDiv << endl;
				}
    			break;
    		}
    		case 4: {
    			exit(0);
    		}
    		default: {
				break;
			}
		}
	}
}
