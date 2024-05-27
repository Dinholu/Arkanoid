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


### Aide pour la compréhension 

http://nick-aschenbach.github.io/blog/2015/04/27/arkanoid-game-levels/
