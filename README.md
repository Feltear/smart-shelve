# Étagère intelligente pour le [Lab-Usine](https://lab-usine.ulaval.ca)

Utilisant les capacités de [OpenFactory](https://github.com/Demo-Smart-Factory-Concordia-University/OpenFactory/blob/main/docs/architecture/architecture.md) en utilisant le [OpenFactory-SDK](https://github.com/Demo-Smart-Factory-Concordia-University/OpenFactory-SDK).

L'étagère utilise des capteurs afin de récupérer de l'information quant à la quantité de matériaux présent l'étagère puis de l'affiché dans un jumeaux numérique.

### Procédure de démarrage
* Dans `MTConnectAdapters/Sensor`, ouvrer le projet. Dans `src/main.cpp` changer les variables `ssid` et `password` pour votre réseau local Wifi (sans WAP2)
* Téléverser le code dans le ESP32
* Dans `OpenFactory-SDK`, ouvrir le devcontainer
* Dans `OpenFactory-SDK/ESP32_adapters/HCSR04.yml`, changer `ip` sous `adapter` pour entrer l'adresse IP du ESP32 sur le réseau local Wifi. 
* Dans le `bash` du devcontainer, entrer la commande `spinup`. Ceci permet de démarrer l'ensemble des services utilisés par OpenFactory
* Dans `OpenFactory-SDK/ESP32_adapters`, entrer la commande `openfactory-sdk device up HCSR04.yml`. Ceci permet de démarrer un agent MTConnect qui va écouter les envoies du capteur ultrason HCSR-04 sur l'addresse `ip:7878`.
* Dans `OpenFactory-SDK/app`, construire l'image Docker en entrant la commande `docker build -t product-quantity-monitoring .`
* Il est possible de modifier la config des produits et de l'étagère dans le fichier `config.json` (Peut être modifié après le démarrage aussi)
* Exécuter le docker en entrant la commande:
```
docker run -d \
  --name PRODUCT-QUANTITY \
  --network factory-net \
  -p 6789:6789 \
  -e APP_UUID=PRODUCT-QUANTITY \
  -e KAFKA_BROKER="localhost:9092,broker:29092" \
  -e KSQLDB_URL="http://localhost:8088" \
  -e DOCKER_SERVICE=product-quantity \
  -e KSQLDB_LOG_LEVEL=WARNING \
  -v $(pwd)/config.json:/app/config.json
  product-quantity-monitoring
```
Cette commande lance le même service qu'utilise OpenFactory pour créer une app, mais ajouter l'ouverture du port `6789` pour être utiliser comme WebSocket par la WebApp.

* Dans `smart-shelve-ui`, entrer `npm install` (Nécessite NPM installé au préalable)
* Démarrer le front-end en mode dev avec la commande `npm run dev`. La webapp devrait s'ouvrir à l'adresse `http://localhost:5173/`