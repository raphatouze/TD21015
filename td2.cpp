
/**
* Programme qui permet de creer et detruire dynamiquement des films, listeFilms, acteure et listeActeurs.
* \file   td2.cpp
* \author Veillette
* \date   13 janvier 2022
* Créé le 11 janvier 2022
*/




#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>

#include "cppitertools/range.hpp"
#include "gsl/span"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).


using namespace std;
using namespace iter;
using namespace gsl;

#pragma endregion//}

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{

UInt8 lireUint8(istream& fichier)
{
	UInt8 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}

ListeFilms::ListeFilms()
{
	cout << "creation de ListeFilms()" << endl;
}
ListeFilms::ListeFilms(Film** elements)
	:elements_(elements)
{
	cout << "creation de ListeFilms(attributs)" << endl;
}

int ListeFilms::getCapacite() const
{
	return capacite_;

}

int ListeFilms::getNElements() const
{
	return nElements_;

}

Film** ListeFilms::getElements() const
{
	return elements_;

}

void ListeFilms::setCapacite(int capacite)
{
	capacite_ = capacite;

}

void ListeFilms::setNElements(int nElements)
{
	nElements_ = nElements;

}

void ListeFilms::setElements(Film** elements)
{
	elements_ = elements;

}

void ListeFilms::agrandirListe()
{
	int nouvelleCapacite = getCapacite();
	int nElements = getNElements();
	Film** elements = getElements();
	nouvelleCapacite++;
	nouvelleCapacite *= 2;
	setCapacite(nouvelleCapacite);

	Film** nouveauxElements = new Film * [nouvelleCapacite]();

	for (int i : range(nElements))
		nouveauxElements[i] = elements[i];
	delete[] getElements();
	setElements(nouveauxElements);
}


void ListeFilms::ajoutFilmListeFilm(Film* filmAjouter)
{

	if (getCapacite() <= getNElements())
	{
		agrandirListe();
	}
	Film** elements = getElements();
	elements[getNElements()] = filmAjouter;
	int nElements = getNElements();
	nElements++;
	setNElements(nElements);

}

void ListeFilms::enleverFilmListeFilm(Film* filmEnlever)
{
	span <Film*> tableauFilms(getElements(), getNElements());
	for (Film* film : tableauFilms)
		if (film == filmEnlever)
		{
			film = nullptr;
			int nElements = getNElements();
			nElements--;
			setNElements(nElements);
		}
	for (int i : range(tableauFilms.size() - 1))
		if (tableauFilms[i] == nullptr)
		{
			swap(tableauFilms[i], tableauFilms[i + 1]);
		}
	Film** elementsPtr = new Film * [getCapacite()]();
	for (int i : range(getNElements()))
		elementsPtr[i] = tableauFilms[i];
	delete[]getElements();
	setElements(elementsPtr);
}
Acteur* ListeFilms::trouverActeurListfilms(const string& nomActeurChercher) const
{
	span <Film*> tableauFilms(getElements(), getNElements());

	for (Film* filmPtr : tableauFilms)
	{
		if (filmPtr != nullptr)
		{
			Film film = *filmPtr;
			span <Acteur*> tableauActeur(film.acteurs.elements, film.acteurs.nElements);
			for (Acteur* acteurPtr : tableauActeur)
			{
				if (acteurPtr->nom == nomActeurChercher)
				{
					return acteurPtr;
				}


			}

		}
	}
	return nullptr;
}
Acteur* ListeFilms::lireActeur(istream& fichier)
{

	Acteur acteur = {};
	acteur.nom = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe = lireUint8(fichier);
	Acteur* acteurPtr = trouverActeurListfilms(acteur.nom);
	if (acteurPtr == nullptr)
	{
		Acteur* nouveauActeurPtr = new Acteur(acteur);
		cout << "nom de l'acteur creer : " << acteur.nom << endl;
		return nouveauActeurPtr;

	}
	else
		return acteurPtr; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
}
void ajouterActeurListeActeur(ListeActeurs& listeActeurs, Acteur* acteurAjouter)
{
	int nouvelleCapacite = listeActeurs.capacite;

	if (listeActeurs.capacite <= listeActeurs.nElements)
	{
		nouvelleCapacite++;
		nouvelleCapacite *= 2;
		listeActeurs.capacite = nouvelleCapacite;

		Acteur** nouveauxElements = new Acteur * [listeActeurs.capacite]();


		for (int i : range(listeActeurs.nElements))
			nouveauxElements[i] = listeActeurs.elements[i];

		nouveauxElements[listeActeurs.nElements] = acteurAjouter;
		listeActeurs.nElements++;
		delete[] listeActeurs.elements;
		listeActeurs.elements = nouveauxElements;
	}
	else
	{
		listeActeurs.elements[listeActeurs.nElements] = acteurAjouter;
		listeActeurs.nElements++;

	}

}


Film* ListeFilms::lireFilm(istream& fichier)
{
	Film film = {};
	film.titre = lireString(fichier);
	film.realisateur = lireString(fichier);
	film.anneeSortie = lireUint16(fichier);
	film.recette = lireUint16(fichier);
	film.acteurs.capacite = lireUint8(fichier);  //NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour réutiliser cette réallocation.
	film.acteurs.nElements = 0;
	film.acteurs.elements = new Acteur * [film.acteurs.capacite]();
	Film* nouveauFilmPtr = new Film(film);

	//TODO: Placer l'acteur au bon endroit dans les acteurs du film.

	for (int i : range(nouveauFilmPtr->acteurs.capacite))
	{
		Acteur* acteurPtr = lireActeur(fichier);
		ajouterActeurListeActeur(nouveauFilmPtr->acteurs, acteurPtr);
		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.
		ajoutFilmListeFilm(nouveauFilmPtr);
	}


	return nouveauFilmPtr; //TODO: Retourner le pointeur vers le nouveau film.


}

ListeFilms creerListe(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	int nElements = lireUint16(fichier);
	//TODO: Créer une liste de films vide.

	ListeFilms nouvelleListe{};
	Film** nouveauTableaau = new Film * [nouvelleListe.getCapacite()]();
	nouvelleListe.setElements(nouveauTableaau);
	for (int i = 0; i < nElements; i++)
	{
		Film* film = nouvelleListe.lireFilm(fichier);
		nouvelleListe.ajoutFilmListeFilm(film); //TODO: Ajouter le film à la liste.
	}
	return nouvelleListe; //TODO: Retourner la liste de films.
}

void detruireFilm(Film*& film)
{

	span <Acteur*> listeActeurs(film->acteurs.elements, film->acteurs.nElements);
	for (Acteur* acteurPtr : listeActeurs)
	{
		acteurPtr->joueDans.enleverFilmListeFilm(film);
		if (acteurPtr->joueDans.getElements() == 0)
		{
			delete[] acteurPtr->joueDans.getElements();
			cout << "acteur detruit : " << acteurPtr->nom << endl;
			delete acteurPtr;

		}
	}
	delete[] film->acteurs.elements;
	delete film;
}

void ListeFilms::detruireListFilms()
{
	delete[] getElements();
}

void ListeFilms::afficherActeur(const Acteur& acteur) const
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

void ListeFilms::afficherDescriptionFilm(const Film* film) const
{
	span<Acteur*> acteurlisteFilm(film->acteurs.elements, film->acteurs.nElements);
	cout << "Titre : " << film->titre << "\n"
		<< "Annee de sortie : " << film->anneeSortie << "\n"
		<< "Recette : " << film->recette << "\n"
		<< "Realisateur : " << film->realisateur << "\n"
		<< "Acteurs :\n";
	for (Acteur* acteurPtr : acteurlisteFilm)
	{
		afficherActeur(*acteurPtr);

	}

}

void ListeFilms::afficherListeFilms() const
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = { "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" };
	cout << ligneDeSeparation;
	//TODO: Changer le for pour utiliser un span.
	span<Film*> liste(getElements(), getNElements());
	for (Film* film : liste)
	{
		//TODO: Afficher le film.
		afficherDescriptionFilm(film);
		cout << ligneDeSeparation;
	}
}

