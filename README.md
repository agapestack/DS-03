# Projet: Blockchain appliquée à un processus éléctoral

## Partie 1

```bash
cd ./ex1
gcc ex1.c exponentiation.c miller_rabin.c -lm

cd ./ex2
gcc ex2.c keys.c ../ex1/exponentiation.c ../ex1/miller_rabin.c -lm
```

### Rappel utilisation gnuplot

```bash
plot "comparaison_exponentiation.txt" using 1:2 title "modpow_naive" with lines
replot "comparaison_exponentiation.txt" using 1:3 title "modpow" with lines
set term postscript portrait
set output "comparaison_exponentiation.ps"
set size 0.7, 0.7
```

## Partie 2

## Partie 3

## Partie 4

## Partie 5

## Error code

- 12: Erreur d'allocation dynamique
- 14: Appel de fonction avec paramètre invalide