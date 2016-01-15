-------------------- Installation --------------------------

Est nécessaire pour l'exécution openmpi

Package nécessaire:
	openmpi-bin
	openmpi-common
	mpi-default-dev

Il est nessaire d'executer la commande 'make install'.

-------------------- Exécution -----------------------------

Il suffit ensuite d'exécuter un simple 'make' puis 'make exec'

-------------------- Explication --------------------------

Le fichier principale est mpiMain.c.Il fait appel au fichier json.c pour le parsage du json, myK.c pour l'application du k-means qui retourne les clusters et les centroids, et enfin a graph4.c qui permet l'affichage des graphs. 

Le fichier json.c est un fichier pris du github de James McLaughlin ( https://github.com/udp/json-parser ).

Le fichier myK.c est un fichier pris du site de Roger Zhang ( http://cs.smu.ca/~r_zhang/code/kmeans.c ) puis modifie pour permettre de mettre en place le k-means sur la structure de data utilisée.

Le fichier graph4.c est un fichier qui utilise le fichier gnuplot_i.c pour la création de graphique. Il récupère pour chaque processus les fichiers écrits pour les afficher a l'écran.

Le fichier gnuplot_i.c est un fichier pris du site de N. Devillard ( http://ndevilla.free.fr/gnuplot/ ).

Le fichier strut.h contient la structure Game qui sert a stocker les informations du fichier json. Pour l'instant seulement une partie des données est pris en charge et utilisés.

Le fichier mpiMain.c est découpé en plusieurs partie. Il commence par une initialisation de ce qui est nécessaire a MPI, comme par exemple la définition de la structure Game. Ensuite le processus principal va lire le fichier json et le parser pour remplir un tableau de structure. Une fois fini le tableau de données est transmis a tout le monde. Ensuite tout le monde va faire 4 nombres de clusters différents pour permettre une bonne répartition des taches ( Un bug apparu a été contourne. Le k-means plante si le nombre de cluster est strictement inférieur a 4, dans ce cas la valeur est mise a 4.). 
Chaque test se deroulle de la façon suivante. En premier lieu, un K-means est effectue sur le jeu de données pour récupérer les clusters ainsi que les centroids. Ensuite les données sont ordonnées, par cluster, dans un tableau a 2 dimensions de structure. Chaque colonne du tableau est ensuite écrit dans un fichier, ce qui permettra le dessin. Pour le dessin une simple fonction est appelé. Enfin, certaines donnes sont nettoyées.

