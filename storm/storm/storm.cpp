/*	Name: Phillip Pham
	Course: CSE310, Section: 84794
	Instructor: Dr. Violet R. Syrotiuk

	Program Title: storm
	Program Description: This program is designed to manage storm event data and answer queries based on storm data given in a comma
			separated values file (.csv). These .csv files contain storm data, and fatality data based on the storm events in the
			given year. It does this by reading in the files, placing all the storm events in separate arrays based on the respective
			year, and then indexing them into a hash table as linked list entries. Therefore, we can do queries such as finding an
			event with a specific ID much faster than it would take to manually search through an entire array.

	Test cases have been provided in the project PDF and on Canvas.

	Extra resources used for building this application:
		https://cs.nyu.edu/courses/fall17/CSCI-UA.0102-001/Notes/HashTableImplementation.html
		http://www.cplusplus.com/doc/tutorial/structures/
		http://www.cplusplus.com/reference/sstream/
		http://www.cplusplus.com/reference/sstream/stringstream/
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "defn.h"
#include <math.h>
using namespace std;

// Needed to make this a global variable since the hash table class I created requires it
//		the hash table is only created after the .csv's are parsed so I do not need to initialize it yet.
int tableSize;

/*	Creating a class to simplify the fatality_event linked lists seemed like the way to go, I have set up multiple constructors to support
 *	different ways of initializing the list. The supported functions are as follows:
 *		insert() --> Allows to insert another fatality_event where we know that the head != NULL
 */
class FatalityList 
{
	public:
		fatality_event *head;

		FatalityList() 
		{
			head = NULL;
		}

		FatalityList(int eventID) 
		{
			fatality_event *temp = new fatality_event;

			temp->fatality_id = 0;
			temp->event_id = eventID;
			temp->fatality_type = ' ';
			temp->fatality_date = " ";
			temp->fatality_age = 0;
			temp->fatality_sex = ' ';
			temp->fatality_location = " ";
			temp->next = NULL;

			head = temp;
		}

		FatalityList(int fatalityID, int eventID, char fatalityType, string fatalityDate, int fatalityAge, char fatalitySex, string fatalityLocation)
		{
			fatality_event *temp = new fatality_event;

			temp->fatality_id = fatalityID;
			temp->event_id = eventID;
			temp->fatality_type = fatalityType;
			temp->fatality_date = fatalityDate;
			temp->fatality_age = fatalityAge;
			temp->fatality_sex = fatalitySex;
			temp->fatality_location = fatalityLocation;
			temp->next = NULL;

			head = temp;
		}

		void insert(int fatalityID, int eventID, char fatalityType, string fatalityDate, int fatalityAge, char fatalitySex, string fatalityLocation) 
		{
			fatality_event *temp = new fatality_event;

			temp->fatality_id = fatalityID;
			temp->event_id = eventID;
			temp->fatality_type = fatalityType;
			temp->fatality_date = fatalityDate;
			temp->fatality_age = fatalityAge;
			temp->fatality_sex = fatalitySex;
			temp->fatality_location = fatalityLocation;

			if (head != NULL) 
			{
				temp->next = head;
				head = temp;
			}
			else 
			{
				head = temp;
				head->next = NULL;
			}
		}
};

/*	I went ahead and created a class for our hash table, seeing as this would be the least confusing way of creating
 *		a hash table of hash_table_entry structs.
 *	Functions inside the class include:
 *		find() --> Finds the storm_event related to the eventID by finding the event_index stored in our hash_table_entry,
 *					and then prints all of the data of the element in the original array.
 *		hashFunction() --> Calculates our hashkey to find out where an entry should be stored in our table.
 *		insert() --> Inserts a hash_table_entry struct into our hash table based on the hash key using hashFunction(),
 *						but also handles collisions by inserting entries into linked lists (every entry is a linked list).
 *		getValues() --> Just made this function so that it's easier to print out all values of the hash table,
 *							mainly useful for troubleshooting.
 *		countSingleChain() --> This function counts how many entries in a chain there are in a given index of the hash table.
 *		getMaxChainCount() --> This function finds the max length of chains in the entire hash table.
 *		summary() --> Prints out an entire summary of relevant info for the hash table, including load factor.
 */
class HashTable 
{
	public:
		hash_table_entry **T;

		HashTable()
		{
			T = new hash_table_entry*[tableSize];

			for (int i = 0; i < tableSize; i++)
			{
				T[i] = NULL;
			}
		}

		int hashFunction(int key)
		{
			return key % tableSize;
		}