void ListeFilms::afficherFilmographieActeur(const string& nomActeur) const
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	const Acteur* acteur = trouverActeurListfilms(nomActeur);
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		afficherListeFilms();
}


//TODO: Une fonction pour ajouter un Film à une ListeFilms, le film existant déjà; on veut uniquement ajouter le pointeur vers le film existant.  Cette fonction doit doubler la taille du tableau alloué, avec au minimum un élément, dans le cas où la capacité est insuffisante pour ajouter l'élément.  Il faut alors allouer un nouveau tableau plus grand, copier ce qu'il y avait dans l'ancien, et éliminer l'ancien trop petit.  Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
//void ajoutFilmListeFilm(ListeFilms& listeFilms,Film* filmAjouter)
//{
//	int nouvelleCapacite = listeFilms.capacite;
//
//	if (listeFilms.capacite<= listeFilms.nElements)
//	{
//		nouvelleCapacite++;
//		nouvelleCapacite *= 2;
//		listeFilms.capacite = nouvelleCapacite;
//
//		Film** nouveauxElements = new Film* [listeFilms.capacite]();
//		
//		
//		for(int i : range(listeFilms.nElements))
//			nouveauxElements[i] = listeFilms.elements[i];
//
//		nouveauxElements[listeFilms.nElements] = filmAjouter;
//		listeFilms.nElements++;
//		delete[] listeFilms.elements;
//		listeFilms.elements = nouveauxElements;
//	}
//	else
//	{
//		listeFilms.elements[listeFilms.nElements] = filmAjouter;
//		listeFilms.nElements++;
//
//	}
//
//
//}

