#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
using namespace std;

enum State
{
	Text,
	Tag,
	OpenTag,
	CloseTag
};

class Node
{
public:
	string tag;
	string text;
	vector<Node> children;

	Node(const string &tag) : tag(tag) {}
	Node(const string &tag, const string &text) : tag(tag), text(text) {}


	void printTree(const string &tab) const;
	void saveNodeToFile(ofstream &file, const string &tab) const;
	int countTags(string &tag);
	void findTags(string &tag, vector<Node *> &tags);
};

void Node::printTree(const string &tab = "") const
{
	string tmpTag = tag;
	tmpTag[0] = toupper(tmpTag[0]);

	std::cout << tab << tmpTag << ": " << text << endl;

	for (const Node &child : children)
	{
		child.printTree(tab + '\t');
	}
}

void Node::saveNodeToFile(ofstream &file, const string &tab = "") const
{
	file << tab << "<" << tag << ">";

	if (!text.empty())
	{
		file << text;
	}
	if (!children.empty())
	{
		file << endl;
		for (const Node &child : children)
		{
			child.saveNodeToFile(file, tab + '\t');
		}
		file << tab;
	}

	file << "</" << tag << ">" << endl;
}

int Node::countTags(string &tag)
{
	int count = 0;

	for (Node &child : children)
		count += child.countTags(tag);

	if (this->tag == tag || this->tag.empty())
		count++;

	return count;
}

void Node::findTags(string &tag, vector<Node *> &tags)
{
	if (this->tag == tag)
	{
		tags.push_back(this);
	}
	for (Node &child : children)
	{
		child.findTags(tag, tags);
	}
}

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

void saveXML(const Node &root, const string &fileName)
{
	ofstream file(fileName);
	if (!file.is_open())
	{
		cerr << "Error: Cannot open " << fileName << "!\n";
		exit(1);
	}

	root.saveNodeToFile(file);

	file.close();
	std::cout << "XML file saved as " << fileName << endl;
}

int chooseTag(vector<Node *> &tags)
{
	int choice;
	std::cout << "Choose a tag: ";

	for (int i = 0; i < tags.size(); i++)
	{
		std::cout << "\n\n" << i + 1 << ".\n";
		tags[i]->printTree();
	}

	std::cout << "\n\nYour choice: ";
	std::cin >> choice;
	return choice;
}

string getString(const string& msg)
{
	string input;

	std::cout << msg;
	std::cin.ignore();
	getline(std::cin, input);
	return input;
}

void tagEdit(vector<Node *> &tags, const string &value)
{
	if (tags.size() == 1)
	{
		tags[0]->text = value;
	}
	else
	{
		int choice = chooseTag(tags);
		tags[choice - 1]->text = value;
	}

	std::cout << "\nThe tag was updated successfully!";
	tags.clear();
}

void addNewTag(Node &root, const string &userTag, const string &value)
{
	string tagInWhichToAdd;
	vector<Node *> tags;

	std::cout << "There is no such tag. We will add a new one.\n\n";
	root.printTree();

	do
	{
		tagInWhichToAdd = getString("\nChoose a tag, in which we will add a new one: ");
		for (char& ch : tagInWhichToAdd)
		{
			ch = tolower(ch);
		}
	} while (root.countTags(tagInWhichToAdd) < 1);

	root.findTags(tagInWhichToAdd, tags);

	if (tags.size() == 1)
	{
		tags[0]->children.push_back(Node(userTag, value));
		tags[0]->text.clear();
	}
	else
	{
		int choice = chooseTag(tags);

		tags[choice - 1]->children.push_back(Node(userTag, value));
		tags[choice - 1]->text.clear();
	}

	std::cout << "\nThe tag was added successfully!";

	tags.clear();
}

void menu(Node &root)
{
	int choice = 0;
	string userTag, userValue, fileName;
	vector<Node *> tags;

	do
	{
		std::cout << "1 - Edit tags\n2 - Save file\n3 - Compare two files\n4 - Print file\n5 - Exit\n\nYour choice: ";
		std::cin >> choice;

		switch (choice)
		{
		case 1:
			system("cls");
			root.printTree();

			userTag = getString("\n\nEnter a tag to edit: ");
			for (char& ch : userTag)
			{
				ch = tolower(ch);
			}
			userValue = getString("\nEnter a new value for the tag: ");

			system("cls");

			if (root.countTags(userTag) < 1)
			{
				addNewTag(root, userTag, userValue);
			}
			else
			{
				root.findTags(userTag, tags);
				tagEdit(tags, userValue);
			}

			Sleep(3000);
			system("cls");

			break;
		case 2:
			system("cls");
			fileName = getString("Enter the name of the file to save in (without .xml): ");
			saveXML(root, fileName + ".xml");

			Sleep(3000);
			system("cls");

			break;
		case 3:

			// Here will be compare function

			break;
		case 4:
			system("cls");
			root.printTree();
			std::cout << "\n\n";

			break;
		case 5:
			break;
		}
	} while (choice != 5);
}

int main()
{
	setlocale(LC_ALL, "rus");
	// SetConsoleCP(1251);
	// SetConsoleOutputCP(1251);

	Node root = parseXML("test.xml");

	menu(root);

	return 0;
}