		bool find(int eventID, annual_storms &stormYear) 
		{
			int hash = 0;

			hash = eventID % tableSize;

			bool found = false;

			hash_table_entry *entry = T[hash];

			if (entry != NULL) 
			{
				while (entry != NULL) 
				{
					if (entry->event_id == eventID) 
					{
						found = true;
						int index = entry->event_index;

						cout << "Event ID: " << stormYear.events[index].event_id << endl;
						cout << "State: " << stormYear.events[index].state << endl;
						cout << "Year: " << stormYear.events[index].year << endl;
						cout << "Month: " << stormYear.events[index].month_name << endl;
						cout << "Event Type: " << stormYear.events[index].event_type << endl;
						cout << "County/Zone Type: " << stormYear.events[index].cz_type << endl;
						cout << "County/Zone Name: " << stormYear.events[index].cz_name << endl;
						cout << "Injuries (Direct) : " << stormYear.events[index].injuries_direct << endl;
						cout << "Injuries (Indirect): " << stormYear.events[index].injuries_indirect << endl;
						cout << "Deaths (Direct): " << stormYear.events[index].deaths_direct << endl;
						cout << "Deaths (Indirect): " << stormYear.events[index].deaths_indirect << endl;
						cout << "Property Damage: $" << stormYear.events[index].damage_property << endl;
						cout << "Crops Damage: $" << stormYear.events[index].damage_crops << endl;
						cout << "Enhanced Fujita Scale: E" << stormYear.events[index].tor_f_scale << endl;
						
						fatality_event *temp = stormYear.events[index].f;

						while (temp != NULL)
						{
							if (temp->fatality_id == 0) 
							{
								cout << "\n\tNo fatalities." << endl;
							}
							else
							{
								cout << "\n\t" << "Fatality ID: " << temp->fatality_id << endl;
								cout << "\t" << "Event ID: " << temp->event_id << endl;
								cout << "\t" << "Fatality Type: " << temp->fatality_type << endl;
								cout << "\t" << "Fatality Date: " << temp->fatality_date << endl;
								cout << "\t" << "Fatality Age: " << temp->fatality_age << endl;
								cout << "\t" << "Fatality Sex: " << temp->fatality_sex << endl;
								cout << "\t" << "Fatality Location: " << temp->fatality_location << endl;
							}

							temp = temp->next;
						}
					}

					entry = entry->next;
				}
			}

			return found;
		}

		int findEventIndex(int eventID) 
		{
			int hash = hashFunction(eventID);
			int index = 0;

			hash_table_entry *entry = T[hash];

			if (entry != NULL)
			{
				while (entry != NULL)
				{
					if (entry->event_id == eventID)
					{
						index = entry->event_index;
						return index;
					}

					entry = entry->next;
				}
			}

			return index;
		}

		void insert(int event_id, int year, int event_index)
		{
			int hash = hashFunction(event_id);

			hash_table_entry* p = NULL;
			hash_table_entry* entry = T[hash];
			
			while (entry != NULL)
			{
				p = entry;
				entry = entry->next;
			}
			if (entry == NULL)
			{
				entry = new hash_table_entry(event_id, year, event_index);

				if (p == NULL)
				{
					T[hash] = entry;
				}
				else
				{
					p->next = entry;
				}
			}
			else
			{
				entry->event_id = event_id;
				entry->year = year;
				entry->event_index = event_index;
			}
		}

		string getValues(int index) 
		{
			string output = "";
			hash_table_entry* p = T[index];

			if(T[index] != NULL)
			{
				while (p != NULL)
				{
					output += "Event ID: " + to_string(p->event_id) + "\n";
					output += "Event Index: " + to_string(p->event_index) + "\n";
					output += "Year: " + to_string(p->year) + "\n";
					output += "\n";

					p = p->next;
				}
			}
			else 
			{
				output += "NULL";
				output += "\n";
			}
			
			return output;
		}

		int countSingleChain(int index)
		{
			hash_table_entry *p = T[index];

			if (p != NULL && p->next == NULL)
			{
				return 1;
			}
			else if (p == NULL)
			{
				return 0;
			}
			else
			{
				int count = 1;

				while (p != NULL)
				{
					if (p->next != NULL)
					{
						count++;

						p = p->next;
					}

					p = p->next;
				}

				return count;
			}
		}

		int getMaxChainCount() 
		{
			int maxChainSize = 0;
			int index = 0;

			for (int i = 0; i < tableSize; i++)
			{
				int currentMax = countSingleChain(i);

				if (currentMax > maxChainSize)
				{
					maxChainSize = currentMax;
				}
			}

			return maxChainSize;
		}		