//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
//void enleverFilmListeFilm( ListeFilms& listeFilms, Film* filmEnlever)
//{	
//	span <Film*> tableauFilms (listeFilms.elements, listeFilms.capacite);
//	for (Film* film : tableauFilms)
//		if (film == filmEnlever)
//		{
//			film = nullptr;
//				listeFilms.nElements--;
//		}
//	for (int i : range(tableauFilms.size() -1)) 
//		if (tableauFilms[i] == nullptr)
//		{
//			swap(tableauFilms[i], tableauFilms[i+1]);
//		}
//
//}
//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.
//Acteur* trouverActeurListfilms(const ListeFilms& listeFilms, const string& nomActeurChercher)
//{
//	span <Film*> tableauFilms(listeFilms.elements, listeFilms.nElements);
//
//	for (Film* filmPtr : tableauFilms)
//	{
//		if (filmPtr != nullptr)
//		{
//			Film film = *filmPtr;
//			span <Acteur*> tableauActeur(film.acteurs.elements, film.acteurs.nElements);
//			for (Acteur* acteurPtr : tableauActeur)
//			{
//				if (acteurPtr->nom == nomActeurChercher)
//					return acteurPtr;
//
//			}
//
//		}
//	}
//	return nullptr;
//}
////TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms.  La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la fonction ci-dessus).
//Acteur* lireActeur(istream& fichier, ListeFilms& listeFilms)
//{
//	Acteur acteur = {};
//	acteur.nom            = lireString(fichier);
//	acteur.anneeNaissance = lireUint16 (fichier);
//	acteur.sexe           = lireUint8  (fichier);
//	Acteur* acteurPtr = trouverActeurListfilms(listeFilms, acteur.nom);
//	if (acteurPtr == nullptr)
//	{
//		Acteur* nouveauActeurPtr = new Acteur (acteur);
//		cout << "nom de l'acteur creer : " << acteur.nom <<endl;
//		return nouveauActeurPtr;
//
//	}
//	else
//		return acteurPtr; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
//}
//
//Film* lireFilm(istream& fichier,ListeFilms& listeFilms)
//{
//	Film film = {};
//	film.titre = lireString(fichier);
//	film.realisateur = lireString(fichier);
//	film.anneeSortie = lireUint16(fichier);
//	film.recette = lireUint16(fichier);
//	film.acteurs.capacite = lireUint8(fichier);  //NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour réutiliser cette réallocation.
//	film.acteurs.nElements = 0;
//	film.acteurs.elements = new Acteur * [film.acteurs.capacite]();
//	Film* nouveauFilmPtr = new Film(film);
//	
//	//TODO: Placer l'acteur au bon endroit dans les acteurs du film.
//
//	for (int i : range(nouveauFilmPtr->acteurs.capacite))
//	{
//		Acteur* acteurPtr = lireActeur(fichier, listeFilms);
//		ajouterActeurListeActeur(nouveauFilmPtr->acteurs, acteurPtr);
//		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.
//		ajoutFilmListeFilm(acteurPtr->joueDans, nouveauFilmPtr);
//	}
//
//	
//	return nouveauFilmPtr; //TODO: Retourner le pointeur vers le nouveau film.
//}



