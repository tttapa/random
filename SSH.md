## SSH

### 1. Login to Helium
```sh
ssh -l <r-nummer> -X ssh.esat.kuleuven.be
```
`-l` = login name  
`-X` = X11 forwarding  
Type `yes` and hit enter to add the server's identity.  
Enter your KU Leuven password to log in.  
You shouldn't do any heavy tasks on Helium, it's just a gateway to reach other computers on the ESAT Intranet.

### 2. Login to a working computer
Either use the number-crunching server `vierre64`
```sh
ssh -X vierre64
```
Or login to a desktop in one of the PC rooms
```sh
ssh -X pc-klas<klas>-<computer>
```
You don't have to specify the login name, because you're already logged in to Helium with that username.  
For example, computer #3 of PC room 2.54 (room #3): `ssh -X pc-klas3-3`.  
You can see who else is logged in to that computer by using the `who` command. If there are many people using that computer already, you should try a different one.
