#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <set>
#include <string>
#include <random>
#include <functional>
#include <algorithm>
#include <utility>
#include <chrono>

enum class Person : int {
	NONE = 0,
	KING = 1, 		// 2314 prince
	PRINCE,			// 1374 king
	DOUBLE,			// 5461 double
	KNIGHT,			// 2512 rebel --
	SOURCERER,		// 4786 knight
	REBEL,			// 4219 sourcerer
	__FIRST = KING,
	__LAST = REBEL
};

bool operator< (Person lsv, Person rsv) {
	return ( (int) lsv ) < ( (int) rsv );
}

struct person_sequential_distribution {
	std::uniform_int_distribution<int> personDistribution;
	std::set<int> used;
	
	person_sequential_distribution() :
		personDistribution((int)Person::__FIRST, (int)Person::__LAST) {}
	
	void reset() {
		used.clear();
	}

	template<class _RandomEngine>
	Person operator() (_RandomEngine engine) {
		int rnd;
		do {
			rnd = personDistribution(engine);
		} while (used.count(rnd));
		
		used.insert(rnd);
		
		return (Person) rnd;
	}
};

struct code_no_repeat_distribution {
	std::uniform_int_distribution<int> codeDistribution;
	std::set<int> used;
	
	code_no_repeat_distribution() :
		codeDistribution(1000,9999) {}
	
	void reset() {
		used.clear();
	}

	template<class _RandomEngine>
	int operator() (_RandomEngine engine) {
		int rnd;
		do {
			rnd = codeDistribution(engine);
		} while (used.count(rnd));
		
		used.insert(rnd);
		
		return rnd;
	}
};

std::string person_str(Person person) {
	switch(person) {
	case Person::KING:
		return "King";
	case Person::PRINCE:
		return "Prince";
	case Person::DOUBLE:
		return "Double";
	case Person::KNIGHT:
		return "Knight";
	case Person::SOURCERER:
		return "Sourcerer";
	case Person::REBEL:
		return "Rebel";
	default:
		return "Seems I've messed this up...\nPlease, call anybody having access to source code and FUCK YOU FIX IT!!111";
	}
}

void end_session() {
	fseek(stdin, 0, SEEK_END);
	std::cout << "Press Enter to continue....";
	getchar();
	system("reset");
}

void start_session() {
	std::cout << "Press Enter....";
	getchar();
	fseek(stdin, 0, SEEK_END);
}


// Round-static part
std::map<int, Person> Persons;
std::map<std::string, Person> Players;
std::map<Person, bool> IsDead;
std::map<Person, bool> IsWinner;

bool PrinceHasSomePoison = true;
bool KingIsCoward = false;

// Round-volatile part
bool KingIsSubstituted = false;

std::string OrderOnKill;
bool OrderConfirmed = false;
Person OrderedTo = Person::NONE;
std::string GotSomePoison;

void dump_players(bool endGame = false) {
	for(auto player : Players) {
		if(player.second == Person::NONE)
			continue;
		std::cout << player.first;
		if(IsDead[player.second])
			std::cout << " (dead)";
		if(endGame) {
			if(IsWinner[player.second])
				std::cout << " (winner)";
			std::cout << " was the " << person_str(player.second);
		}
		std::cout << std::endl;
	}
}

// Resets round-volatile vars
void rise_and_shine() {
	start_session();
	
	KingIsSubstituted = false;

	OrderOnKill.clear();
	OrderConfirmed = false;
	OrderedTo = Person::NONE;
	
	GotSomePoison.clear();
	
	std::cout << "Stats: " << std::endl;
	dump_players();
}

bool is_valid_person(Person p) {
	return ((int) Person::__FIRST) <= ((int) p) && ((int) p) <= ((int) Person::__LAST) && !IsDead[p];
}

bool is_gamestate_ok() {
	int win = 0;
	if(!IsDead[Person::KING]) {
		++win;
		if(	IsDead[Person::REBEL] &&
			IsDead[Person::PRINCE] )
	
			--win, IsWinner[Person::KING] = true;
	}
	
	if(!IsDead[Person::PRINCE]) {
		++win;
		if( IsDead[Person::KING] &&
			IsDead[Person::REBEL] )
			
			--win, IsWinner[Person::PRINCE] = true;
	}
	
	if(!IsDead[Person::DOUBLE]) {
		++win;
		if(	IsDead[Person::REBEL] )
			--win, IsWinner[Person::DOUBLE] = true;
	}
	
	if(!IsDead[Person::KNIGHT]) {
		++win;
		if( IsDead[Person::SOURCERER] )
			--win, IsWinner[Person::KNIGHT] = true;
	}
	
	if(!IsDead[Person::SOURCERER]) {
		++win;
		if( IsDead[Person::KNIGHT] )
			--win, IsWinner[Person::SOURCERER] = true;
	}
	
	if(!IsDead[Person::REBEL]) {
		++win;
		if( IsDead[Person::KING] &&
			IsDead[Person::DOUBLE] )
			--win, IsWinner[Person::REBEL] = true;
	}
	
	// if win is not zero, than there are some unsolved states
	return win;
}

