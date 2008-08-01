1) Hypothèses
   ----------

a) Une version de SALOME est installée dans le répertoire : <salome>

b) Le produit BLSURF est installé dans le répertoire <salome>/BLSurf

c) Le produit "decimesh" doit être dans la variable d'environnement PATH

2) tgz
   ---

cd <salome>
tar zxf BLSURFPLUGIN_SRC.tgz

3) Construire le plugin BLSURF
   ---------------------------

bash
cd <salome>
source env_build.sh
source BLSURFPLUGIN_SRC_3.2.4/env_blsurf.sh

cd BLSURFPLUGIN_SRC_3.2.4
./build_configure

cd ..
mkdir BLSURFPLUGIN_BUILD
mkdir BLSURFPLUGIN_INSTALL
cd BLSURFPLUGIN_BUILD
../BLSURFPLUGIN_SRC_3.2.4/configure --prefix=<salome>/BLSURFPLUGIN_INSTALL

# Dans les traces chercher la rubrique "Summary",
# et vérifier que toutes les réponses sont à yes.
# S'il y a au moins 1 "no", il faut résoudre cela avant de poursuivre.

make
make install

exit

4) Lancer SALOME avec BLSURF
   -------------------------

bash
cd <salome>

cp BLSURFPLUGIN_SRC_3.2.4/SalomeApprc.3.2.4  ~/.SalomeApprc.<version de salome>
# Cette copie est à faire une fois

source env_products.sh
source BLSURFPLUGIN_SRC_3.2.4/env_blsurf.sh

runSalome

5) Session interactive
   -------------------

a) Aller dans le module de géométrie et construire une figure simple

b) Aller dans le module de maillage

c) Aller dans le menu de création d'un maillage

d) Choisir la figure simple comme géométrie à mailler

e) Aller dans l'onglet 2D

f) Choisir BLSURF

g) Cliquer dans le bouton parametre, une boite de dialogue apparait, cette boite n'est pas terminée.
   Seul la case à cocher "optimize" est signifiante:
   - décochée: uniquement BLSURF
   - cochée  : BLSURF + decimesh

h) Cliquer OK

i) Aller dans le menu de maillage et lancer l'item "Compute"

j) Résultat: un maillage créer par BLSURF et visualiser dans SALOME

k) Pour paramétrer BLSURF: il faut pour l'instant créer un fichier blsurf.env dans le répertoire de lancement de SALOME

l) fin
