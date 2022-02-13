#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>

#include "cppitertools/range.hpp"
#include "gsl/span"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp" 

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.
class ListeFilms
{
public:
    ListeFilms();
    ListeFilms(Film elements);

    //getters
    int getCapacite() const;
    int getNElements() const;
    Film getElements() const;

    //setters
    void setCapacite(int capacite);
    void setNElements(int nElements);
    void setElements(Film** elements);

    //methodes
    void agrandirListe();
    void ajoutFilmListeFilm(Film* filmAjouter);
    void enleverFilmListeFilm(Film* filmEnlever);
    Film* lireFilm(std::istream& fichier);
    Acteur* lireActeur(std::istream& fichier);
    Acteur* trouverActeurListfilms(const std::string& nomActeurChercher) const;
    void detruireListFilms();
    void afficherActeur(const Acteur& acteur)const;
    void afficherDescriptionFilm(const Film* film) const;
    void afficherListeFilms() const;
    void afficherFilmographieActeur(const std::string& nomActeur) const;

private:
    int capacite = 1;
    int nElements = 0;
    Film elements_;
};

//struct ListeFilms {
//    int capacite, nElements;
//    Film elements; // Pointeur vers un tableau de Film, chaque Film pointant vers un Film.
//};

struct ListeActeurs
{
    int capacite, nElements;
    Acteur** elements; // Pointeur vers un tableau de Acteur, chaque Acteur pointant vers un Acteur.
};

struct Film
{
    std::string titre, realisateur; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
    int anneeSortie, recette; // Année de sortie et recette globale du film en millions de dollars
    ListeActeurs acteurs;
};

struct Acteur
{
    std::string nom; int anneeNaissance; char sexe;
    ListeFilms joueDans;

};