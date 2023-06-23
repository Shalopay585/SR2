#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
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
	void editXML(Node& root, string& tag, const string& value, int& count, vector<Node>& tags, vector<string*>& text_ptr, bool& worked);
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

	cout << "\nThe tag was updated successfully!";

	tags.clear();
	text_ptr.clear();
}

void addTagToFile(Node& root, const string& tag, const string& value)
{
	Node newNode(tag);
	newNode.text = value;
	root.children.push_back(newNode);
	cout << "\nAdded tag <" << tag << ">" << endl;
}

void Node::editXML(Node& root, string& tag, const string& value, int& count, vector<Node>& tags, vector<string*>& text_ptr, bool& worked)
{
	if (count >= 2)
	{
		if (!worked)
		{
			if (root.tag == tag)
			{
				tags.push_back(root);
				text_ptr.push_back(&root.text);
			}

			for (Node& child : root.children)
				child.editXML(child, tag, value, count, tags, text_ptr, worked);
		}
	}
	else
		if (!root.children.empty())
			for (Node& child : root.children)
			{
				worked = true;
				child.editXML(child, tag, value, count, tags, text_ptr, worked);
			}
		else if (root.children.empty())
			if (root.tag == tag)
			{
				worked = true;
				root.text = value;
			}
}

void menu(Node& root, vector<Node>& tags, vector<string*>& text_ptr, static bool worked)
{
	int choice, returning;
	string newTag, newValue;

	do
	{
		cout << "1 - Edit tags\n2 - Save file\n3 - Compare two files\n4 - Print file\n5 - Exit\n\nYour choice: ";
		cin >> choice;

		switch (choice)
		{
		case 1:
			root.printTree();

			cout << "\n\nEnter a tag to edit: ";
			cin.ignore();
			getline(cin, newTag);
			cout << "\nEnter a new value for the tag: ";
			getline(cin, newValue);
			{
				int count = root.counting(root, newTag);
				if (count < 1)
					addTagToFile(root, newTag, newValue);
				else
				{
					root.editXML(root, newTag, newValue, count, tags, text_ptr, worked);
					if (!worked)
						tagEdit(root, tags, newValue, text_ptr);
					else
						cout << "\nThe tag was updated successfully!";
				}

			}
			Sleep(3000);
			system("cls");

			break;
		case 2:
			saveXML(root, "test.xml");

			Sleep(3000);
			system("cls");

			break;
		case 3:

			//Here will be compare function

			break;
		case 4:

			root.printTree();

			cout << endl << "Enter 0 to return to menu: ";
			cin >> returning;

			if (returning == 0)
				system("cls");

			break;
		case 5:
			break;
		}
	} while (choice != 5);
}

int main()
{
	Node root = parseXML("test.xml");
	vector<string*> text_ptr;
	vector<Node> tags;
	bool worked = false;

	menu(root, tags, text_ptr, worked);

	return 0;
}
