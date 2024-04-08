# TuringBombe
Simulation of Enigma machine and Turing's bombe

## `enigma_app.exe`

This is an Enigma simulator, supporting most military Enigma variants (including Kriegmarine M4).

Usage: `enigma_app <numrotors> <UKW> <R1> <R2> <R3> [R4] [steckers] <ring> <grun>`

| Param    | Description |
|----------|------------------|
|numrotors | Number of rotors (3 or 4)  |
|UKW       | Reflector (1:beta 2:gamma) |
|R1-R4     | Rotors (1-8, 1:beta 2:gamma) |
|ring      | Ringstellung (ring settings) |
|grun      | Grundstellung (Starting rotor display) |

(All rotor settings must be in left-to-right order)

Examples:
```dos
echo "The quick brown fox jumps over the lazy dog" | ./enigma_app 4 2 2 1 2 3 VXYZ PQDV
```
cypher text will be: `IRPMRQDZVBQIQDRDPCXGBAWODPBFIPISCAW`

```dos
echo "The quick brown fox jumps over the lazy dog" | ./enigma_app 4 2 2 1 2 3 AB:CD:EF:GH VXYZ PQDV
```
cypher text will be: `ICOMRQDZSAQIQMRCPDXHABWQCPWFIUISCBV`

To verify the above outputs, just decipher them using Rijmenants' Enigma simulator:
https://www.ciphermachinesandcryptology.com/en/enigmasim.htm

If settings are input correctly, Rijmenants' Enigma simulator will return the following result:
`THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG`

## `turing_bombe.exe`

This application runs the bombe for a given wheel order

Usage: `turing_bombe <menufile> <UKW> <R1> <R2> <R3> [R4]`

| Param    | Description |
|----------|------------------|
|menufile  | name of menu file |
|UKW       | Reflector (1:beta 2:gamma) |
|R1-R4     | Rotors (1-8, 1:beta 2:gamma) |

(All rotor settings must be in left-to-right order)

Examples:

```dos
./turing_bombe menu.txt 1 2 1 3
Bombe run takes 0.1127151 sec
1 2 1 3    BGX E:X
```

```dos
./turing_bombe test_menu.txt 1 1 2 4 1
Bombe run takes 6.6594702 sec
101 101 2 4 1    AXTW N:R
101 101 2 4 1    HJIE N:N
101 101 2 4 1    LIOE N:B
101 101 2 4 1    UIFO N:T
101 101 2 4 1    VJNG N:W
```
