#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

class Node
{
public:
	string tag;
	string text;
	vector<Node> children;

	Node(const string& tag) : tag(tag) {}

	void printTree() const;
}

main()
{
	


	return 0;
}