void king_choice() {
	std::string choice;
	
	std::cout << "My king, You can order an execution of anybody in here. "
				 "Please, write his name right here, simply write \"none\" "
				 "to spare life of those silly peasants, or write \"god save the king\" if You are "
				 "sure You are just about to be assasinated." << std::endl
			  << "Here is the list of player names: " << std::endl;
	
	dump_players();
	
	do {
		std::cout << "Please, make your decision: ";
		std::cin >> choice;
	
		if(choice == "none")
			break;
		else if(choice == "god save the king") {
			if(KingIsCoward) {
				std::cout << "Sorry my king, but Your citizen are already aware of Your trick."
							 "You will not be able to do this once again..." << std::endl;
				continue;
			} else if(IsDead[Person::DOUBLE]) {
				std::cout << "Sorry my king, but Your double is dead..." << std::endl;
				continue;
			} else {
				KingIsSubstituted = true;
				KingIsCoward = true;
				break;
			}
		}
		else if(!is_valid_person(Players[choice]))
			std::cout << "My king, it seems You've just mistyped the name of an pesty peasant." << std::endl
					  << "Please, be so kind to try again." << std::endl;
		else {
			OrderOnKill = choice;
			
			std::cout << "Decision made. Now please, say which one of your men would You like to accomplish "
						 "your order, the Sourcerer (S) or the Knight (K): ";
			do {
				char c;
				std::cin >> c;
				switch(c) {
				case 's':
				case 'S':
					OrderedTo = Person::SOURCERER;
					break;
				case 'k':
				case 'K':
					OrderedTo = Person::KNIGHT;
					break;
				default:
					continue;
				}
				break;
			} while(1);
			break;
		}
	} while(1);
	
	std::cout << "Now You only should wait for your men to carry out Your just order." << std::endl
			  << "Please, be so kind to press Enter before You leave" << std::endl;
}

void prince_choice() {
	std::string choice;
	
	std::cout << "My lord, You appear to have some poison you may pour into somebody's soup. "
				 "Well, you may also spare it for the next time simply by writing \"spare\"." << std::endl
			  << "Here is the list of who will be today at the dinner: " << std::endl;
	
	dump_players();
	
	do {
		std::cout << "Please, make your decision: ";
		std::cin >> choice;
	
		if(choice == "spare")
			break;
		else if(!is_valid_person(Players[choice]))
			std::cout << "My lord, are you drunk??" << std::endl
					  << "Please, try once again." << std::endl;
		else {
			GotSomePoison = choice;
			
			std::cout << "Please, be careful not to eat from this plate ;)" << std::endl;
			break;
		}
	} while(1);
	
}

void warrior_choice() {
	
	std::cout << "Sir. Your king has ordered you to kill " << OrderOnKill << ". "
				 "Sure, it will not be good if you rise against your King's will, but still "
				 "you can kill that person ('K'), or spare his life ('S')" << std::endl;
	do {
		std::cout << "Your choice: ";
		char c;
		std::cin >> c;
		switch(c) {
		case 'k':
		case 'K':
			OrderConfirmed = true;
			break;
		case 's':
		case 'S':
			OrderConfirmed = false;
			break;
		default:
			continue;
		}
		break;
	} while(1);
	
	std::cout << "OK" << std::endl;
}

bool first_person_choice(Person person) {
	if(!is_valid_person(person))
		return false;
	
	if(
		// King is to order
		(person == Person::KING) ||
		// Double is to order
		(person == Person::DOUBLE &&
		 IsDead[Person::KING]) ||
		// Prince is to order
		(person == Person::PRINCE &&
		 IsDead[Person::KING] && IsDead[Person::DOUBLE]) )
		
		king_choice();
	 
	std::cout << std::endl << "Nothing more for you, sir" << std::endl;
	
	return true;
}

bool castling_person_choice(Person person) {
	if(!is_valid_person(person))
		return false;
	
	if(
		// Double is to order
		person == Person::DOUBLE &&
		KingIsSubstituted )
		
		king_choice();

	std::cout << std::endl << "Nothing more for you, sir" << std::endl;
	
	return true;
}

bool second_person_choice(Person person) {
	if(!is_valid_person(person))
		return false;

	if(person == OrderedTo && !OrderOnKill.empty())
		warrior_choice();
	
	std::cout << std::endl << "Nothing more for you, sir" << std::endl;
	
	return true;
}