		string summary() 
		{
			string output;
			int maxChain = getMaxChainCount() + 1;
			int *chains = new int[maxChain];
			int chainsNotZero = 0;

			output += "Table size: " + to_string(tableSize) + "\n";
			output += "Max chain length: " + to_string(getMaxChainCount()) + "\n";

			// Initialize all elements in the array to zero first
			for (int i = 0; i < maxChain; i++)
			{
				chains[i] = 0;
			}

			int n = 0;
			int index = 0;

			while (n <= maxChain) 
			{
				index = 0;

				while (index < tableSize) 
				{
					int count = countSingleChain(index);

					if (count == n) 
					{
						chains[n] = chains[n] + 1;
					}

					index++;
				}

				n++;
			}

			// Print the number of chains for the respective number of chain up to the max chain
			for (int i = 0; i < maxChain; i++)
			{
				output += "Chains of length " + to_string(i) + ": " + to_string(chains[i]) + "\n";
			}

			// Calculate how many chains that are not of length zero
			for (int i = 1; i < maxChain; i++)
			{
				chainsNotZero += chains[i];
			}

			// Formula for computing load factor is the (# entries) / (table capacity), should be some number >1.
			double loadFactor = ((double)chainsNotZero / (double)tableSize);

			output += "Amount of chains with length greater than zero: " + to_string(chainsNotZero) + "\n";
			output += "Amount of NULL/unfilled entries in hash table: " + to_string(chains[0]) + "\n";
			output += "Load factor of the hash table: " + to_string(loadFactor) + "\n";

			return output;
		}
};

class MaxHeap
{
	private:
		int size = 0;
		int totalHeight = 0;
		int heightLeft = 0;
		int heightRight = 0;

	public:
		storm_event * H;

		MaxHeap(int length) 
		{
			H = new storm_event[length];
			size = length;
		}

		void buildMaxHeap(string type)
		{
			for (int i = (size / 2); i >= 0; i--)
			{
				max_heapify(i, type);
			}
		}

		void max_heapify(int i, string type)
		{
			int left = (2 * i) + 1;
			int right = (2 * i) + 2;
			int largest = i;

			int damageLeft = 0;
			int damageRight = 0;
			int damageIndex = 0;
			int damageLargest = 0;

			if (type.find("damage_crops") != string::npos) 
			{
				damageLeft = H[left].damage_crops;
				damageRight = H[right].damage_crops;
				damageIndex = H[i].damage_crops;
				damageLargest = 0;
			}
			else if (type.find("damage_property") != string::npos)
			{
				damageLeft = H[left].damage_property;
				damageRight = H[right].damage_property;
				damageIndex = H[i].damage_property;
				damageLargest = 0;
			}
			else if (type.find("fatality") != string::npos) 
			{
				if (left < (size - 1) && right < (size - 1)) 
				{
					fatality_event *leftTemp = H[left].f;
					fatality_event *rightTemp = H[right].f;

					while (leftTemp != NULL)
					{
						if (leftTemp->fatality_id != 0)
						{
							damageLeft++;
						}

						leftTemp = leftTemp->next;
					}

					while (rightTemp != NULL)
					{
						if (rightTemp->fatality_id != 0)
						{
							damageRight++;
						}

						rightTemp = rightTemp->next;
					}
				}
				
				fatality_event *indexTemp = H[i].f;

				while (indexTemp != NULL)
				{
					if (indexTemp->fatality_id != 0) 
					{
						damageIndex++;
					}

					indexTemp = indexTemp->next;
				}				
			}

			if (left <= (size - 1) &&  damageLeft > damageIndex)
			{
				largest = left;
				damageLargest = damageLeft;
			}
			else
			{
				largest = i;
				damageLargest = damageIndex;
			}

			if (right <= (size - 1) && damageRight > damageLargest)
			{
				largest = right;
				damageLargest = damageRight;
			}

			if (largest != i)
			{
				swap(H[i], H[largest]);
	
				max_heapify(largest, type);
			}
		}

