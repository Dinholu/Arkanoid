# Arkanoid

## Description
Arkanoid est un jeu classique de casse-briques où le joueur contrôle une raquette pour dévier une balle et détruire des briques. Le projet est implémenté en C et offre deux branches :
- **main** : contient le jeu avec des effets sonores dont il est nécessaire d'installer la libraire `SDL_Mixer()` sur votre machine (`apt-get install libsdl2-mixer-2.0-0`)
- **no-sound-branch** : contient le jeu sans effets sonores.

## Règles du jeu
- Le joueur utilise la raquette pour garder la balle en jeu et casser toutes les briques sur l'écran.
- Certaines briques ont des caractéristiques spéciales :
  - **Briques grises** : nécessitent 2 coups pour être détruites, puis un coup supplémentaire tous les 8 coups.
  - **Briques dorées** : indestructibles.
- Le jeu comporte 33 niveaux, avec un boss final nommé Doh au dernier niveau.

## Bonus Capsules
Des capsules de bonus tombent parfois des briques détruites. Chaque type de capsule a un effet unique et rapporte 1000 points :
- **S** : Ralentit la balle.
- **C** : Attrape la balle et permet de tirer.
- **E** : Agrandit la raquette.
- **D** : Divise la balle en trois.
- **L** : Ajoute des lasers tirés avec la barre d'espace.
- **B** : Passe au niveau suivant (le plus rare).
- **P** : Ajoute une vie au joueur.

## Fonctionnalités additionnelles
- Ombres des balles et des briques.
- Tracé derrière la balle.
- Disparition progressive des briques.
- Distribution non aléatoire des bonus.
- Niveau final avec le boss Doh.

## Contributeurs
@WRKT alias Winness Rakotozafy  
@Dinholu alias Alizée HETT
