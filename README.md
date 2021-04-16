# Pong en C

Projet de jeu en C avec utilisation d'échange entre processus par signal, boîte aux lettres, mémoire partagée, ...

## Installation

### Requis

- libncurses
`sudo apt install libncurses-dev`
- libopenal 
`sudo apt install libopenal-dev`
- libalut
`sudo apt install libalut-dev`
- libaudio
`sudo apt install libaudio-dev`

### Compilation du projet

`make`

### Execution du projet

Après avoir compilé le projet avec la commande make, exécuter :
`./main.o`

## Architecture du logiciel 

![Schéma d'architecture logicielle](./schem.png)


## Crédit

- Exemple d'utilisation OpenAL en C pour lire un fichier WAV : [ici](https://github.com/ffainelli/openal-example)
- Musique de fond : [DAFT PUNK - Robot Rock](https://www.youtube.com/watch?v=sFZjqVnWBhc)

