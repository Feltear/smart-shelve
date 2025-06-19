import os
import time
import csv
import websockets
import asyncio
import json

from openfactory.apps import OpenFactoryApp
from openfactory.kafka import KSQLDBClient
from openfactory.assets import Asset, AssetAttribute


connected_clients = set()
last_values = {}
previous_values = {}
currentconfig = {}


class ProductQuantityMonitoring(OpenFactoryApp):

    SENSOR_UUID: str = os.getenv(
        'SENSOR_UUID', 'HCSR04_ULTRASOUND_DISTANCE_SENSOR')

    def __init__(self, app_uuid, ksqlClient, bootstrap_servers, loglevel='INFO'):
        super().__init__(app_uuid, ksqlClient, bootstrap_servers, loglevel)

        print(self.SENSOR_UUID)

        currentconfig = load_config()

        self.add_attribute('distance_system', AssetAttribute(
            self.SENSOR_UUID,
            type='Events',
            tag='DeviceUuid'))

        self.sensor = Asset(self.SENSOR_UUID,
                            ksqlClient=ksqlClient,
                            bootstrap_servers=bootstrap_servers)

        self.verify_sensor_availability()

        self.sensor.subscribe_to_events(self.on_event, 'sensor_events')
        self.sensor.subscribe_to_samples(self.on_sample, 'sensor_data')

    def app_event_loop_stopped(self) -> None:
        """
        Called automatically when the main application event loop is stopped.

        This method handles cleanup tasks such as stopping the temperature
        sensor's sample subscription to ensure a graceful shutdown.
        """
        self.sensor.stop_events_subscription()

    def main_loop(self) -> None:
        """ Main loop of the App. """
        asyncio.run(start_websocket())

    def on_event(self, msg_key: str, msg_value: dict) -> None:

        print((msg_key, msg_value['value']))

    def on_sample(self, msg_key: str, msg_value: dict) -> None:

        print(msg_key, msg_value['value'])

        try:
            distance = float(msg_value["value"])
            config = load_config()
            print(config["shelf"]["shelves"]["shelf_1_height"],
                  config["products"][0]["thickness"])
            quantity = self.measureQuantity(
                distance, config["shelf"]["shelves"]["shelf_1_height"], config["products"][0]["thickness"])
            if last_values.get("distance_1") != distance:
                last_values["distance_1"] = distance
            if last_values.get("product_1_quantity") != quantity:
                last_values["product_1_quantity"] = quantity
        except ValueError:
            print("Not a float")

    def verify_sensor_availability(self) -> None:
        print(
            f"Current sensor state: {self.sensor.__getattr__('avail').value}")

    def measureQuantity(self, distance: float, reference: float, productThickness: float):
        if (reference < distance) or productThickness <= 0:
            return 0

        thickness_difference = reference - distance

        # Round down the product quantity
        productQuantity = round(thickness_difference/productThickness)

        return productQuantity


async def websocket_handler(websocket):
    connected_clients.add(websocket)
    print(f"[WebSocket] Client connecté: {websocket.remote_address}")
    try:
        # Lancer les deux tâches en parallèle
        listener_task = asyncio.create_task(handle_client_messages(websocket))
        sender_task = asyncio.create_task(send_periodic_data(websocket))

        # Attendre que l’une se termine (ex : client déconnecté)
        done, pending = await asyncio.wait(
            [listener_task, sender_task],
            return_when=asyncio.FIRST_COMPLETED
        )

        # Annuler les tâches restantes proprement
        for task in pending:
            task.cancel()

    finally:
        connected_clients.remove(websocket)
        print("🔌 Client déconnecté")


async def start_websocket():
    server = await websockets.serve(websocket_handler, "0.0.0.0", 6789)
    print("Serveur WebSocket démarré sur ws://0.0.0.0:6789")
    await server.wait_closed()


def load_config(path="/app/config.json"):
    with open(path, "r") as f:
        config = json.load(f)

    return config


async def handle_client_messages(websocket):
    async for message in websocket:
        print("[Client → Serveur] Message reçu :", message)

        if message == "load_config":
            try:
                currentconfig = load_config()
                print(currentconfig)

                await websocket.send(json.dumps({
                    "type": "config_data",
                    "payload": currentconfig
                }))
            except Exception as e:
                await websocket.send(json.dumps({
                    "type": "error",
                    "message": str(e)
                }))
        elif message == "calibrate_shelves_height":
            try:
                currentconfig["shelf"]["shelves"]["shelf_1_height"] = last_values["distance_1"]
                DISTANCE_REF_1 = last_values["distance_1"]
                last_values["product_1_quantity"] = 0
                with open("/app/config.json", "w") as f:
                    json.dump(currentconfig, f, indent=2)
                await websocket.send(json.dumps({
                    "type": "save_status",
                    "message": "✅ Configuration sauvegardée"
                }))
            except Exception as e:
                await websocket.send(json.dumps({
                    "type": "error",
                    "message": f"Erreur d’écriture : {e}"
                }))
        else:
            await websocket.send(json.dumps({
                "type": "error",
                "message": f"Commande inconnue : {message}"
            }))


async def send_periodic_data(websocket):
    while True:
        changed_data = {}

        for key, value in last_values.items():
            if previous_values.get(key) != value:
                changed_data[key] = value
                previous_values[key] = value

        if changed_data:
            print(f"[Serveur → Client] Données envoyées : {changed_data}")
            await websocket.send(json.dumps(changed_data))

        await asyncio.sleep(0.5)

app = ProductQuantityMonitoring(
    app_uuid='PRODUCT-QUANTITY',
    ksqlClient=KSQLDBClient("http://ksqldb-server:8088"),
    bootstrap_servers="broker:29092"
)

app.run()
