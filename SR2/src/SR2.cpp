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

void compareXML(Node &first, Node &second, Node &diff)
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
        if (j < second.children.size() && first.children[i].tag == second.children[j].tag)
        {
            Node childDiff;
            compareXML(first.children[i], second.children[j], childDiff);
            if (!childDiff.tag.empty() || !childDiff.text.empty() || !childDiff.children.empty())
            {
                diff.tag = first.tag;
                diff.children.push_back(childDiff);
            }
            ++i;
			if (j != second.children.size() - 1)
            	++j;
        }
        else
        {
            bool tagFound = false;
            int foundIndex = -1;
            for (int k = j; k < second.children.size(); ++k)
            {
                if (first.children[i].tag == second.children[k].tag)
                {
                    tagFound = true;
                    foundIndex = k;
                    break;
                }
            }

            if (tagFound && foundIndex > j)
            {
                diff.tag = first.tag;
                Node child("[file2]");
                child.children.push_back(second.children[j]);
                child.children.push_back(first.children[j]);
                diff.children.push_back(child);
                ++j;
            }
            else
            {
                tagFound = false;
                foundIndex = -1;
                for (int k = i; k < first.children.size(); ++k)
                {
                    if (second.children[j].tag == first.children[k].tag)
                    {
                        tagFound = true;
                        foundIndex = k;
                        break;
                    }
                }
				
                if (tagFound && foundIndex > i)
                {
                    diff.tag = first.tag;
                    Node child("[file1]");
					child.children.push_back(second.children[i]);
                    child.children.push_back(first.children[i]);
                    diff.children.push_back(child);
                    ++i;
                }
                else
                {
                    diff.tag = first.tag;
                    Node child("[file2]");
                    child.children.push_back(second.children[j]);
					child.children.push_back(first.children[j]);
                    diff.children.push_back(child);
                    ++j;
                }
            }
        }
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