		string extractMax(string type)
		{
			int damage = 0;
			string x = "";

			if (type.find("damage_property") != string::npos) 
			{
				damage = H[0].damage_property;

				x = "Event ID: " + to_string(H[0].event_id) + ", Event Type: " + H[0].event_type + ", Amount of damage: $" + to_string(damage) + "\n";
			}
			else if (type.find("damage_crops") != string::npos) 
			{
				damage = H[0].damage_crops;

				x = "Event ID: " + to_string(H[0].event_id) + ", Event Type: " + H[0].event_type + ", Amount of damage: $" + to_string(damage) + "\n";
			}
			else if (type.find("fatality") != string::npos)
			{
				fatality_event *temp = H[0].f;

				while (temp != NULL)
				{
					if (temp->fatality_id == 0)
					{
						x = "No fatalities.";
					}
					else
					{
						string typeFat(1, temp->fatality_type);
						string sex(1, temp->fatality_sex);

						x += "\nFatality ID: " + to_string(temp->fatality_id) + "\n";
						x += "Event ID: " + to_string(temp->event_id) + "\n";
						x += "Fatality Type: " + typeFat + "\n";
						x += "Fatality Date: " + temp->fatality_date + "\n";
						x += "Fatality Age: " + to_string(temp->fatality_age) + "\n";
						x += "Fatality Sex: " + sex + "\n";
						x += "Fatality Location: " + temp->fatality_location;
					}

					temp = temp->next;
				}
			}

			H[0] = H[size - 1];

			size--;

			max_heapify(0, type);

			return x;
		}

		void findSubTreeHeight(int index) 
		{
			int heightLeft = (int)(size / ((2 * index) + 1));
			int heightRight = (int)(size / ((2 * index) + 2));
		}

		int findHeight(int num) 
		{
			return ceil(log2(num + 1)) - 1;
		}

		void summary() 
		{
			findSubTreeHeight(0);

			cout << "The total number of nodes in this max heap is: " << size << endl;
			cout << "The total height of the max heap is: " << findHeight(size) << endl;
			cout << "The total height of the left subtree is: " << heightLeft << endl;
			cout << "The total height of the right subtree is: " << heightRight << endl;
		}
};

class BinarySearchTree 
{
	private:
		int nodeCount = 0;
		int heightRoot = 0;
		int heightLeft = 0;
		int heightRight = 0;

	public:
		bst * root;

		BinarySearchTree() 
		{
			root = NULL;
		}

		bst * insert(bst * node, int eventID, string keyWord) 
		{
			if (node == NULL)
			{
				bst * temp = new bst;
				temp->event_id = eventID;
				temp->s = keyWord;
				temp->left = NULL;
				temp->right = NULL;

				return temp;
			}

			if (keyWord < node->s)
			{
				node->left = insert(node->left, eventID, keyWord);
			}
			else if(keyWord == node->s)
			{
				if (eventID < node->event_id) 
				{
					node->left = insert(node->left, eventID, keyWord);
				}
				else if (eventID > node->event_id)
				{
					node->right = insert(node->right, eventID, keyWord);
				}
			}
			else if(keyWord > node->s)
			{
				node->right = insert(node->right, eventID, keyWord);
			}

			return node;
		}

		int getHeight(bst * root)
		{
			if (root == NULL)
			{
				return 0;
			}
			else
			{
				int leftDepth = getHeight(root->left);
				int rightDepth = getHeight(root->right);

				heightLeft = leftDepth;
				heightRight = rightDepth;

				if (leftDepth > rightDepth)
				{
					return (leftDepth + 1);
				}
				else
				{
					return (rightDepth + 1);
				}
			}
		}

		void inOrder(bst* root, string low, string high, HashTable T, annual_storms * annual, string fieldName, int numYears) 
		{
			string output = "";

			if (root == NULL) 
			{
				return;
			}

			if (low < root->s) 
			{
				inOrder(root->left, low, high, T, annual, fieldName, numYears);
			}

			if (low <= root->s && high >= root->s) 
			{
				int indexInArray = T.findEventIndex(root->event_id);
				int j = 0;

				while (annual[j].events[indexInArray].event_id != root->event_id) 
				{
					j++;
				}

				if (fieldName.find("state") != string::npos) 
				{
					output += "\nState: " + annual[j].events[indexInArray].state + "\n";
					output += "Event ID: " + to_string(annual[j].events[indexInArray].event_id) + "\n";
					output += "Year: " + to_string(annual[j].events[indexInArray].year) + "\n";
					output += "Event Type: " + annual[j].events[indexInArray].event_type + "\n";
					output += "County/Zone Type: ";
					output += annual[j].events[indexInArray].cz_type + "\n";
					output += "County/Zone Name: " + annual[j].events[indexInArray].cz_name + "\n";

					cout << output;
					nodeCount++;
				}
				else 
				{
					output += "\nMonth: " + annual[j].events[indexInArray].month_name + "\n";
					output += "Event ID: " + to_string(annual[j].events[indexInArray].event_id) + "\n";
					output += "Year: " + to_string(annual[j].events[indexInArray].year) + "\n";
					output += "Event Type: " + annual[j].events[indexInArray].event_type + "\n";
					output += "County/Zone Type: ";
					output += annual[j].events[indexInArray].cz_type + "\n";
					output += "County/Zone Name: " + annual[j].events[indexInArray].cz_name + "\n";

					cout << output;
					nodeCount++;
				}
				
			}

			if (high > root->s) 
			{
				inOrder(root->right, low, high, T, annual, fieldName, numYears);
			}
		}

