cd backend
docker buildx build --platform linux/arm/v7 . -t victordavion/romcon-backend --push
docker buildx build --platform linux/arm64 . -t victordavion/romcon-backend --push

cd ..\frontend
docker buildx build --platform linux/arm/v7 . -t victordavion/romcon-frontend --push
docker buildx build --platform linux/arm64 . -t victordavion/romcon-frontend --push

cd ..\mqttDiscoveryService
docker buildx build --platform linux/arm/v7 . -t victordavion/romcon-discovery --push
docker buildx build --platform linux/arm64 . -t victordavion/romcon-discovery --push

cd ..\rabbitmq
docker buildx build --platform linux/arm/v7 . -t victordavion/romcon-rabbitmq --push
docker buildx build --platform linux/arm64 . -t victordavion/romcon-rabbitmq --push

cd ..\streamService
docker buildx build --platform linux/arm/v7 . -t victordavion/romcon-stream --push
docker buildx build --platform linux/arm64 . -t victordavion/romcon-stream --push
