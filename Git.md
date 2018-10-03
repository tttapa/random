## 0. Installeer git
### Ubuntu/Debian
```sh
sudo apt install git
```
### Windows
[https://git-scm.com/downloads](https://git-scm.com/downloads)  
Open daarna Git Bash.

## 1. Create SSH key pair and add to your GitLab account
[https://gitlab.esat.kuleuven.be/help/ssh/README](https://gitlab.esat.kuleuven.be/help/ssh/README)

## 2. Download de hele repository
```sh
cd ~/Documents/KUL/2018-2019/P_O/  # Ga naar de map waar je de EAGLE repository wilt opslaan
git clone git@gitlab.esat.kuleuven.be:EAGLE-gitlab/EAGLE1/EAGLE-students-2018.git
cd EAGLE-students-2018  # Ga naar de map van de EAGLE repository
```
Alle code en de geschiedenis van de repository van ons team staat nu lokaal op je computer. Je kan nu lokaal wijzigingen maken en toevoegen aan de repository.

## 3. Voeg de algemene EAGLE repository toe als `remote`
Je kan je lokale repository syncen met de versie op de ESAT-servers. Die remote repository heet `origin`.  
Aangezien updates en patches aan de opgave niet in de repository van ons team gebeuren, moeten we de algemene EAGLE repository ook toevoegen als remote, zodat we de nieuwe wijzigingen ervan kunnen ophalen, en lokaal toepassen.  
We voegen een nieuwe remote toe die we `upstream` noemen.  
```sh
git remote add upstream git@gitlab.esat.kuleuven.be:EAGLE-gitlab/EAGLE-students-2018.git
git remote -v  # Toon de remotes
```
De output van het tweede commando zou er zo uit moeten zien:
```
origin	git@gitlab.esat.kuleuven.be:EAGLE-gitlab/EAGLE1/EAGLE-students-2018.git (fetch)
origin	git@gitlab.esat.kuleuven.be:EAGLE-gitlab/EAGLE1/EAGLE-students-2018.git (push)
upstream	git@gitlab.esat.kuleuven.be:EAGLE-gitlab/EAGLE-students-2018.git (fetch)
upstream	git@gitlab.esat.kuleuven.be:EAGLE-gitlab/EAGLE-students-2018.git (push)
```
## 4. Haal de laatste wijzigingen op uit de algemene repository
Wanneer de opgave verandert, of als er bestanden worden toegevoegd, moeten we onze team-repository bijwerken.
```sh
git fetch upstream         # Download de wijzigingen van de algemene repository, zonder ze lokaal toe te passen
git checkout master        # Ga naar de master branch van je lokale team-repository
git merge upstream/master  # Voeg de wijzigingen van de algemene repository samen met de lokale master branch
git push origin master     # Upload de wijzigingen van de lokale master branch naar de team-repository op de ESAT-server (origin)
```
## 5. Maak een feature branch aan
Het is geen goed idee om op de master branch te werken, dus maak een nieuwe branch aan waaraan je werkt:
```sh
git checkout -b feature  # Vervang 'feature' door een duidelijke naam (zonder spaties)
```
`git checkout` wordt gebruikt om naar een andere branch te gaan, of om een specifieke versie van een bestand te openen, maar kan ook gebruikt worden om een nieuwe branch aan te maken, zoals hierboven.

## 6. Stage en commit nieuwe bestanden
Als je nieuwe bestanden toevoegt, moet je git vertellen dat hij ze moet bijhouden. Nieuwe bestanden die nog niet gekend zijn door git zijn 'untracked files'.  
Als je bijvoorbeeld een nieuw bestand 'test.txt' aanmaakt, en `git status` uitvoert:
```sh
git status
```
```
Untracked files:
  (use "git add <file>..." to include in what will be committed)

	test.txt

nothing added to commit but untracked files present (use "git add" to track)
```
Met `git add` kan je nieuwe bestanden toevoegen aan de index. De index is waar je je commit voorbereidt.
```sh
git add test.txt
git status
```
```
Changes to be committed:
  (use "git rm --cached <file>..." to unstage)

	new file:   test.txt

```
Het nieuwe bestand is nu 'staged' (toegevoegd aan de index).

Als er tenminste één wijziging in de index zit, kan je een commit doen. Dit slaat de wijzigingen in de index op. Elke commit heeft een unieke code, een auteur en een commit message. Later kan je nakijken welke wijzigingen door wie, wanneer, en in welke commit gemaakt werden. Als er bijvoorbeeld ergens een bug geïntroduceerd werd, kan je teruggaan naar de commit ervoor.  
Het is belangrijk om regelmatig een commit te doen, met een duidelijke boodschap voor de rest van het team (en voor jezelf) die kort zegt wat je veranderd hebt, en eventueel waarom. Het is natuurlijk niet de bedoeling dat je voor elke lijn die je aanpast een nieuwe commit aanmaakt, dan wordt het te onoverzichtelijk.

```sh
git commit -m "Fix buffer overflow logging system"
```

## 7. Commit gewijzigde bestanden
Elke keer expliciet ingeven welke bestanden gestaged moeten worden is nogal omslachtig. Meestal doe je het dan ook alleen maar voor nieuwe bestanden, of als je wilt kiezen welke wijzigingen je wilt toevoegen en welke niet.  
Wanneer je enkel bestanden hebt gewijzigd die al bijgehouden worden door git, kan je de optie `-a` gebruiken bij het commit commando.

```sh
git commit -am "<message>"
```

## 8. Merge de master branch in je feature branch
Het is belangrijk dat je feature branch niet te veel achter loopt op de master branch van de rest van het team. Daarom moet je af en toe de wijzigingen op de master branch toepassen op de feature branch door een merge.  
Als jij bijvoorbeeld werkt aan de integratie van de encryptie, is het misschien handig dat je steeds de nieuwste versie van de QR-lezer hebt, die door andere teamleden wordt ontwikkeld.
```sh
git fetch                # Haal de laatste wijzigingen op van de ESAT server
git merge origin/master  # Pas de nieuwe wijzigingen van de master branch toe op je huidige branch
```

## 9. Upload je feature branch naar de ESAT server
Het is een goed idee om je feature branch niet alleen lokaal bij te houden, maar ook naar de ESAT server te uploaden. Zo heb je een back-up, en kunnen je teamleden volgen wat je doet.

De eerste keer dat je dit doet, moet je aangeven naar welke remote je je branch wilt uploaden. Dit moet `origin` zijn, de ESAT server.
```sh
git push --set-upstream origin feature  # Vervang 'feature' door de naam van je feature branch
```
Nadien kan je gewoon pushen zonder de remote te specifiëren:
```sh
git push
```
## 10. Merge je feature branch in de master branch
Wanneer je feature klaar is, moet je je wijzigingen toepassen op de master branch.  
Zorg eerst dat je alle wijzigingen van de master branch toepast op de feature branch (zie stap 8).  
Merge daarna de feature branch in de lokale master branch:
```sh
git checkout master  # Ga naar de master branch
git pull             # Maak de lokale master branch up to date met die op de ESAT server
git merge feature    # Merge de feature branch in de master branch
```
Daarna kan je de master branch uploaden naar de ESAT server:
```sh
git push
```

## Addendum: merge conflicts
Wanneer je twee branches merget, kan het zijn dat op beide branches dezelfde lijn code gewijzigd is, door twee verschillende personen. In dit geval moet je handmatig kiezen welke van de twee wijzigingen je wilt behouden.

Als er een merge conflict optreedt, zal git dit duidelijk zeggen, en je kan met git status kijken om welke bestanden het gaat.  
Open de bestanden, en verwijder de wijzigingen die je niet wilt. Git geeft aan welke wijzigingen van welke branch komen door `<<<<<`, `=====` en `>>>>>`, doe die tekens ook weg.  
Sla het bestand op, en voeg het toe aan de index door middel van `git add`.  
Daarna kan je ofwel committen, of `git merge --continue` doen.
