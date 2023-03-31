
docker buildx create --use --name romcon_builder default
docker buildx bake --push --set *.platform=linux/amd64,linux/arm64,linux/arm/v7
docker buildx rm romcon_builder
