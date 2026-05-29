# Traceroute
Programming my own traceroute using raw socket

## Presentation de Traceroute

Voila un exemple d'utilisation de traceroute:
```bash
traceroute -4 google.com
traceroute to google.com (142.251.208.238), 30 hops max, 60 byte packets
 1  _gateway (10.0.2.2)  0.158 ms  0.154 ms  0.147 ms
 2  _gateway (10.12.254.254)  0.724 ms  0.795 ms  0.977 ms
 3  nat.42nice.fr (10.60.1.1)  0.443 ms  0.433 ms  0.426 ms
 4  62.129.8.169 (62.129.8.169)  1.451 ms  1.614 ms  1.810 ms
 5  * * *
 6  * * *
 7  * * *
 8  217.180.129.112 (217.180.129.112)  3.948 ms  4.119 ms  4.303 ms
 9  * * *
10  192.178.105.27 (192.178.105.27)  3.982 ms 192.178.105.91 (192.178.105.91)  3.076 ms 192.178.105.27 (192.178.105.27)  4.196 ms
11  142.251.78.83 (142.251.78.83)  3.025 ms 142.251.78.81 (142.251.78.81)  3.074 ms 142.251.78.83 (142.251.78.83)  3.074 ms
12  ncmrsa-aq-in-f14.1e100.net (142.251.208.238)  3.064 ms  3.044 ms  3.059 ms
```
>[!NOTE]
> L'argument -4 permet de specifier l'utilisation de l'ipv4, comme demande dans le sujet

## Décryptage du comportement réseau

### 1. L'En-tête
> `traceroute to google.com (142.251.208.238), 30 hops max, 60 byte packets`

* **Résolution DNS :** Le domaine cible `google.com` a été résolu par le système vers l'adresse IP `142.251.208.238`.
* **Configuration des sondes :** La limite maximale de sauts (TTL max) est définie à `30` et chaque paquet réseau envoyé pèse `60 octets`.

### 2. Le Réseau Local (Sauts 1 à 3)
Les premières étapes s'effectuent au sein de l'infrastructure locale (dans cet exemple, le réseau de l'école 42 Nice).
* **Sauts 1 & 2 (`10.0.2.2` & `10.12.254.254`) :** Plages d'adresses IP privées. Le premier saut correspond généralement à la passerelle de la machine virtuelle (NAT de l'hyperviseur), tandis que le second est le routeur de l'infrastructure interne.
* **Saut 3 (`nat.42nice.fr - 10.60.1.1`) :** La passerelle NAT principale de l'école, représentant le point de sortie ("Default Gateway") vers l'Internet public.
* **Mesures RTT (Round Trip Time) :** L'outil envoie par défaut 3 paquets de sonde par niveau de TTL. Les trois valeurs en millisecondes (`ms`) correspondent aux temps de réponse de chaque paquet. En local, la latence est extrêmement faible (inférieure à 1 ms).

### 3. Filtrage et Sécurité (Sauts 5, 6, 7 et 9)
> `5  * * *`

* **Comportement :** L'outil affiche des astérisques `* * *` lorsqu'il n'obtient aucune réponse avant l'expiration de son délai d'attente (*timeout*).
* **Explication :** De nombreux routeurs de transit (notamment sur le cœur de réseau des opérateurs internet) sont configurés pour ignorer les paquets dont le TTL expire ou pour bloquer explicitement l'émission de messages **ICMP Time Exceeded (Type 11)**. Cela permet de préserver les ressources CPU du routeur et de masquer la topologie exacte de l'infrastructure par mesure de sécurité.

### 4. Répartition de Charge / Load Balancing (Sauts 10 & 11)
> `10  192.178.105.27 (...)  192.178.105.91 (...)  192.178.105.27 (...)`

* **Comportement :** Pour un même saut (TTL identique), les trois sondes renvoient des adresses IP distinctes (ici `.27` et `.91`).
* **Explication :** Les paquets entrent ici dans l'infrastructure réseau de Google. Afin de traiter d'immenses volumes de données, des routeurs équipés de mécanismes de répartition de charge (*Load Balancing* ou routage *ECMP / Anycast*) distribuent le trafic de manière dynamique. Les trois paquets consécutifs ont donc emprunté des chemins physiques différents.

### 5. La Destination Finale (Saut 12)
> `12  ncmrsa-aq-in-f14.1e100.net (142.251.208.238)  3.064 ms`

* **Terminus :** L'adresse IP qui répond au saut 12 concorde exactement avec la cible initiale résolue dans l'en-tête.
* **Mécanisme d'arrêt :** Le serveur final ne renvoie pas une erreur de dépassement de temps (TTL Exceeded), mais répond directement à la requête de la sonde via un message de succès (comme un **ICMP Echo Reply (Type 0)** pour un traceroute basé sur ICMP). L'outil intercepte cette réponse finale, identifie que la cible est atteinte et interrompt proprement sa boucle d'exécution.