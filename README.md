# 🚀 Bruteforceur SHA-256 Accéléré par GPU (C & OpenCL)

Ce projet est un moteur de hachage cryptographique et de bruteforce haute performance, développé intégralement de zéro. Il implémente le standard de sécurité SHA-256 en pur langage C et exploite la puissance de calcul parallèle des cartes graphiques (GPU) grâce à la bibliothèque OpenCL.

L'objectif de ce projet est de démontrer la vulnérabilité des mots de passe courts face à la puissance de calcul moderne, en générant et en testant des milliards de combinaisons en quelques secondes.

## ✨ Fonctionnalités Clés

* **Implémentation native de SHA-256 :** Développement complet du moteur de hachage sans utiliser de bibliothèques externes. Comprend les opérations bit à bit, le padding dynamique, l'extension de message et les boucles de compression avec les constantes cryptographiques officielles.
* **Parallélisation massive (OpenCL) :** Conception d'une architecture *Host/Kernel* séparant la logique de contrôle (CPU) de la logique de calcul intensif (GPU).
* **Génération mathématique $O(1)$ :** Création d'un algorithme convertissant directement un identifiant de thread (ID) en mot de passe. Cette approche mathématique remplace les boucles séquentielles classiques et débloque le calcul parallèle à grande échelle.
* **Haute Capacité (64 bits) :** Gestion des espaces de recherche colossaux (plusieurs milliards de combinaisons) via l'utilisation de types 64 bits (`uint64_t` et `ulong`) pour prévenir les dépassements d'entiers (*Integer Overflows*).
* **Résilience système :** Gestion intelligente de la taille des lots de travail (*Work-groups*) pour éviter les interruptions matérielles du système d'exploitation (TDR - *Timeout Detection and Recovery*) lors du traitement de charges extrêmes.

## 🛠️ Compétences Techniques Démontrées

* **Langages :** C, OpenCL C.
* **Cryptographie :** Maîtrise des standards de hachage de la famille SHA-2, compréhension de l'endianness (*Big-Endian* vs *Little-Endian*) et manipulation de données au niveau binaire.

## 📁 Architecture du Projet

Le projet est divisé en deux composants principaux qui travaillent en synergie :

1. **`lab1.c` (Host Code) :** Exécuté par le processeur (CPU). Il détecte le matériel OpenCL disponible, alloue les tampons de mémoire, prépare la cible de hachage et envoie les instructions au GPU. Il s'occupe également de récupérer l'ID vainqueur et de le traduire en mot de passe en clair.
2. **`bruteforce.cl` (Kernel Code) :** L'usine de calcul. Compilé à la volée pour la carte graphique, ce code est exécuté par des millions de *threads* simultanément. Chaque thread génère son propre mot de passe, applique le padding binaire, calcule l'empreinte SHA-256 complète et la compare à la cible.

## 📌 Remarques Importantes
**Assistance IA** : Bien que la logique cryptographique et le moteur SHA-256 aient été conçus manuellement, la création du pipeline de communication Host/Kernel (tuyauterie OpenCL) a été réalisée avec l'assistance d'une intelligence artificielle.

**Configuration de la Cible** : Pour que le programme fonctionne avec une nouvelle empreinte, il faut prendre le hash SHA-256 à casser, le diviser manuellement en 8 valeurs hexadécimales de 32 bits (format 0x...) et l'assigner au tableau target_hash dans le code. De plus, la variable password_size doit impérativement être configurée avec la taille exacte du mot de passe à chercher, sans quoi la vérification échouera.

**Limites du bruteforce** : En raison d'un potentiel int overflow et de la limite materielle des gpu classiques, il ne semble pas possible de casser des hashs de plus de 7 caractères sur l'alphabet minuscule (a-z), et 5 sur les deux alphabets et les signes spéciaux (A-Za-z&!).

## ⚙️ Compilation et Utilisation

**Prérequis :**
* Compilateur C (ex: GCC).
* Pilotes de carte graphique à jour.
* Bibliothèque OpenCL et en-têtes Khronos (`CL/cl.h`).

**Compilation (Exemple sous Windows avec MinGW) :**

Soit "C:\Windows\System32\OpenCL.dll", le chemin vers votre propre bibliothèque OpenCL :
```bash
gcc -Wall -Wextra -Werror -I. lab1.c -o bruteforce_gpu.exe "C:\Windows\System32\OpenCL.dll"
