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
			int hash = hashFunction(eventID);
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
			
		// Deconstructor
		~HashTable()
		{
			delete[] T;
		}
};

/* Quick function to convert the damage by corresponding K (thousand) or M (million) label */
int convertDamage(string damage) 
{
	// Set it to this value just in case there is a number, but no K or M.
	int actualDamage = atoi(damage.c_str());

	if (damage.find("K") != string::npos) 
	{
		actualDamage = atoi(damage.c_str()) * 1000;
	}
	else if (damage.find("M") != string::npos)
	{
		actualDamage = atoi(damage.c_str()) * 1000000;
	}

	return actualDamage;
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
	arguments = new const char*[argc - 3];
	annual_storms *annual = new annual_storms[argc - 3];
	int *eventNums = new int[argc - 3];
	int totalEvents = 0;

	int j = 0;

	// Put the years part of the arguments into its own array
	for (int i = 3; i < argc; i++)
	{
		arguments[j] = argv[i];
		j++;
	}

	// Convert the years strings into integers
	for (int i = 0; i < (argc - 3); i++)
	{
		annual[i].year = atoi(arguments[i]);
	}

	// Grab the number of events we are expecting so that we can allocate our respective arrays
	for (int i = 0; i < (argc - 3); i++)
	{
		string year = arguments[i];
		eventNums[i] = getEventCount("details-" + year + ".csv");
	}

	// Get the total number of events by taking the sum of events from every year
	for (int k = 0; k < (argc - 3); k++)
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
	for (int i = 0; i < (argc - 3); i++)
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
			if (command.find("find") != string::npos) 
			{
				if (command.find("event") != string::npos) 
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

						if (n > (argc - 3)) 
						{
							cout << "Storm event " << separated[2] << " not found." << endl;
							break;
						}
					}

					cout << "----------------------------------------------------------------------------------" << endl;
				}
			} // There are else if's here when the other queries are implemented
			else 
			{
				// Gives an error if the query entered is not supported, and then prints a list of supported queries
				cout << "ERROR: Supported commands are as follows: \n" << "\t 'find event (any positive integer)'\n";
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