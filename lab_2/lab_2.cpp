#include <iostream>
#include <vector>

using namespace std;

// червоно-чорне дерево
template <class T>
class RedBlackTree {
	// структура вузла
	// value - значення
	// p1,p2 - вказівники на ліве та праве піддерева
	// red   - колір (true - якщо червоний)
	// childCount - кількість нашадків вершини
	struct Node {
		Node *p1,*p2;
		T& value;
		bool red;
		int childCount;
		
		Node(T& value) : value(value) {
			this->p1=this->p2=0;
			this->red=true;
			this->childCount = 0;
		}
	};
	
	Node *treeRoot;	// корінь
	int size; // розмір
	
	// видалення вершини
	void delItem(Node *node)
	{
		delete node;
	}
	
	// знос дерева (рекурсивна частина)
	void clear(Node *node)
	{
		if(!node) return;
		clear(node->p1);
		clear(node->p2);
		delItem(node);
	}
	
	// виведення дерева, рекурсивна частина
	// \param node вузол
	// \param depth глибина
	// \param dir "*" - корінь, "-" - ліве піддерево, "+" - праве піддерево
	// \code show(root, 0, '*'); \endcode
	void show(Node *node, int depth, char dir)
	{
		int n;
		if(!node) return;
		for(n=0; n<depth; n++) putchar(' ');
		printf("%c[%s] (%s) ch=%d\n", dir,
			((string)node->value).c_str(), node->red?"red":"black", node->childCount);
		show(node->p1,depth+2,'-');
		show(node->p2,depth+2,'+');
	}
	
	// оновити кількість синів вузла
	void updateChildren(Node* node) {
		if (!node) {
			return;
		}
		node->childCount = (node->p1 ? node->p1->childCount + 1 : 0) + 
			               (node->p2 ? node->p2->childCount + 1 : 0);
	}
	
	// поворот вліво
	// \param index індекс вершини
	// \result нова вершина дерева
	Node *rotate21(Node *node)
	{
		Node *p2 =node->p2;
		Node *p21=p2->p1;
		p2->p1=node;
		node->p2=p21;
		updateChildren(node);
		updateChildren(node->p2);
		updateChildren(p2->p1->p2);
		return p2;
	}
	
	// поворот вправо
	// \param index індекс вершини
	// \result нова вершина дерева
	Node *rotate12(Node *node)
	{
		Node *p1 =node->p1;
		Node *p12=p1->p2;
		p1->p2=node;
		node->p1=p12;
		updateChildren(node);
		updateChildren(node->p1);
		updateChildren(p1->p2->p1);
		return p1;
	}
	
	
	// вставка вершини з балансуванням 
	void balanceInsert(Node **root)
	{
		Node *p1,*p2,*px1,*px2;
		Node *node=*root;
		if(node->red) return;
		p1=node->p1;
		p2=node->p2;
		if(p1 && p1->red) {
			px2=p1->p2;				// знайти дві поруч розміщенні червоні вершини
			if(px2 && px2->red) p1=node->p1=rotate21(p1);
			px1=p1->p1;
			if(px1 && px1->red) {
				node->red=true;
				p1->red=false;
				if(p2 && p2->red) {	// перекраска вершин
					px1->red=true;
					p2->red=false;
					return;
				}
				*root=rotate12(node);
			}
		}
		// аналогічно в іншу сторону
		if(p2 && p2->red) {
			px1=p2->p1;				// знайти дві поруч розміщенні червоні вершини
			if(px1 && px1->red) p2=node->p2=rotate12(p2);
			px2=p2->p2;
			if(px2 && px2->red) {
				node->red=true;
				p2->red=false;
				if(p1 && p1->red) {	// перекраска вершин
					px2->red=true;
					p1->red=false;
					return;
				}
				*root=rotate21(node);
			}
		}
		updateChildren(node->p1);
		updateChildren(node->p2);
		updateChildren(node);
	}
	
	// видалення вершини з балансуванням 1
	bool balanceRemove1(Node **root)
	{
		Node *node=*root;
		Node *p1=node->p1;
		Node *p2=node->p2;
		if(p1 && p1->red) {
			p1->red=false;return false;
		}
		if(p2 && p2->red) { // випадок 1
			node->red=true;
			p2->red=false;
			node=*root=rotate21(node);
			if(balanceRemove1(&node->p1)) node->p1->red=false;
			return false;
		}
	 	unsigned int mask=0;
		Node *p21=p2->p1;
		Node *p22=p2->p2;
		if(p21 && p21->red) mask|=1;
		if(p22 && p22->red) mask|=2;
		switch(mask)
		{
		case 0:		// випадок 2 - if((!p21 || !p21->red) && (!p22 || !p22->red))
			p2->red=true;
			return true;
		case 1:
		case 3:		// випадок 3 - if(p21 && p21->red)
			p2->red=true;
			p21->red=false;
			p2=node->p2=rotate12(p2);
			p22=p2->p2; 
		case 2:		// випадок 4 - if(p22 && p22->red)
			p2->red=node->red;
			p22->red=node->red=false;
			*root=rotate21(node);
		}
		return false;
	}
	
