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
	void printDiff(const Node& diff, const string& parentTags = "");
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

void compareXML(const Node& first, const Node& second, Node& diff, const string& source1 = "", const string& source2 = "")
{
	 if (first.tag != second.tag || first.text != second.text)
	 {
	        if (diff.tag.empty()) {
	            diff.tag = first.tag;
	        }
	        Node firstCopy = first;
	        Node secondCopy = second;
	        firstCopy.tag = "[file1]";
	        secondCopy.tag = "[file2]";
	        diff.children.push_back(firstCopy);
	        diff.children.push_back(secondCopy);
	        return;
	    }
	 int i = 0; j = 0;
	 while (i < first.children.size() && j < second.children.size())
	 {
	     Node childDiff;
	     compareXML(first.children[i], second.children[j], childDiff, source1, source2);
	     if (!childDiff.tag.empty() || !childDiff.text.empty() || !childDiff.children.empty()) {
	         diff.children.push_back(childDiff);
	     }
	     ++i;
	     ++j;
	 }

}
void printDiff(const Node& diff, const string& indent = "", const string& source = "") {
	bool printAll = true;
    if (!diff.children.empty()) {
    	cout << indent << diff.tag << "" << endl;
        for (const Node& child : diff.children) {
            printDiff(child, indent + "\t", source);
        }else {
            if (printAll) {
                cout << indent << diff.tag << ": " << diff.text;
                if (!source.empty()) {
                    cout << " [" << source << "]";
                }
                cout << endl;
            }
        }
int main()
{
	Node first = parseXML("file1.xml");
	Node second = parseXML("file2.xml");
	Node diff;
	compareXML(first, second, diff);
	cout << "----------------------------------------\n";
	printDiff(diff);


	return 0;
}
