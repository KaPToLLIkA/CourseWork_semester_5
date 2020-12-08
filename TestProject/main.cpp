#include <iostream>
#include <set>
#include <vector>

struct RecordWithSerialId {
private:
	static int g_id;
	int entity_id;
	char data[196];
public:
	RecordWithSerialId() {
		g_id++;
		this->entity_id = g_id;

		for (int i = 0; i < 196; ++i) {
			this->data[i] = char(rand() % 20 + 66);
		}
		this->data[195] = '\0';
	}

	void print() {
		std::cout << "\n" << this->entity_id;
		for (int i = 0; i < 196; ++i) {
			if (i % 80 == 0)
				std::cout << "\n\t\t";
			std::cout << data[i];
		}
	}
};

int RecordWithSerialId::g_id = 0;

struct RecordWithRandomId {
private:
	static int g_id;
	static const int buffer_size = 100;
	static std::vector<int> bufferized_ids;

	int entity_id;
	char data[196];
public:
	RecordWithRandomId() {
		if (bufferized_ids.empty()) {
			for (int i = 0; i < buffer_size; ++i) {
				bufferized_ids.push_back(++g_id);
			}
		}

		int rand_iterator = rand() % bufferized_ids.size();
		this->entity_id = bufferized_ids[rand_iterator];
		bufferized_ids.erase(bufferized_ids.begin() + rand_iterator);

		for (int i = 0; i < 196; ++i) {
			this->data[i] = char(rand() % 20 + 66);
		}
		this->data[195] = '\0';
	}

	void print() {
		std::cout << "\n" << this->entity_id;
		
	}
};

int RecordWithRandomId::g_id = 0;
std::vector<int> RecordWithRandomId::bufferized_ids;


template <class T>
class A {

public:
	bool compare(T a, T b) {
		return a < b;
	}
};

long long level = 0;

void rec(RecordWithRandomId* a, RecordWithSerialId& b) {
	std::cout << ++level << std::endl;
	rec(a, b);
}

int main() {
	RecordWithRandomId a;
	RecordWithSerialId b;
	rec(&a, b);

}