#include <iostream>
#include <vector>

struct node // структура для представления узлов дерева
{
	int key;
	unsigned char height;
	node* left;
	node* right;
	node(int k) { key = k; left = right = 0; height = 1; }
};

unsigned char height(node* p)
{
	return p ? p->height : 0;
}

int bfactor(node* p)
{
	return height(p->right) - height(p->left);
}

void fixheight(node* p)
{
	unsigned char hl = height(p->left);
	unsigned char hr = height(p->right);
	p->height = (hl > hr ? hl : hr) + 1;
}

node* rotateright(node* p) // правый поворот вокруг p
{
	node* q = p->left;
	p->left = q->right;
	q->right = p;
	fixheight(p);
	fixheight(q);
	return q;
}

node* rotateleft(node* q) // левый поворот вокруг q
{
	node* p = q->right;
	q->right = p->left;
	p->left = q;
	fixheight(q);
	fixheight(p);
	return p;
}

node* balance(node* p) // балансировка узла p
{
	fixheight(p);
	if (bfactor(p) == 2)
	{
		if (bfactor(p->right) < 0)
			p->right = rotateright(p->right);
		return rotateleft(p);
	}
	if (bfactor(p) == -2)
	{
		if (bfactor(p->left) > 0)
			p->left = rotateleft(p->left);
		return rotateright(p);
	}
	return p; // балансировка не нужна
}

node* insert(node* p, int k) // вставка ключа k в дерево с корнем p
{
	if (!p) return new node(k);
	if (k < p->key)
		p->left = insert(p->left, k);
	else
		p->right = insert(p->right, k);
	return balance(p);
}

node* findmin(node* p) // поиск узла с минимальным ключом в дереве p 
{
	return p->left ? findmin(p->left) : p;
}

node* removemin(node* p) // удаление узла с минимальным ключом из дерева p
{
	if (p->left == 0)
		return p->right;
	p->left = removemin(p->left);
	return balance(p);
}

node* remove(node* p, int k) // удаление ключа k из дерева p
{
	if (!p) return 0;
	if (k < p->key)
		p->left = remove(p->left, k);
	else if (k > p->key)
		p->right = remove(p->right, k);
	else //  k == p->key 
	{
		node* q = p->left;
		node* r = p->right;
		delete p;
		if (!r) return q;
		node* min = findmin(r);
		min->right = removemin(r);
		min->left = q;
		return balance(min);
	}
	return balance(p);
}

void rec_print(
	std::ostream& out,
	node* node,
	std::vector<bool>& levels,
	int32_t depth,
	uint32_t l,
	char spacer,
	char down,
	char cross
)
{

	for (int32_t i = 0; i < depth; ++i) {
		if (i != depth - 1) {
			if (levels[i]) {
				out << down;
			}
			else {
				out << " ";
			}
			for (uint32_t x = 0; x < l; ++x) {
				out << " ";
			}
		}
		else {
			out << cross;
		}
	}

	if (depth) {
		for (uint32_t i = 0; i < l; ++i) {
			out << spacer;
		}
	}

	if (node) {
		
		out << node->key << std::endl;

		levels.push_back(true);
		rec_print(out, node->left, levels, depth + 1, l, spacer, down, cross);
		levels[levels.size() - 1] = false;
		rec_print(out, node->right, levels, depth + 1, l, spacer, down, cross);
		levels.erase(--levels.end());
	}
	else {
		out << std::endl;
	}
}

int main() {
	std::vector<int> nums;
	node* root = nullptr;

	for (int i = 0; i < 1000*1000; ++i) {
		root = insert(root, i);
	}

	//std::vector<bool> levels;
	//rec_print(std::cout, root, levels, 0, 3, '-', '|', '+');

	int x;
	std::cin >> x;

	return 0;
}