		void summary() 
		{
			cout << "The total number of nodes in this binary search tree: " << nodeCount << endl;
			cout << "Height of the binary search tree from the root: " << getHeight(root) << endl;
			cout << "Height of the left subtree: " << heightLeft << endl;
			cout << "Height of the right subtree: " << heightRight << endl;
		}
};

/* Quick function to convert the damage by corresponding K (thousand) or M (million) label */
int convertDamage(string damage) 
{
	// Set it to this value just in case there is a number, but no K or M.
	double actualDamage = atof(damage.c_str());

	if (damage.find("K") != string::npos) 
	{
		actualDamage = atof(damage.c_str()) * 1000;
	}
	else if (damage.find("M") != string::npos)
	{
		actualDamage = atof(damage.c_str()) * 1000000;
	}

	return (int)actualDamage;
}

/*	This function is from prime.cc, and I'm using it to bruteforce finding a prime number for my hashtable	*/
bool TestForPrime(int val)
{
	int limit, factor = 2;

	limit = (long)(sqrtf((float)val) + 0.5f);

	while ((factor <= limit) && (val % factor))
	{
		factor++;
	}

	return(factor > limit);
}

/*	This function was made so that I could count how many events there are in a .csv file */
int getEventCount(string fileName) 
{
	// This will read the amount of comma separated cells first
	ifstream stream(fileName);
	string line;
	int numEvents = 0;

	// Skip the first line
	getline(stream, line);

	while (getline(stream, line))
	{
		numEvents++;
	}

	stream.close();

	return numEvents;
}

/*	This function does two things: 1) It reads the fatalities related .csv files, 
 *									2) then calls the insert function to create a linked list.
 */
fatality_event * createFatalityLinkedList(int eventID, string year, string fileName) 
{
	ifstream stream(fileName);
	string line;
	int numCells = 0;
	int numIDs = 0;

	getline(stream, line);

	while (getline(stream, line))
	{
		line += ",";

		istringstream s(line);
		string field;

		if(line.find(to_string(eventID)) != string::npos)
		{
			while (getline(s, field, ','))
			{
				numCells++;
			}
		}

		numIDs++;
	}

	stream.close();

	// Here we actually parse the comma separated sells now that we know how many there are
	ifstream streamTwo(fileName);
	string data;
	string * lines = new string[numCells];
	int i = 0;

	// Skip the first line
	getline(streamTwo, line);

	while (getline(streamTwo, data))
	{
		data += ",";

		istringstream s(data);
		string field;		

		if (data.find(to_string(eventID)) != string::npos)
		{
			while (getline(s, field, ','))
			{
				if(field != "")
				{
					lines[i] = field;
					i++;
				}
				else 
				{
					lines[i] = " ";
					i++;
				}
			}
		}
	}

	streamTwo.close();

	// If the storm event has no related fatality ID, then we will just make a default fatality_event list
	if(numCells == 0)
	{
		FatalityList fatList(eventID);

		return fatList.head;
	}
	else // But if there are related fatality IDs, then we will put them into our storm event with related values
	{
		FatalityList fatList;

		for (int i = 0; i < (numCells); i = i + 7)
		{
			int fatalityID = atoi(lines[i].c_str());
			char fatalityType = lines[i + 2].at(0);
			string fatalityDate = lines[i + 3];
			int fatalityAge = atoi(lines[i + 4].c_str());
			char fatalitySex = lines[i + 5].at(0);
			string fatalityLocation = lines[i + 6];

			fatList.insert(fatalityID, eventID, fatalityType, fatalityDate, fatalityAge, fatalitySex, fatalityLocation);
		}

		return fatList.head;
	}
}

/*	This function populates the storm_event arrays that should be located in each annual_storm struct.
 *		We are simultaneously creating our fatality linked lists, as well as populating our hash table.
 *	Then, we return our storm event array and place it inside our annual_storm object for the respective year.
 */
