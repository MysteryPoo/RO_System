# Reverse Osmosis Controller Web App

## Description
Multi-project directory structure.
### backend
This contains a NestJS service to act as an API bridge for the frontend to connect to the database and the mqtt broker.
### frontend
This service serves a Vue 3 frontend for system monitoring and configuration.
### mqttDiscoveryService
A UDP listener for the device to broadcast to requesting the IP and credentials of the mqtt broker
### rabbitmq
MQTT Plug-in enabled RabbitMQ service
### streamService
Microservice that listens for Particle cloud messages and mqtt broker messages to capture, process, and store in the database.

## How to run
Simply create a .env (dotenv) file with the required variables (shown below) followed by 'docker-compose up' and you're good to go.

### arm64 specfic
Using Ubuntu 20.04 on Raspberry Pi, the Pi is capable of building all images itself, however, there seems to be a power issue or something
as the 'backend' and 'frontend' services have a tendency to halt the system during build if they are already running. (Example: The services 
are already running and you perform a docker-compose up -d --build to update-in-place). I've avoided this by bringing the system down
(docker-compose down) first. This, unfortunately, means the service is down for approximately 10 minutes as it rebuilds (depending on changes).
This may or may not occur on a Raspberry Pi 4, but I don't have one to test. Since it's multiple core, I doubt it would thrash like the Pi 3.
If you want to avoid this, you can utilize docker buildx on a desktop/server for the arm64 arch and push changes by running docker-compose pull instead.
I use docker buildx to build the frontend/backend images for arm64 on an amd64 platform and push them to docker hub. Then, modify the docker-compose.yml to
not build the images locally by commenting out the 'build:' statements. Then create the .env file and run docker-compose up like normal and it should 
automatically pull the latest images from docker hub.
Ex.
docker buildx build --platform linux/arm/v7 . -t victordavion/romcon-backend --push
The batch file 'buildXplatform.bat' has been provided to easily one-click build all services for arm64

# .ENV Required Variables
## Required for Docker-Compose, runtime ENV
PARTICLE_USERNAME= # Username for particle cloud
PARTICLE_PASSWORD= # Password for particle cloud
API_SECRET= # Arbitrary secret used for Jwt
DOCKER_USERNAME= # Used to name the images on the docker repository
FRONTEND_PORT= # The port to host the front-end on, usually 80
BACKEND_PORT=4000 # Needs to be 4000 at the moment for the frontend. Not sure why I hardcoded it.
MONGODB_DB_NAME=
MONGODB_USERNAME=
MONGODB_PASSWORD=
MQTT_USERNAME=
MQTT_PASSWORD=

# .ENV Optional Variables
MQTT_OVERRIDE= # An IP address to an MQTT broker on a different sever than the mqttDiscovery service