bool dummy_person_choice(Person person) {
	if(!is_valid_person(person))
		return false;

	if(person == Person::PRINCE && PrinceHasSomePoison)
		prince_choice();
	
	std::cout << std::endl << "Nothing more for you, sir" << std::endl;
	
	return true;
}

template<typename Funct>
void user_cycle(Funct f) {
	for(int i = 0; i < 6; ++i) {
		start_session();
		
		int code;
		
		std::cout << "Enter code: ";
		std::cin >> code;
		
		if(! f(Persons[code]) ) {
			std::cout	<< "Error!" << std::endl;
			--i;
		}
		
		end_session();
	}
}

std::string kill_player(std::string player, bool byMagic = false) {
	Person p = Players[player];
	
	if(!is_valid_person(p))
		return "";
	else if(p == Person::PRINCE && byMagic)
		return "";
	else if(KingIsSubstituted && p == Person::DOUBLE) {
		KingIsSubstituted = false;
	} else if(KingIsSubstituted && p == Person::KING) {
		IsDead[Person::DOUBLE]= true;
		std::string reallyKilled = std::find_if(Players.begin(), Players.end(), 
								[](const std::pair<std::string, Person> &t) { return t.second == Person::DOUBLE; } )
									->first;
		return reallyKilled;
	} 
	
	IsDead[p] = true;
	return player;
}

void day_end() {
	if(OrderConfirmed) {
		std::string r = kill_player(OrderOnKill, OrderedTo == Person::SOURCERER);
		if(!r.empty()) {
			start_session();
		
			std::cout << "Message: " << std::endl
					  << "Dear friends, " << OrderOnKill << " was executed to today, at noon, by the order "
						 "of our beloved ruler. Behold" << std::endl;
		}
	}
	if(!GotSomePoison.empty()) {
		std::string r = kill_player(GotSomePoison);
		if(!r.empty()) {
			start_session();
		
			std::cout << "Message: " << std::endl
					  << "Dear friends, " << GotSomePoison << " was found dead in his bedroom, soon after the "
					 	 "dinner. Reason of death: unknown" << std::endl;
		}
	}
	
	start_session();
	std::cout << "There are no unread news, you are free to go to sleep" << std::endl;
	
	end_session();
}

void nightfall() {
	if(!IsDead[Person::REBEL]) {
		int code;
		
		do {
			start_session();
			std::cout << "Enter code: ";
			std::cin >> code;
		
			if(Persons[code] != Person::REBEL) {
				std::cout	<< "Error!" << std::endl;
				end_session();
				continue;
			}
			break;
		} while(1);
		
		dump_players();
		std::string choice;
		do {
			std::cout << "Rebel, enter name of your victim or \"none\": ";
			std::cin >> choice;
		} while(choice != "none" && !is_valid_person(Players[choice]));
		
		if(choice != "none") {
			kill_player(choice);
		
			std::cout << "Most certainly, he is not breathing anymore" << std::endl;
		}
		end_session();
	}
}

int main () {
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 randomGenerator (seed);
	randomGenerator();
	code_no_repeat_distribution codeDistribution;
	person_sequential_distribution personDistribution;
	auto codeGenerator = std::bind(codeDistribution, randomGenerator);
	auto personGenerator = std::bind(personDistribution, randomGenerator);
	
	system("reset");
	
	for(int i = 0; i < 6; ++i) {
		start_session();
		
		
		std::string name;
		std::cout << "Please, write your name: ";
		std::cin >> name;
		
		fseek(stdin, 0, SEEK_END);
		
		Person person = (Person) personGenerator();
		int code 	  = codeGenerator();
		std::cout	<< "You are " << name << ", the " << person_str(person) << std::endl
					<< "Your code is " << code << ". Remember it =)" << std::endl;
		
		Persons[code] = person;
		Players[name] = person;
		
		end_session();
	}
	
	
	while(is_gamestate_ok()) {
		// Reset vars
		rise_and_shine();
		
		std::cout << "First cycle" << std::endl;
		end_session();
		// King (or double, or prince) has to choose who to order kill and to who
		// Also, king may though decide to hide
		user_cycle(first_person_choice);
		
		std::cout << "Second cycle" << std::endl;
		end_session();
		if(KingIsSubstituted)
			// Double may have to make a decision
			user_cycle(castling_person_choice);
		else
			// Sourcerer or knight has to agree or disagree on kings order
			user_cycle(second_person_choice);
		
		std::cout << "Third cycle" << std::endl;
		end_session();
		if(KingIsSubstituted)
			// Sourcerer or knight has to agree or disagree on kings order
			user_cycle(second_person_choice);
		else
			user_cycle(dummy_person_choice);
		
		day_end();
		
		std::cout << "Nightfall" << std::endl;
		end_session();
		nightfall();
	}
	
	std::cout << "Game is finished. Congratulations to the winners ^_^" << std::endl;
	
	dump_players(true);
	
	return 1;
}