storm_event * readDetails(string fileName, string year, int numEvents, HashTable &table) 
{
	// Here we actually parse the comma separated sells now that we know how many there are
	ifstream streamTwo(fileName);
	string data;
	int num = numEvents * 14;
	string * lines = new string[num];
	int i = 0;

	// Skip the first line
	getline(streamTwo, data);

	while (getline(streamTwo, data))
	{
		data += ",";

		istringstream s(data);
		string field;

		while (getline(s, field, ','))
		{
			if (field != "")
			{
				lines[i] = field;
				i++;
			}
			else
			{
				lines[i] = " ";
				i++;
			}
		}
	}

	streamTwo.close();

	// Now we start filling in these storm events
	storm_event * events = new storm_event[numEvents];
	int r = 0;

	for (int q = 0; q < numEvents; q++)
	{
		events[q].event_id = atoi(lines[r].c_str());
		r++;

		events[q].state = lines[r];

		r++;

		events[q].year = atoi(lines[r].c_str());

		r++;

		events[q].month_name = lines[r];

		r++;

		events[q].event_type = lines[r];

		r++;

		events[q].cz_type = lines[r].at(0);

		r++;

		events[q].cz_name = lines[r];

		r++;

		events[q].injuries_direct = atoi(lines[r].c_str());

		r++;

		events[q].injuries_indirect = atoi(lines[r].c_str());

		r++;

		events[q].deaths_direct = atoi(lines[r].c_str());

		r++;

		events[q].deaths_indirect = atoi(lines[r].c_str());

		r++;

		events[q].damage_property = convertDamage(lines[r]);

		r++;

		events[q].damage_crops = convertDamage(lines[r]);

		r++;

		events[q].tor_f_scale = lines[r];

		r++;

		events[q].f = createFatalityLinkedList(events[q].event_id, year, "fatalities-" + year + ".csv");

		table.insert(events[q].event_id, events[q].year, q);
	}

	return events;
}

