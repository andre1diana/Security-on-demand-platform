##SafeZone App

SafeZone este o aplicatie security-on-demand cu urmatoarele functii(optiuni de meniu):
--Encrypt file  -AES_256_CBC
                -AES_256_OFB
                -TripleDES
--Decrypt file
--Hash on files -SHA1
                -SHA2
                -SHA3
--MAC on files
--Generate keys -RSA 
                -DSA
--Generate self-signed certificates

Versions : 
Version 1.0 --sunt implementate si testate toate functiile criptografice ale menuiului pe server //all the menu options are implemented on the server app and tested
Version 1.1 --implementarea clientului si crearea conexiunii dintre client si server folosind Sockets //implement the client and create and test the connection with server
Version 2.0 --crearea GUI pentru client si configurarea optiunilor din meniu pentru a functiona intre server-client //create GUI and configure the menu options to work for the client

##Scopul proiectului
Acest proiect are ca scop oferirea de optiuni de securitate. Platforma este dezvoltata astfel incat datele sa fie transmise in siguranta de la server la client. Aceasta aplicatie nu isi propune sa fie ca cele din categoria "CyberChef", ci mai mult o aplicatie ce ofera posibilitatea creari unei protectii pentru fisierele tale. Aplicatia ofera o gama variata de optiuni din care clientul poate alege.

##Implementari functionale
1. Clientul poate incarca orice fisier din calculator pentru a efectua operatii asupra lui
2. Aplicatia este proiectata ca doar fisierele encriptate de client sa poata fi si decriptate mai apoi, daca fisierul nu a fost encriptat mai inainte, atunci el nu poate fi decriptat de aplicatie
3. Generarea cheii se face pe plan local, insa daca clientul doreste sa o descarce in fisierele lui, este posibil apasand butonul de Download
4. Fisierele create cu celelalte optiuni sunt descarcate automat in baza clientului
5. Manipularea erorilor si a logurilor sistemului intr-un fisier
6. Optiunea de logare a clientilor care au deja cont
7. Optiunea de SignUp a clientilor noi care vor sa intre in aplicatie
8. Opiunile de encriptare efectuate asupra fisierelor. Atunci cand este apasat butonul de sendFile din fereastra nou deschisa(dupa alegerea meniului dorit) fisierul selectat este trimis la server si primit inapoi in timp util si deschis automat de catre sistem. Cheia si iv-ul sunt generate random de catre sistem la efectuarea procesului de encriptare si salvate in server
9. Optiunile de hash si mac asupra fisierelor. Clientul va primi un fisier inapoi care contine digest-ul rezultat in urma hash-ului. In cazul optiunii MAC, fisierul va contine si cheia cu care a fost realizat MAC-ul.
10. Optiunea de generare de chei expune 2 variante -RSA/DSA- cu mai multe optiuni de dimensiuni de chei -1024,2048,3096- ce sunt la alegerea clientului. Fisierul primit va contine si cheia publica si cea privata. Acestea pot fi separate ce catre client
11. Optiunea de generare de certificate. Cleintul trebuie sa aleaga o cheie deja existenta in baza lui de date pentru a semna certificatul. De asemenea trebuie sa completeze datele certificatului. Certificatul va fi primit automat inapoi la client dupa crearea acestuia cu extensia .pem si numele ales de client.

##Nefunctionalitati ale aplicatiei
1. Incarcarea necorspunzatoare a datelor in fisier la oprirea aplicatiei pentru a fi salvate si incarcate inapoi atunci cand porneste aplicatia 
2. Stergerea fisierelor din server dupa ce acestea devin nefolositoare