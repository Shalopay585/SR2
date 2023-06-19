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
};

enum State
{
	Text,
	Tag,
	OpenTag,
	CloseTag
};

main()
{
	


	return 0;
}
