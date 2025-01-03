import random
import requests
import time
import sys
import os

MSize = int(os.getenv('MSize', 4))
def send_messages_to_broker(PDtype):
    broker_url = 'http://consumer:8080'
    i = 0
    while i < MSize ** 2:
        if i % 100 == 0:
            print(f"Sending message {i}...")
        try:
            data = {
                'message_type': PDtype,
                'message_content': random.randint(1, 1000),
            }
            response = requests.post(broker_url, json=data)

            if response.status_code != 200:
                print(f"Received invalid response: {response.status_code}, {response.text}")
                continue

            while response.text == "-520":
                print("Received -520, retrying...")
                time.sleep(1)
                response = requests.post(broker_url, json=data)

            i += 1
        except requests.exceptions.RequestException as E:
            print(f"Error sending message: {E}")
            time.sleep(1)

    try:
        data = {
            'message_type': PDtype,
            'message_content': -1,
        }
        response = requests.post(broker_url + "/end", json=data)
        if response.status_code == 200:
            print("End signal sent successfully.")
        else:
            print(f"Error sending end signal: {response.status_code}, {response.text}")
    except requests.exceptions.RequestException as E:
        print(f"Error sending end message: {E}")
        time.sleep(1)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        try:
            arg = int(sys.argv[1])
            if arg not in [1, 2]:
                print("Invalid producer type. Should be 1 or 2.")
                sys.exit(1)
            send_messages_to_broker(arg)
        except ValueError:
            print("Invalid argument. Producer type should be an integer (1 or 2).")
            sys.exit(1)
    else:
        print("No arguments provided. Should be provided producer type: 1 or 2")
