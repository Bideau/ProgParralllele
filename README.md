-------------------- Installation --------------------------

Est n�cessaire pour l'ex�cution openmpi

Package n�cessaire:
	openmpi-bin
	openmpi-common
	mpi-default-dev

Il est nessaire d'executer la commande 'make install'.

-------------------- Ex�cution -----------------------------

Il suffit ensuite d'ex�cuter un simple 'make' puis 'make exec'

-------------------- Explication --------------------------

Le fichier principale est mpiMain.c.Il fait appel au fichier json.c pour le parsage du json, myK.c pour l'application du k-means qui retourne les clusters et les centroids, et enfin a graph4.c qui permet l'affichage des graphs. 

Le fichier json.c est un fichier pris du github de James McLaughlin ( https://github.com/udp/json-parser ).

Le fichier myK.c est un fichier pris du site de Roger Zhang ( http://cs.smu.ca/~r_zhang/code/kmeans.c ) puis modifie pour permettre de mettre en place le k-means sur la structure de data utilis�e.

Le fichier graph4.c est un fichier qui utilise le fichier gnuplot_i.c pour la cr�ation de graphique. Il r�cup�re pour chaque processus les fichiers �crits pour les afficher a l'�cran.

Le fichier gnuplot_i.c est un fichier pris du site de N. Devillard ( http://ndevilla.free.fr/gnuplot/ ).

Le fichier strut.h contient la structure Game qui sert a stocker les informations du fichier json. Pour l'instant seulement une partie des donn�es est pris en charge et utilis�s.

Le fichier mpiMain.c est d�coup� en plusieurs partie. Il commence par une initialisation de ce qui est n�cessaire a MPI, comme par exemple la d�finition de la structure Game. Ensuite le processus principal va lire le fichier json et le parser pour remplir un tableau de structure. Une fois fini le tableau de donn�es est transmis a tout le monde. Ensuite tout le monde va faire 4 nombres de clusters diff�rents pour permettre une bonne r�partition des taches ( Un bug apparu a �t� contourne. Le k-means plante si le nombre de cluster est strictement inf�rieur a 4, dans ce cas la valeur est mise a 4.). 
Chaque test se deroulle de la fa�on suivante. En premier lieu, un K-means est effectue sur le jeu de donn�es pour r�cup�rer les clusters ainsi que les centroids. Ensuite les donn�es sont ordonn�es, par cluster, dans un tableau a 2 dimensions de structure. Chaque colonne du tableau est ensuite �crit dans un fichier, ce qui permettra le dessin. Pour le dessin une simple fonction est appel�. Enfin, certaines donnes sont nettoy�es.

