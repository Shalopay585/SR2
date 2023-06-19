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

void parseXMLNode(ifstream& file, Node& node)
{
	// Recursive node parsing
}

Node parseXML(const string& fileName)
{
	ifstream file(fileName);
    if (!file.is_open())
	{
		cerr << "Error: " << fileName << " not found!\n";
		exit(1);
	}

	Node root("root");
	parseXMLNode(file, root);

	if (root.children.size() == 1)
		return root.children[0];
	else
	{
		cerr << "Error: Invalid XML format\n";
		exit(1);
	}
}

main()
{
	


	return 0;
}
