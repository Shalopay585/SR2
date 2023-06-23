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

	Node(const string& tag) : tag(tag) {}

	void printTree(const string& tab) const;
	void saveToXML(ofstream& file, const string& tab) const;
	int counting(Node& root, string& tag);
	void editXML(Node& root, string& tag, const string& value, int& count, vector<Node>& tags, vector<string*>& text_ptr);
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
	{
		return root.children[0];
	}
	else
	{
		cerr << "Error: Invalid XML format\n";
		exit(1);
	}
}

void Node::printTree(const string& tab = "") const
{
	string tmpTag = tag;
	tmpTag[0] = toupper(tmpTag[0]);
	cout << tab << tmpTag << ": " << text << endl;
	for (const Node& child : children)
	{
		child.printTree(tab + '\t');
	}
}

void Node::saveToXML(ofstream& file, const string& tab) const
{
	file << tab << "<" << tag << ">";

	if (!text.empty())
		file << text;

	if (!children.empty())
	{
		file << endl << endl;
		for (const Node& child : children)
			child.saveToXML(file, tab + '\t');

		file << tab;
	}

	file << "</" << tag << ">" << endl << endl;
}

void saveXML(const Node& root, const string& fileName)
{
	ofstream file(fileName);
	if (!file.is_open())
	{
		cerr << "Error: Cannot open " << fileName << "!\n";
		exit(1);
	}

	root.saveToXML(file, "");

	file.close();
	cout << "XML file saved as " << fileName << endl;
}

int Node::counting(Node& root, string& tag)
{
	int count = 0;

	for (Node& child : root.children)
		count += child.counting(child, tag);

	if (root.tag == tag || root.tag.empty())
		count++;

	return count;
}

void tagEdit(Node& root, vector<Node>& tags, const string& value, vector<string*>& text_ptr)
{
	int choice;

	cout << "Choose a tag to edit: ";

	for (int i = 0; i < tags.size(); i++)
		cout << "\n\n" << i + 1 << ". <" << tags[i].tag << ">" << tags[i].text << "</" << tags[i].tag << ">";

	cout << "\n\nYour choice: ";
	cin >> choice;

	*text_ptr[choice - 1] = value;
}

void addTagToFile(Node& root, const string& tag, const string& value)
{
	Node newNode(tag);
	newNode.text = value;
	root.children.push_back(newNode);
	cout << "Added tag <" << tag << ">" << endl;
}

void Node::editXML(Node& root, string& tag, const string& value, int& count, vector<Node>& tags, vector<string*>& text_ptr)
{
	static bool worked = false;

	if (count >= 2)
	{
		if (!worked)
		{
			if (root.tag == tag)
			{
				tags.push_back(root);
				cout << "Added\n";
				text_ptr.push_back(&root.text);
				cout << "Added2\n";
			}

			for (Node& child : root.children)
				child.editXML(child, tag, value, count, tags, text_ptr);
		}

	}
	else if (count < 1)
		addTagToFile(root, "newTag", "New Value");
	else
		if (root.children.empty())
			if (root.tag == tag)
			{
				root.text = value;
				worked = true;
			}
		else if (root.children.size() >= 1)
				for (Node& child : root.children)
				{
					child.editXML(child, tag, value, count, tags, text_ptr);
					worked = true;
				}
}

int main()
{
	vector<string*> text_ptr;

	Node root = parseXML("test.xml");
	root.printTree();

	string editTag = "new";
	string editValue = "337";

	int count = root.counting(root, editTag);

	vector<Node> tags;
	root.editXML(root, editTag, editValue, count, tags, text_ptr);
	/*tagEdit(root, tags, editValue, text_ptr);*/

	saveXML(root, "test.xml");

	return 0;
}
