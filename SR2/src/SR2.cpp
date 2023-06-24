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

	Node(const string &tag) : tag(tag) {}
	Node(const string &tag, const string &text) : tag(tag), text(text) {}


	void printTree(const string &tab) const;
	void saveNodeToFile(ofstream &file, const string &tab) const;
	int counting(string &tag);
	void editXML(string &tag, const string &value, int &count, vector<Node *> &tags);
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
		file << text;

	if (!children.empty())
	{
		file << endl;
		for (const Node &child : children)
			child.saveNodeToFile(file, tab + '\t');

		file << tab;
	}

	file << "</" << tag << ">" << endl;
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

int Node::counting(string &tag)
{
	int count = 0;

	for (Node &child : children)
		count += child.counting(tag);

	if (this->tag == tag || this->tag.empty())
		count++;

	return count;
}

void tagEdit(vector<Node *> &tags, const string &value)
{
	int choice;

	std::cout << "Choose a tag to edit: ";

	for (int i = 0; i < tags.size(); i++)
	{
			std::cout << "\n\n" << i + 1 << endl;
			tags[i]->printTree();
	}

	std::cout << "\n\nYour choice: ";
	std::cin >> choice;

	tags[choice - 1]->text = value;

	std::cout
		<< "\nThe tag was updated successfully!";

	tags.clear();
}

void addTagToFile(Node &root, vector<Node *> &tags, const string &newTag, const string &value)
{
	std::cout << "There is no such tag. We will add a new one.\n\n";
	root.printTree();

	string tagInWhichToAdd;
	int tagCount;

	do
	{
		std::cout << "\nChoose the tag, in which we will add a new one: ";
		std::cin.ignore();
		getline(std::cin, tagInWhichToAdd);
		for (char& ch : tagInWhichToAdd)
		{
			ch = tolower(ch);
		}
		tagCount = root.counting(tagInWhichToAdd);
	} while (tagCount < 1);

	root.editXML(tagInWhichToAdd, value, tagCount, tags);

	if (tags.size() == 1)
	{
		tags[0]->children.push_back(Node(newTag, value));
		tags[0]->text.clear();
	}
	else
	{
		int choice;

		for (int i = 0; i < tags.size(); i++)
		{
			std::cout << "\n\n" << i + 1 << endl;
			tags[i]->printTree();
		}

		std::cout << "\n\nYour choice: ";
		std::cin >> choice;

		tags[choice - 1]->children.push_back(Node(newTag, value));
		tags[choice - 1]->text.clear();
	}

	std::cout << "\nThe tag was added successfully!";

	tags.clear();
}

void Node::editXML(string &tag, const string &value, int &count, vector<Node *> &tags)
{
	if (this->tag == tag)
	{
		tags.push_back(this);
	}
	for (Node &child : children)
	{
		child.editXML(tag, value, count, tags);
	}
}

void menu(Node &root, bool worked)
{
	int choice = 0, count = 0;
	string newTag, newValue, fileName;
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

			std::cout << "\n\nEnter a tag to edit: ";
			std::cin.ignore();
			getline(std::cin, newTag);
			for (char& ch : newTag)
			{
				ch = tolower(ch);
			}
			std::cout << "\nEnter a new value for the tag: ";
			getline(std::cin, newValue);

			system("cls");

			count = root.counting(newTag);
			if (count < 1)
				addTagToFile(root, tags, newTag, newValue);
			else if (count == 1)
			{
				root.editXML(newTag, newValue, count, tags);
				tags[0]->text = newValue;
				std::cout << "\nThe tag was updated successfully!";
			}
			else
			{
				root.editXML(newTag, newValue, count, tags);
				tagEdit(tags, newValue);
				std::cout << "\nThe tag was updated successfully!";
			}

			Sleep(3000);
			system("cls");

			break;
		case 2:
			system("cls");
			std::cout << "Enter the name of the file to save in (without .xml): ";
			std::cin.ignore();
			getline(std::cin, fileName);
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
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	Node root = parseXML("test.xml");
	bool worked = false;

	menu(root, worked);

	return 0;
}