//TODO: Une fonction pour détruire un film (relâcher toute la mémoire associée à ce film, et les acteurs qui ne jouent plus dans aucun films de la collection).  Noter qu'il faut enleve le film détruit des films dans lesquels jouent les acteurs.  Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.

//TODO: Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.
//void detruireListFilms(ListeFilms& listeFilms)
//{
//
//	delete[] listeFilms.elements;
//}



//TODO: Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).

//void afficherListeFilms(const ListeFilms& listeFilms)
//{
//	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
//	static const string ligneDeSeparation = {"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"};
//	cout << ligneDeSeparation;
//	//TODO: Changer le for pour utiliser un span.
//	span<Film*> liste(listeFilms.elements, listeFilms.nElements);
//	for (Film* film : liste)
//	{
//		//TODO: Afficher le film.
//		afficherDescriptionFilm(film);
//		cout << ligneDeSeparation;
//	}
//}

//void afficherFilmographieActeur(const ListeFilms& listeFilms, const string& nomActeur)
//{
//	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
//	const Acteur* acteur = trouverActeurListfilms(listeFilms,nomActeur);
//	if (acteur == nullptr)
//		cout << "Aucun acteur de ce nom" << endl;
//	else
//		afficherListeFilms(acteur->joueDans);
//}

int main()
{
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	//int* fuite = new int; //TODO: Enlever cette ligne après avoir vérifié qu'il y a bien un "Fuite detectee" de "4 octets" affiché à la fin de l'exécution, qui réfère à cette ligne du programme.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	static const string ligneDeSeparation2 = { "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" };

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ListeFilms listeFilms = creerListe("films.bin");

	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	//TODO: Afficher le premier film de la liste.  Devrait être Alien.
	listeFilms.afficherDescriptionFilm(listeFilms.getElements()[0]);
	cout << ligneDeSeparation << "Les films sont:" << endl;
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	listeFilms.afficherListeFilms();
	//TODO: Modifier l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).  Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	Acteur* benedictPtr = listeFilms.trouverActeurListfilms("Benedict Cumberbatch");
	benedictPtr->anneeNaissance = 1976;
	cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl;
	//TODO: Afficher la liste des films où Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	benedictPtr->joueDans.afficherListeFilms();
	//TODO: Détruire et enlever le premier film de la liste (Alien).  Ceci devrait "automatiquement" (par ce que font vos fonctions) détruire les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.

	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//TODO: Afficher la liste des films.
	listeFilms.afficherListeFilms();
	//TODO: Faire les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.

	//TODO: Détruire tout avant de terminer le programme.  La bibliothèque de verification_allocation devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	cout << ligneDeSeparation << endl;
	span<Film*> listeFilms1(listeFilms.getElements(), listeFilms.getNElements());

	for (Film* film : listeFilms1)
	{
		cout << "Destruction du film : " << film->titre << endl;
		detruireFilm(film);
		cout << ligneDeSeparation2 << endl;
	}

	listeFilms.detruireListFilms();

}