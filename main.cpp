#include <vector>
#include <concepts>
#include <type_traits>
#include <functional>
#include <utility>
#include <string>
#include <fstream>
#include <iostream>
#include "hashtable.h"

struct FoodMenu {
	std::string name;
	double price = 0;
	int kcal_count = 0;
	FoodMenu() {}
	FoodMenu(std::string name_init, double price_init, int kcal_init) : name(name_init), price(price_init), kcal_count(kcal_init) {}
};

int main() {
	Hashtable<std::string, FoodMenu> food_map;

	std::ifstream fin("pairs.in");
	int n;
	fin >> n;
	for (int i = 0; i < n; i++) {
		std::string restaurant, food_name;
		double price;
		int kcals;
		fin >> restaurant >> food_name >> price >> kcals;
		food_map.insert(restaurant, FoodMenu(food_name, price, kcals));
	}

	std::cout << "Bine ai venit la hashtable menu!\n";

	while (true) {
		std::cout << "\n(1) cauta elemente in tabel\n";
		std::cout << "(2) adauga elemente in tabel\n";
		std::cout << "(3) sterge elemente din tabel\n";
		std::cout << "(4) afisare secventiala tabel\n";
		std::cout << "(orice altceva) quit\n\n";
		std::cout << "Alegerea ta: ";

		int option;
		std::cin >> option;
		
		switch (option) {
		case 1: {
			std::cout << "Cheia cautata: ";
			std::string key;
			std::cin >> key;

			auto it = food_map.find(key);
			if (it != food_map.end()) {
				auto menu = it->second;
				std::cout << "\n" << key << ": '" << menu.name << "', " << menu.price << "RON, " << menu.kcal_count << "kcal.\n";
			}
			else {
				std::cout << "\nNu a fost gasita in tabel o pereche cu cheia '" << key << "'.\n";
			}
			break;
		}
		case 2: {
			std::cout << "Cheie: ";
			std::string key;
			std::cin >> key;

			FoodMenu menu;
			std::cout << "Valoare (nume, pret, kcal): ";
			std::cin >> menu.name >> menu.price >> menu.kcal_count;

			food_map.insert(key, menu);
			break;
		}
		case 3: {
			std::cout << "Cheie: ";
			std::string key;
			std::cin >> key;

			bool removed = food_map.remove(key);

			if (removed)
				std::cout << "\nSters cu succes perechea cu cheia '" << key << "' din tabel.\n";
			else
				std::cout << "\nNu exista pereche cu cheia '" << key << "' in tabel.\n";
			break;
		}
		case 4: {
			int i = 0;
			std::cout << "\n";
			for (auto it = food_map.begin(); it != food_map.end(); ++it) {
				std::string key = it->first;
				FoodMenu menu = it->second;
				std::cout << key << ": '" << menu.name << "', " << menu.price << "RON, " << menu.kcal_count << "kcal.\n";
				i++;
			}
			std::cout << "\n" << i << " elemente.\n";
			break;
		}
		default:
			return 0;
		}
	}
}