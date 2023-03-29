cd backend
docker buildx build --platform linux/arm64 . -t victordavion/romcon-backend --push

cd ..\frontend
docker buildx build --platform linux/arm64 . -t victordavion/romcon-frontend --push

cd ..\MQTT_Discovery_Service
docker buildx build --platform linux/arm64 . -t victordavion/romcon-discovery --push

cd ..\rabbitmq
docker buildx build --platform linux/arm64 . -t victordavion/romcon-rabbitmq --push

cd ..\streamService
docker buildx build --platform linux/arm64 . -t victordavion/romcon-stream --push
