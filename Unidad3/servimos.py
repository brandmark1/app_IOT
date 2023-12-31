# python 3.6

import requests
import json
import logging
import random
import time

from db_storage import DBStorage

from paho.mqtt import client as mqtt_client
from datetime import datetime, timedelta

BROKER = 'v3e23249.ala.us-east-1.emqxsl.com'
PORT = 8883
SUB_TOPIC = "monitores/#"
TOPIC = "monitores/YG"

# generate client ID with pub prefix randomly
CLIENT_ID = f'python-mqtt-tls-pub-sub-{random.randint(0, 1000)}'
USERNAME = 'server'
PASSWORD = 'server'

FIRST_RECONNECT_DELAY = 1
RECONNECT_RATE = 2
MAX_RECONNECT_COUNT = 12
MAX_RECONNECT_DELAY = 60

FLAG_EXIT = True

def on_connect(client, userdata, flags, rc):
    if rc == 0 and client.is_connected():
        print("Connected to MQTT Broker!")
        client.subscribe(SUB_TOPIC)
    else:
        print(f'Failed to connect, return code {rc}')


def on_disconnect(client, userdata, rc):
    logging.info("Disconnected with result code: %s", rc)
    reconnect_count, reconnect_delay = 0, FIRST_RECONNECT_DELAY
    while reconnect_count < MAX_RECONNECT_COUNT:
        logging.info("Reconnecting in %d seconds...", reconnect_delay)
        time.sleep(reconnect_delay)

        try:
            client.reconnect()
            logging.info("Reconnected successfully!")
            return
        except Exception as err:
            logging.error("%s. Reconnect failed. Retrying...", err)

        reconnect_delay *= RECONNECT_RATE
        reconnect_delay = min(reconnect_delay, MAX_RECONNECT_DELAY)
        reconnect_count += 1
    logging.info("Reconnect failed after %s attempts. Exiting...",
                 reconnect_count)
    global FLAG_EXIT
    FLAG_EXIT = True


def on_message(client, userdata, msg):
    print(f'Received {msg.payload.decode()} from {msg.topic} topic')
    try:
        message = msg.payload.decode()

        # decode json string to python dict
        try:
            msg_dict = json.loads(message)
        except json.decoder.JSONDecodeError:
            return

        # check if message is for me
        if msg_dict['to'] != "server":
            return

        # check if message has "action" key
        if "action" not in msg_dict:
            return

        if msg_dict["action"] == "SEND_DATA":
            # verify if message has "data" key
            if "data" not in msg_dict:
                return

            # verify if data is a dict
            if not isinstance(msg_dict["data"], dict):
                return

            # verify if data has "temperature" and "humidity" keys
            if "temperature" not in msg_dict["data"] or "humidity" not in msg_dict["data"]:
                return

            # TODO store data in database
            print("Storing data in database...")
            db = DBStorage("data.db")
            db.connect()
            db.create_table()
            db.insert(msg_dict["data"]["humidity"],
                      msg_dict["data"]["temperature"])
            print("Rose a guardado tus datos en la base de datos")
            db.disconnect()
        
        elif msg_dict["action"] == "GET_DATA":
            print("Obteniendo la data del server de YG")

            # Calcula la fecha de inicio (hace una hora atrás desde la fecha actual)
            fecha_inicio = datetime.now() - timedelta(hours=1)
            
            # La fecha final es la fecha actual
            fecha_final = datetime.now()

            db = DBStorage()
            db.connect()
            data = db.get_measurements(fecha_inicio, fecha_final)
            db.disconnect()
            print("Perfecto hemos logrado extraer los datos de la última hora")
            msg_dict = {"from":"server","to":"web","action":"SEND_DATA","data": data}
            print("Miercoles creo que nos equivocamos y no son datos bancarios T_T")
            out_msg = json.dumps(msg_dict)
            client.publish(msg.topic, out_msg)
        
        elif msg_dict["action"] == "GET_JOKE":
            try:
                url = "https://matchilling-chuck-norris-jokes-v1.p.rapidapi.com/jokes/random"

                headers = {
                    "accept": "application/json",
                    "X-RapidAPI-Key": "cfd2abb8b9msh30bd50d80bc3141p1cd901jsnc18b6fbb4199",
                    "X-RapidAPI-Host": "matchilling-chuck-norris-jokes-v1.p.rapidapi.com"
                }

                # Realizar la solicitud a la API de chistes
                response = requests.get(url, headers=headers)
                response.raise_for_status()  # Lanza una excepción en caso de error HTTP

                # Parsamos la respuesta a un JSON
                # joke_data = response.json()

                if response.status_code == 200:
                    # pasamos la respuesta JSON
                    joke_data = response.json()

                    # Accedemos al valor de la broma
                    joke_value = joke_data['value']

                    # Imprimimos la broma
                    print(f"Broma: {joke_value}")
                    # Crear un nuevo mensaje con el chiste
                    joke_msg = {"from": "server", "to": "web", "action": "SEND_JOKE", "joke": joke_value}

                    # Convertir el mensaje a formato JSON
                    out_msg = json.dumps(joke_msg)

                    # Publicar el chiste como mensaje MQTT
                    client.publish(TOPIC, out_msg)
                    print("Chiste enviado con éxito")
                else:
                    print(f"Error en la solicitud. Código de respuesta: {response.status_code}")
                    print("No se recibieron chistes de la API")

            except requests.exceptions.RequestException as e:
                print(f"Error al obtener chiste: {e}")

    except Exception as e:
        print(e)


def connect_mqtt():
    client = mqtt_client.Client(CLIENT_ID)
    client.tls_set(ca_certs='Unidad3\mqtt\emqxsl-ca.crt')
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER, PORT, keepalive=120)
    client.on_disconnect = on_disconnect
    return client


def publish(client):
    msg_count = 0
    while not FLAG_EXIT:
        msg_dict = {'msg': msg_count, 'from': 'server', 'to': 'client'}
        msg = json.dumps(msg_dict)
        if not client.is_connected():
            logging.error("publish: MQTT client is not connected!")
            time.sleep(1)
            continue
        result = client.publish(TOPIC, msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            # print(f'Send `{msg}` to topic `{TOPIC}`')
            pass
        else:
            print(f'Failed to send message to topic {TOPIC}')
        msg_count += 1
        time.sleep(1)


def run():
    logging.basicConfig(
        format='%(asctime)s - %(levelname)s: %(message)s', level=logging.DEBUG)
    client = connect_mqtt()
    client.loop_start()
    time.sleep(1)
    if client.is_connected():
        publish(client)
    else:
        client.loop_stop()


if __name__ == '__main__':
    run()
    while True:
        continue