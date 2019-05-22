#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

const int INF = 1000 * 1000 * 1000;

// ������ �������� ������
template <typename T>
class BinarySearchTree {
	// l, r - ��������� �� ��� � ����� �������� 
	// key - �������� � ����
	struct Node {
		Node *l, *r;
		T key;
		
		Node(T key = T(), Node* l = NULL, Node* r = NULL) : key(key), l(l), r(l) {
		}
	};
	// �����
	Node* root;
	// ���� ������� ������ �� ������� ��������� ������� (�������)
	int cost;
	
	// ���������� ���������� ������ �� ����� ������ frs � ��������� ������ keys 
	// ��� ������ �� l �� r ��/��� ��������� ���� �������� �������
	// dp[l][r] = 0, ���� ���� ������� �� ����������� � ������ �����������, ������ - �� 
	// �������� ������� �������� ������, ������������ � ������ �� l �� r �������
	// frs, keys - ������� � ������� �����
	// ������� ������� ���������� ���������� ������ ��� �������� ��������
	Node* buildOptimalDP(int l, int r, bool create, vector < vector <int> >& dp, vector <int>& frs, vector <T>& keys) {
		// ������ ���������� � ������ �����
		if (l == r) {
			dp[l][r] = frs[l];
			return create ? new Node(keys[l], NULL, NULL) : NULL;
		}
		// ���������� ���� ������
		int s = 0;
		for (int i = l; i <= r; i++) {
			s += frs[i];
		}
		// ��������� ���������� ������� �� �������, ��� ������� ������ ������� ������
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
		// ����������� ���������� �������
		dp[l][r] = mn + s;
		if (!create) {
			return NULL;
		}
		// ������ ������ �� ������� �������� �� �����, ���� ���������
		Node* node = new Node(keys[mni]);
		node->l = l == mni ? NULL : buildOptimalDP(l, mni - 1, true, dp, frs, keys);
		node->r = r == mni ? NULL : buildOptimalDP(mni + 1, r, true, dp, frs, keys);
		return node;
	}
	
	// ���������� ���������� ������ �� ����� ������ frs � ��������� ������ keys
	// ������� ������� �������� ������� ������, ��� ����� �������
	int buildOptimal(vector <int>& frs, vector <T>& keys) {
		vector < vector <int> > dp(frs.size());
		for (auto& v : dp) {
			v.assign(frs.size(), 0);
		}
		root = buildOptimalDP(0, frs.size() - 1, true, dp, frs, keys);
		return dp[0][frs.size() - 1];
	}
	
	// ��������� ������ (���������� �������)
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
	// ����������� 1
	BinarySearchTree() : root(NULL) {
	}	
	
	// ����������� 2
	BinarySearchTree(vector <int> frs, vector <T> keys) : BinarySearchTree() {
		cost = buildOptimal(frs, keys);
	}
		
	// ��������� ������
	void show() {
		cout << "[tree]" << endl;
		cout << "cost = " << cost << endl;
		show(root, 0, '*');
	}
	
};

// ������� ������
class CompanyDivision {
	// �����
	string name;
	friend void read(); 
	
public:
	// ������� ������������� ������
	operator string() {
		return name;
	}
};

typedef pair <CompanyDivision, int> CInfo;

// ������ ��� �������: ������� ��������� �� �������� - ��� �������
vector <CInfo> compDivsInfo; 

// ���������� �������� ������ �� �� �������� ������
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

// ����������
int main() {
	read();
	// ���������� �������� �� ���������� �������
	sort(compDivsInfo.begin(), compDivsInfo.end(), 
		[](CInfo a, CInfo b) { return (string)a.first < (string)b.first; }); 
	vector <int> frs;
	vector <CompanyDivision> compDivs;
	for (auto it : compDivsInfo) {
		compDivs.push_back(it.first);
		frs.push_back(it.second);
	}
	// �������� ������������ ������ �������� ������ �� ����� ������ � ������
	BinarySearchTree <CompanyDivision> bst(frs, compDivs);
	bst.show();
}
