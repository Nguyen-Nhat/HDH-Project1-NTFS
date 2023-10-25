#include "NTFS.h"
int main() {

	wstring disk_name;
	cout << "Enter the drive: ";
	wcin >> disk_name;
	disk_name = L"\\\\.\\" + disk_name + L":";
	LPCWSTR disk = disk_name.c_str();
	NTFS c;
	c.Init(disk);
	string commandline = "";
	cin.ignore();
	while (commandline != "exit") {
		_setmode(_fileno(stdout), _O_U8TEXT);
		wcout << c.showPath() << L'>';
		_setmode(_fileno(stdout), _O_TEXT);
		getline(cin, commandline);
		if (commandline == "showBS") {
			c.printBootSector();
		}
		else if (commandline == "showTree") {
			c.printCurrentTree();
		}
		else if (commandline == "showpath") {
			wcout << c.showPath();
		}
		else if (commandline.find("cd") != string::npos) {
			if (commandline == "cd ..") {
				c.backParentDirectory();
			}
			else {
				int pos = commandline.find_first_of("0123456789");
				if (pos != string::npos) {
					string numberString = commandline.substr(pos);
					try {
						int number = stoi(numberString);
						c.changeDirectory(number);
					}
					catch (exception e) {
						cout << "Error\n";
					}
				}
				else {
					cout << "Index hasn't been entered\n";
				}
			}
		}
		else if (commandline == "") {
			cout << "Command line is empty\n";
		}
		else if (commandline == "cls") {
			system("cls");
		}
		else if (commandline == "ls") {
			c.showListChild();
		}
		else if (commandline == "read") {
			c.readFile();
		}
	}
	c.Finish();
}