	// видалення вершини з балансуванням 1
	bool balanceRemove2(Node **root)
	{
		Node *node=*root;
		Node *p1=node->p1;
		Node *p2=node->p2;
		if(p2 && p2->red) {p2->red=false;return false;}
		if(p1 && p1->red) { // випадок 1
			node->red=true;	
			p1->red=false;
			node=*root=rotate12(node);
			if(balanceRemove2(&node->p2)) node->p2->red=false;
			return false;
		}
	 	unsigned int mask=0;
		Node *p11=p1->p1;
		Node *p12=p1->p2;
		if(p11 && p11->red) mask|=1;
		if(p12 && p12->red) mask|=2;
		switch(mask) {
		case 0:		// випадок 2 - if((!p12 || !p12->red) && (!p11 || !p11->red))
			p1->red=true;
			return true;
		case 2:
		case 3:		// випадок 3 - if(p12 && p12->red)
			p1->red=true;
			p12->red=false;
			p1=node->p1=rotate21(p1);
			p11=p1->p1;
		case 1:		// випадок 4 - if(p11 && p11->red)
			p1->red=node->red;
			p11->red=node->red=false;
			*root=rotate12(node);
		}
		return false;
	}
	
	
	// рекурсивна частина вставки
	// \result true якщо змін не відбулось або балансування в даній вершині не потрібне
	bool insert(T& value,Node **root)
	{
		Node *node=*root;
		if(!node) *root=new Node(value);
		else {
			(*root)->childCount++;
			if(value==node->value) return true; 
			if(insert(value,value<node->value?&node->p1:&node->p2)) return true;
			size++;
			balanceInsert(root);
		}
		return false;
	}
	
	
	// найти та видалити максимальной вузол піддерева
	// \param root корінь дерева
	// \param res елемент, що був видалений
	// \result true якщо потрібне балансування
	bool getMin(Node **root,Node **res)
	{
		Node *node=*root;
		if(node->p1) {
			if(getMin(&node->p1,res)) return balanceRemove1(root);
		} else {
			*root=node->p2;
			*res=node;
			return !node->red;
		}
		return false;
	}
	
	
	// рекурсивна частина видалення
	// \result true якщо потрібен баланс
	bool remove(Node **root,int value)
	{
		Node *t,*node=*root;
		if(!node) return false;
		if(node->value<value) {
			if(remove(&node->p2,value))	return balanceRemove2(root);
		} else if(node->value>value) {
			if(remove(&node->p1,value))	return balanceRemove1(root);
		} else {
			bool res;
			if(!node->p2) {
				*root=node->p1;
				res=!node->red;
				delItem(node);
				return res;
			}
			res=getMin(&node->p2,root);
			t=*root;
			t->red=node->red;
			t->p1=node->p1;
			t->p2=node->p2;
			delItem(node);
			if(res) res=balanceRemove2(root);
			return res;
		}
		return 0;
	}
	
	// знайти к-ий мінімум (рекурсивна частина)
	// \param root корінь дерева
	// \param k порядок статистики
	T& statistic(Node* root, int k) {
		int lc = root->p1 ? root->p1->childCount + 1 : 0; 
		if (k == lc + 1) {
			return root->value;
		}
		if (k <= lc) {
			return statistic(root->p1, k);
		} else {
			return statistic(root->p2, k - lc - 1);
		}
	}
	
	public:
	
	RedBlackTree() {
		size = 0;
		treeRoot = 0;
	}
	
	// вивидення дерева
	void show()
	{
		printf("[tree]\n");
		show(treeRoot,0,'*');
	}
	
	// вставка вузла
	void insert(T& value)
	{
		insert(value,&treeRoot);
		if(treeRoot) treeRoot->red=false;
	}
	
	// видалення вузла
	void remove(int value)
	{
		remove(&treeRoot,value); 
	}
	
	// знос дерева
	void clear()
	{
		clear(treeRoot);
		treeRoot=0;
	}
	
	// знайти к-ий мінімум
	T& statistic(int k) {
		if (k > size) {
			throw "Statistic out of range!";
		}
		statistic(treeRoot, k);
	}
};

// підрозділ компанії
class CompanyDivision {
	string name; // назва
	vector <string> employees; // імена працівників
	friend void readCompanyDivisions();
	
	public:
	
	// рядкове представлення підрозділу	
	operator string() {
		return name;	
	}
	
	// порівняння двох підрозділів
	operator <(CompanyDivision& a) {
		return this->name < a.name;
	}
	
	// перевірка на рівність двох підрозділів
	operator ==(CompanyDivision& a) {
		return this->name == a.name;
	}
};

// обмеження на кількість статистик, що знаходяться
const int MAXK = 100000;

vector <CompanyDivision> compDivs; // список підрозділів компанії
int k, s[MAXK]; // кількість статистик та самі статистики

// зчитування підрозділів компанії та їх працівників
void readCompanyDivisions() {
	int divN;
	cin >> divN;
	for (int i = 0; i < divN; i++) {
		CompanyDivision compDiv;
		cin >> compDiv.name;
		int empNum;
		cin >> empNum;
		for (int i = 0; i < empNum; i++) {
			string empName;
			cin >> empName;
			compDiv.employees.push_back(empName);
		}
		compDivs.push_back(compDiv);
	}
}

// зчитування статистик
void readStatistics() {
	cin >> k;
	for (int i = 0; i < k; i++) {
		cin >> s[i];
	}
}

// тестування 
int main() {
	readCompanyDivisions();
	readStatistics();
	RedBlackTree <CompanyDivision> rbt;
	for (auto& c : compDivs) {
		rbt.insert(c);
	}
	rbt.show();
	for (int i = 0; i < k; i++) {
		cout << "Statistic N" << s[i] << " = " << (string)rbt.statistic(s[i]) << endl;
	}
}

