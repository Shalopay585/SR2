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

	Node() {}
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

void compareXML(const Node &first, const Node &second, Node &diff, const string &source1 = "", const string &source2 = "")
{
	if (first.text != second.text)
	{
		diff.tag = first.tag;
		Node firstCopy = first;
		Node secondCopy = second;
		firstCopy.tag = "[file1]";
		secondCopy.tag = "[file2]";
		diff.children.push_back(firstCopy);
		diff.children.push_back(secondCopy);
		return;
	}
	int i = 0, j = 0;
	while (i < first.children.size() && j < second.children.size())
	{
		if (first.children[i].tag == second.children[i].tag)
		{
			Node childDiff;
			compareXML(first.children[i], second.children[j], childDiff, source1, source2);
			if (!childDiff.tag.empty() || !childDiff.text.empty() || !childDiff.children.empty())
			{
				diff.tag = first.tag;
				diff.children.push_back(childDiff);
			}
			++i;
			++j;
		}
		else
		{
			Node child1("[file1]"), child2("[file2]");
			child1.children.push_back(first.children[i]);
			child2.children.push_back(second.children[i]);
			diff.children.push_back(child1);
			diff.children.push_back(child2);
			diff.tag = first.tag;
			return;
		}
	}
	while (i < first.children.size())
	{
		Node childDiff = first.children[i];
		childDiff.tag += " [" + source1 + "]";
		diff.children.push_back(childDiff);
		++i;
	}

	while (j < second.children.size())
	{
		Node childDiff = second.children[j];
		childDiff.tag += " [" + source2 + "]";
		diff.children.push_back(childDiff);
		++j;
	}
}

int main()
{
	Node first = parseXML("file1.xml");
	Node second = parseXML("file2.xml");
	Node diff;
	compareXML(first, second, diff);
	cout << "----------------------------------------\n";
	diff.printTree();

	return 0;
}
