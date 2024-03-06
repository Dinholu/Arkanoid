# Cahier des charges 


## Ce qui est déjà fait 

- Une interface graphique
- Génération d'une plateforme accueillant la balle
- Une balle qui bouge dans le cadrillage du plateau


## A faire 

 - Que la balle touche la barre
 - AJouter les briques 3 types de brique:
        - brique simple
        - brique double
        - brique indestructible    
 - Ajouter la vitesse selon l'angle etc
 - Ajouter le score
 - Ajouter les bonus :
    - Ralentir
     - Accrocher la balle et tirer
     - Aggrandir la plateforme
     - Diviser la balle en 3 balles
     - laser (qui explose les briques)
     - break ?
     - Player ADDITION? 
 - 33 rounds
 - Niveaux stocker sous forme de texte
 - Les données de la balle (position, direction, vitesse) seront calculé et stockés en réel pour plus de précision, même si les positions à l’écran sont entières (pixel)
 - Les objets à l’écran (hormis la balle) étant tous rectangulaires, l’algorithme de détection de collision se fait simplement en comparant x_min, x_max, y_min, y_max avec la position de la balle. Les briques étant alignées régulièrement,
 - Si on touche quelque-chose de vertical, la coordonnée x de la vitesse s’inverse.
 - Si on touche quelque-chose d’horizontal, la coordonnée y de la vitesse s’inverse.
 - Vous prendrez en compte la vitesse (horizontale) du vaisseau pour changer la direction du rebond.

### BONUS 
-  deux joueurs,
-   ombres,
-  traces derrière la balle,
-   disparition progressive des briques.
-   distribution bonus/malus non aléatoire, etc ...

## CheckList
### Jeu de base
- [x] Balle touche la barre
- [ ] Ajouter les 3 types de briques :
	- [x] Brique simple
	- [ ] Brique dure :
		- [ ] 2 fois : Lvl 1 - 8
		- [ ] 3 fois : Lvl 9 - 16
		- [ ] 4 fois : Lvl 17 - 24
		- [ ] 5 fois : Lvl 25 - 32
		- [ ] Points bonus pour briser les mur : 100 x LvL
	- [ ] Brique indestructible
- [x] Collision avec les briques
	- [x] Collision horizontal et vertical
 	- [x] Une brique par collision (break)
  	- [ ] Collision en angle  
- [ ] Récupérer les coordonnées sur le sprites
	- [ ] Brique couleur fade
 	- [ ] Power-Up
  	- [ ] Map
  	- [ ] Vaisseau 
- [ ] Power-up aléatoire dans briques
- [ ] Affichage du score

### Design Level
- [x] Des fichiers textes qui modélisent les différents niveaux
	- [x] Test avec briques alignés
