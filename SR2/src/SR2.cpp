#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Node
{
public:
	string tag;
	string text;
	vector<Node> children;

	Node(const string &tag) : tag(tag) {}

	void printTree(const string &tab) const;
};

enum State
{
	Text,
	Tag,
	OpenTag,
	CloseTag
};

void parseXMLNode(ifstream &file, Node &node)
{
	State state = State::Text;
	string tag, text;

	char ch;
	while (file.get(ch))
	{
		switch (state)
		{
		case State::Text:
			if (ch == '<')
			{
				state = State::Tag;
				if (!text.empty())
				{
					node.text = text;
					text.clear();
				}
			}
			else
			{
				text += ch;
			}
			break;
		case State::Tag:
			if (ch == '/')
			{
				state = State::CloseTag;
			}
			else
			{
				state = State::OpenTag;
				tag += ch;
			}
			break;
		case State::OpenTag:
			if (ch == '>')
			{
				state = State::Text;
				Node child(tag);
				node.children.push_back(child);
				parseXMLNode(file, node.children.back());
				tag.clear();
			}
			else
			{
				tag += ch;
			}
			break;
		case State::CloseTag:
			if (ch == '>')
			{
				state = State::Text;
				if (tag != node.tag)
				{
					cerr << "Error: invalid closing tag [" << tag
						 << "] for opening tag [" << node.tag << "] !\n";
					exit(1);
				}
				if (node.children.size() > 0)
				{
					node.text.clear();
				}
				return;
			}
			else
			{
				tag += ch;
			}
			break;
		}
	}
}

Node parseXML(const string &fileName)
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
	{
		return root.children[0];
	}
	else
	{
		cerr << "Error: Invalid XML format\n";
		exit(1);
	}
}

void Node::printTree(const string &tab = "") const
{
	string tmpTag = tag;
	tmpTag[0] = toupper(tmpTag[0]);
	cout << tab << tmpTag << ": " << text << endl;
	for (const Node &child : children)
	{
		child.printTree(tab + '\t');
	}
}

int main()
{
	Node root = parseXML("test.xml");
	root.printTree();

	return 0;
}