int main(int argc, char* argv[])
{
	// Initialize dynamic arrays and variables
	const char** arguments;

	int numArg = argc - 2; // Gets number of arguments that arent the program title and number following it

	int numberOfYears = atoi(argv[1]);

	arguments = new const char*[numArg];

	annual_storms *annual = new annual_storms[numberOfYears];
	int *eventNums = new int[numArg];
	int totalEvents = 0;

	int j = 0;

	// Put the years part of the arguments into its own array
	for (int i = 2; i < argc; i++)
	{
		arguments[j] = argv[i];
		j++;
	}

	// Ensuring that the amount of years specified is equal to the amount of years actually input in command line
	if (j != numberOfYears) 
	{
		cout << "ERROR: Command line argument for amount of years," << j << ", does not match the following input of years: " << endl;

		for (int e = 0; e < j; e++)
		{
			cout << "\t" << arguments[e] << endl;
		}

		cout << "Please re-execute the program with the proper command line syntax: " << endl;
		cout << "ex: './storm 3 1950 1951 1952', where the last three integers represent the years to examine," << endl 
						<< "\tand the first number represents the amount of years to examine.";

		return 0;
	}

	// Checks to see if the years properly fall within range of 1950 - 2019
	for (int i = 0; i < numberOfYears; i++)
	{
		if (atoi(arguments[i]) < 1950 && atoi(arguments[i]) > 2019)
		{
			cout << "ERROR: A year that was input, " << arguments[i] << ", is not within the range 1950-2019." << endl;
			cout << "\tPlease re-execute the program, and specify years that fall within 1950-2019 in the command line arguments." << endl;

			return 0;
		}
	}

	// Convert the years strings into integers
	for (int i = 0; i < (numberOfYears); i++)
	{
		annual[i].year = atoi(arguments[i]);
	}

	// Grab the number of events we are expecting so that we can allocate our respective arrays
	for (int i = 0; i < (numArg); i++)
	{
		string year = arguments[i];
		eventNums[i] = getEventCount("details-" + year + ".csv");
	}

	// Get the total number of events by taking the sum of events from every year
	for (int k = 0; k < (numArg); k++)
	{
		totalEvents += eventNums[k];
	}

	// Find our prime number for our hash table
	int findPrime = 2 * totalEvents;

	while (!TestForPrime(findPrime))
	{
		findPrime += 1;
	}

	// Set our table size for our hash table equal to our prime number
	// NOTE: tableSize is a global variable not local
	tableSize = findPrime;

	// Finally, create our hash table
	HashTable table;
	
	// Go through every required annual .csv file specified, and populate our arrays as well as our hash table
	for (int i = 0; i < (numArg); i++)
	{
		string year = arguments[i];
		annual[i].events = readDetails("details-" + year + ".csv", year, eventNums[i], table);
	}

	// Here we begin the query part of the project
	string queries;

	cout << "Please enter an integer of how many queries you would like to complete:\n";

	do 
	{
		getline(cin, queries);

		// Checking to make sure that the input is actually an integer
		if (!(queries.find_first_not_of("0123456789") == string::npos)) 
		{
			cout << "ERROR: You did not enter a valid integer. Please retry.\n";
		}
	} 
	while (!(queries.find_first_not_of("0123456789") == string::npos));
	
	// Convert the input into an integer representing the number of queries chosen by the user
	int numQueries = atoi(queries.c_str());

	if (numQueries > 0) 
	{
		for (int q = 0; q < numQueries; q++)
		{
			string command;

			cout << "Please enter your query:\n";

			getline(cin, command);

			cout << "----------------------------------------------------------------------------------" << endl;

			// Checks to see if the query is "find entry (some integer)"
			if (command.find("find event") != string::npos) 
			{
				int m = 0;
				stringstream parseID(command);
				string word;
				string * separated = new string[3];

				while (getline(parseID, word, ' ')) 
				{
					separated[m] = word;

					m++;

					if (m > 3) 
					{
						cout << "ERROR: command 'find event' must be proceeded only by 1 integer, ex. 'find event 10096220'.";
						if (q > 0)
						{
							q--;
						}
						else if (q == 0)
						{
							numQueries = numQueries + 1;
						}
						break;
					}
					else if(!(separated[2].find_first_not_of("0123456789") == string::npos))
					{
						cout << "ERROR: The event ID is not an integer." << endl;
						if (q > 0)
						{
							q--;
						}
						else if (q == 0)
						{
							numQueries = numQueries + 1;
						}
						break;
					}
				}

				int n = 0;

				while (!table.find(atoi(separated[2].c_str()), annual[n])) 
				{
					n++;

					if (n > numberOfYears) 
					{
						cout << "Storm event " << separated[2] << " not found." << endl;
						break;
					}
				}

				cout << "----------------------------------------------------------------------------------" << endl;	
			}
			else if (command.find("find max") != string::npos)
			{
				int m = 0;
				int z = 0;
				bool invalid = false;
				stringstream parse(command);
				string word;
				string * separated = new string[5];

				while (getline(parse, word, ' '))
				{
					separated[m] = word;

					m++;

					if (m > 5)
					{
						cout << "ERROR: command 'find max' must be proceeded by a positive integer, a year (may be specific, or all), and a single word notating the type of damage." << endl;
						cout << "\tExamples are as follows:" << endl;
						cout << "\t\tfind max 4 1950 damage_property" << endl;
						cout << "\t\tfind max 10 all damage_crops" << endl;

						if (q > 0)
						{
							q--;
						}
						else if (q == 0)
						{
							numQueries = numQueries + 1;
						}

						invalid = true;

						break;
					}
				}

				// If we are finding the max fatalities
				if (command.find("find max fatality") != string::npos)
				{
					if (separated[4].find("all") != string::npos)
					{
						storm_event * allYears = new storm_event[totalEvents];

						int t = 0;

						for (int k = 0; k < numberOfYears; k++)
						{
							for (int p = 0; p < eventNums[k]; p++)
							{
								allYears[t] = annual[k].events[p];
								t++;
							}
						}

						MaxHeap heap = MaxHeap(totalEvents);

						heap.H = allYears;

						heap.buildMaxHeap(separated[2]);

						for (int i = 0; i < atoi(separated[3].c_str()); i++)
						{
							string output = "";

							output = heap.extractMax(separated[2]);

							cout << output << endl;
						}

						heap.summary();
					}
					else
					{
						int index = 0;
						int year = atoi(separated[4].c_str());

						while (annual[index].year != year)
						{
							index++;
						}

						MaxHeap heap = MaxHeap(eventNums[index]);

						heap.H = annual[index].events;

						heap.buildMaxHeap(separated[2]);

						for (int i = 0; i < atoi(separated[3].c_str()); i++)
						{
							string output = "";

							output = heap.extractMax(separated[2]);

							cout << output << endl;
						}

						heap.summary();
					}
				}
				else if(!invalid)
				{
					if (separated[3].find("all") != string::npos)
					{
						storm_event * allYears = new storm_event[totalEvents];
						int t = 0;

						for (int k = 0; k < numberOfYears; k++)
						{
							for (int p = 0; p < eventNums[k]; p++)
							{
								allYears[t] = annual[k].events[p];
								t++;
							}
						}

						MaxHeap heap = MaxHeap(totalEvents);

						heap.H = allYears;

						heap.buildMaxHeap(separated[4]);

						for (int i = 0; i < atoi(separated[2].c_str()); i++)
						{
							string output = "";

							output = heap.extractMax(separated[4]);

							cout << output << endl;
						}

						heap.summary();
					}
					else
					{
						int index = 0;
						int year = atoi(separated[3].c_str());

						while (annual[index].year != year)
						{
							index++;
						}

						MaxHeap heap = MaxHeap(eventNums[index]);

						heap.H = annual[index].events;

						heap.buildMaxHeap(separated[4]);

						for (int i = 0; i < atoi(separated[2].c_str()); i++)
						{
							string output = "";

							output = heap.extractMax(separated[4]);

							cout << output << endl;
						}

						heap.summary();
					}
				}

				cout << "----------------------------------------------------------------------------------" << endl;
			}
			else if (command.find("range") != string::npos)
			{
					int m = 0;
					stringstream parse(command);
					string word;
					string * separated = new string[5];

					while (getline(parse, word, ' '))
					{
						separated[m] = word;

						m++;

						if (m > 5)
						{
							cout << "ERROR: command 'range' must be proceeded by a valid <year>, <field_name>, <low>, <high>" << endl;
							cout << "\tExamples are as follows:" << endl;
							cout << "\t\trange 1950 state A C" << endl;
							cout << "\t\trange all month_name January January" << endl;

							if (q > 0)
							{
								q--;
							}
							else if (q == 0)
							{
								numQueries = numQueries + 1;
							}

							break;
						}
					}

					BinarySearchTree bst = BinarySearchTree();

					if (separated[1].find("all") != string::npos) 
					{
						storm_event * allYears = new storm_event[totalEvents];
						int t = 0;

						for (int k = 0; k < numberOfYears; k++)
						{
							for (int p = 0; p < eventNums[k]; p++)
							{
								allYears[t] = annual[k].events[p];
								t++;
							}
						}

						if (separated[2].find("month_name") != string::npos) 
						{
							for (int i = 0; i < totalEvents; i++)
							{
								bst.root = bst.insert(bst.root, allYears[i].event_id, allYears[i].month_name);
							}
						}
						else if (separated[2].find("state") != string::npos)
						{
							for (int i = 0; i < totalEvents; i++)
							{
								bst.root = bst.insert(bst.root, allYears[i].event_id, allYears[i].state);
							}
						}
						
						bst.inOrder(bst.root, separated[3], separated[4], table, annual, separated[2], numberOfYears);
						bst.summary();
					}
					else 
					{
						int index = 0;
						int year = atoi(separated[1].c_str());

						while (annual[index].year != year)
						{
							index++;
						}

						if (separated[2].find("month_name") != string::npos)
						{
							for (int i = 0; i < eventNums[index] - 1; i++)
							{
								bst.root = bst.insert(bst.root, annual[index].events[i].event_id, annual[index].events[i].month_name);
							}
						}
						else if (separated[2].find("state") != string::npos)
						{
							for (int i = 0; i < eventNums[index] - 1; i++)
							{
								bst.root = bst.insert(bst.root, annual[index].events[i].event_id, annual[index].events[i].state);
							}
						}

						bst.inOrder(bst.root, separated[3], separated[4], table, annual, separated[2], numberOfYears);
						bst.summary();
					}

					cout << "----------------------------------------------------------------------------------" << endl;
			}
			else 
			{
				// Gives an error if the query entered is not supported, and then prints a list of supported queries
				cout << "ERROR: Supported commands are as follows: \n" << "\t 'find event (any positive integer)'\n";
				cout << "\t 'find max fatality <number of maxes> <year specified OR 'all'>\n";
				cout << "\t 'find max <number of maxes> <year specified OR 'all'> <'damage_type'>\n";
				cout << "\t 'range <year specified OR 'all'> <String low> <String high>\n";
				if (q > 0)
				{
					q--;
				}
				else if (q == 0) 
				{
					numQueries = numQueries + 1;
				}
			}
		}
	}

	// Lets print out a summary of the entire hash table
	cout << table.summary();

	// Exit gracefully
	return 0;
}